#include <survery6_active/ping.h>

static uint64_t utime(void)
{
    struct timeval now;
    return gettimeofday(&now, NULL) != 0
           ? 0
           : now.tv_sec * 1000000 + now.tv_usec;
}



static uint16_t compute_checksum(const char *buf, size_t size)
{
    /* RFC 1071 - http://tools.ietf.org/html/rfc1071 */

    size_t i;
    uint64_t sum = 0;

    for (i = 0; i < size; i += 2) {
        sum += *(uint16_t *)buf;
        buf += 2;
    }
    if (size - i > 0)
        sum += *(uint8_t *)buf;

    while ((sum >> 16) != 0)
        sum = (sum & 0xffff) + (sum >> 16);

    return (uint16_t)~sum;
}

bool ping(string address) {
    int i;
    int ip_version = IP_VERSION_ANY;
    int error;
    socket_t sockfd = -1;
    struct addrinfo *addrinfo_list = NULL;
    struct addrinfo *addrinfo;
    char addr_str[INET6_ADDRSTRLEN] = "<unknown>";
    struct sockaddr_storage addr;
    socklen_t dst_addr_len;
    uint16_t id = (uint16_t)getpid();
    uint16_t seq;
    uint64_t start_time;
    uint64_t delay;
    bool success= false;


    if (ip_version == IP_V6
        || (ip_version == IP_VERSION_ANY && error != 0)) {
        struct addrinfo hints = {0};
        hints.ai_family = AF_INET6;
        hints.ai_socktype = SOCK_RAW;
        hints.ai_protocol = IPPROTO_ICMPV6;
        error = getaddrinfo(address.c_str(),
                            NULL,
                            &hints,
                            &addrinfo_list);
    }
    if (error != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(error));
        goto exit_error;
    }

    for (addrinfo = addrinfo_list;
         addrinfo != NULL;
         addrinfo = addrinfo->ai_next) {
        sockfd = socket(addrinfo->ai_family,
                        addrinfo->ai_socktype,
                        addrinfo->ai_protocol);
        if (sockfd >= 0) {
            break;
        }
    }

    if ((int)sockfd < 0) {
        psockerror("socket");
        goto exit_error;
    }

    memcpy(&addr, addrinfo->ai_addr, addrinfo->ai_addrlen);
    dst_addr_len = (socklen_t)addrinfo->ai_addrlen;

    freeaddrinfo(addrinfo_list);
    addrinfo = NULL;
    addrinfo_list = NULL;



    /*
     * Switch the socket to non-blocking I/O mode. This allows us to implement
     * the timeout feature.
     */
    if (fcntl(sockfd, F_SETFL, O_NONBLOCK) == -1) {
        psockerror("fcntl");
        goto exit_error;
    }

    if (addr.ss_family == AF_INET6) {
        /*
         * This allows us to receive IPv6 packet headers in incoming messages.
         */
        int opt_value = 1;
        error = setsockopt(sockfd,
                           IPPROTO_IPV6,
                           IPV6_RECVPKTINFO,
                           (char *)&opt_value,
                           sizeof(opt_value));
        if (error != 0) {
            psockerror("setsockopt");
            goto exit_error;
        }
    }

    /*
     * Convert the destination IP-address to a string.
     */
    inet_ntop(addr.ss_family,
              addr.ss_family == AF_INET6
              ? (void *)&((struct sockaddr_in6 *)&addr)->sin6_addr
              : (void *)&((struct sockaddr_in *)&addr)->sin_addr,
              addr_str,
              sizeof(addr_str));


    for (seq = 0;(seq<10 && !success ); seq++) {
        struct icmp request;

        request.icmp_type = ICMP6_ECHO;
        request.icmp_code = 0;
        request.icmp_cksum = 0;
        request.icmp_id = htons(id);
        request.icmp_seq = htons(seq);
/*
     * Checksum is calculated from the ICMPv6 packet prepended  with an IPv6 "pseudo-header".
*/
        struct icmp6_packet request_packet = {0};

        request_packet.ip6_hdr.src = in6addr_loopback;
        request_packet.ip6_hdr.dst =
                ((struct sockaddr_in6 *)&addr)->sin6_addr;
        request_packet.ip6_hdr.plen = htons((uint16_t)ICMP_HEADER_LENGTH);
        request_packet.ip6_hdr.nxt = IPPROTO_ICMPV6;
        request_packet.icmp = request;

        request.icmp_cksum = compute_checksum((char *)&request_packet,
                                              sizeof(request_packet));
        error = (int)sendto(sockfd,
                            (char *)&request,
                            sizeof(request),
                            0,
                            (struct sockaddr *)&addr,
                            (int)dst_addr_len);
        if (error < 0) {
            psockerror("sendto");
            goto exit_error;
        }

        start_time = utime();

        for (;;) {
            char msg_buf[MESSAGE_BUFFER_SIZE];
            char packet_info_buf[MESSAGE_BUFFER_SIZE];
            struct in6_addr msg_addr = {0};
            struct iovec msg_buf_struct = {
                    msg_buf,
                    sizeof(msg_buf)
            };
            struct msghdr msg = {
                    NULL,
                    0,
                    &msg_buf_struct,
                    1,
                    packet_info_buf,
                    sizeof(packet_info_buf),
                    0
            };
            size_t msg_len;
            cmsghdr_t *cmsg;
            size_t ip_hdr_len;
            struct icmp *reply;
            int reply_id;
            int reply_seq;
            uint16_t reply_checksum;
            uint16_t checksum;

            error = (int)recvmsg(sockfd, &msg, 0);
            delay = utime() - start_time;

            if (error < 0) {
                if (errno == EAGAIN) {
                    if (delay > REQUEST_TIMEOUT) {
                        goto next;
                    } else {
                        /* No data available yet, try to receive again. */
                        continue;
                    }
                } else {
                    psockerror("recvmsg");
                    goto next;
                }
            }

            /*
          * The IP header is not included in the message, msg_buf points
          * directly to the ICMP data.
          */
            ip_hdr_len = 0;

            /*
             * Extract the destination address from IPv6 packet info. This
             * will be used to compute the checksum later.
             */
            for (
                    cmsg = CMSG_FIRSTHDR(&msg);
                    cmsg != NULL;
                    cmsg = CMSG_NXTHDR(&msg, cmsg))
            {
                if (cmsg->cmsg_level == IPPROTO_IPV6
                    && cmsg->cmsg_type == IPV6_PKTINFO) {
                    struct in6_pktinfo *pktinfo = (in6_pktinfo *)CMSG_DATA(cmsg);
                    memcpy(&msg_addr,
                           &pktinfo->ipi6_addr,
                           sizeof(struct in6_addr));
                }
            }
            reply = (struct icmp *)(msg_buf + ip_hdr_len);
            reply_id = ntohs(reply->icmp_id);
            reply_seq = ntohs(reply->icmp_seq);

            /*
             * Verify that this is indeed an echo reply packet.
             */
            if (!(addr.ss_family == AF_INET
                  && reply->icmp_type == ICMP_ECHO_REPLY)
                && !(addr.ss_family == AF_INET6
                     && reply->icmp_type == ICMP6_ECHO_REPLY)) {
                continue;
            }

            /*
             * Verify the ID and sequence number to make sure that the reply
             * is associated with the current request.
             */
            if (reply_id != id || reply_seq != seq) {
                continue;
            }

            reply_checksum = reply->icmp_cksum;
            reply->icmp_cksum = 0;

            /*
             * Verify the checksum.
             */
            size_t size = sizeof(struct icmp6_packet);
            struct icmp6_packet *reply_packet =(icmp6_packet* ) malloc(size);

            if (reply_packet == NULL) {
                 psockerror("malloc");
                goto exit_error;
            }
            memcpy(&reply_packet->ip6_hdr.src,
                   &((struct sockaddr_in6 *)&addr)->sin6_addr,
                   sizeof(struct in6_addr));
            reply_packet->ip6_hdr.dst = msg_addr;
            reply_packet->ip6_hdr.plen = htons((uint16_t)msg_len);
            reply_packet->ip6_hdr.nxt = IPPROTO_ICMPV6;
            memcpy(&reply_packet->icmp,
                   msg_buf + ip_hdr_len,
                   msg_len - ip_hdr_len);

            checksum = compute_checksum((char *)reply_packet, size);

            success=true;
            break;
        }

        next:
        if (delay < REQUEST_INTERVAL) {
            usleep(REQUEST_INTERVAL - delay);
        }
    }

    close_socket(sockfd);
    return success;

    exit_error:

    if (addrinfo_list != NULL) {
        freeaddrinfo(addrinfo_list);
    }

    close_socket(sockfd);

    return EXIT_FAILURE;
}