
#ifndef SURVEY6_ACTIVE_PING_H
#define SURVEY6_ACTIVE_PING_H
#ifndef _GNU_SOURCE
#define _GNU_SOURCE /* for additional type definitions */
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <errno.h>
#include <fcntl.h>            /* fcntl() */
#include <netdb.h>            /* getaddrinfo() */
#include <stdint.h>
#include <unistd.h>
#include <arpa/inet.h>        /* inet_XtoY() */
#include <netinet/in.h>       /* IPPROTO_ICMP */
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>  /* struct icmp */
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>

using namespace std;

typedef int socket_t;
typedef struct msghdr msghdr_t;
typedef struct cmsghdr cmsghdr_t;

#define IP_VERSION_ANY 0
#define IP_V4 4
#define IP_V6 6

#define ICMP_HEADER_LENGTH 8
#define MESSAGE_BUFFER_SIZE 1024

#ifndef ICMP_ECHO
#define ICMP_ECHO 8
#endif
#ifndef ICMP6_ECHO
#define ICMP6_ECHO 128
#endif
#ifndef ICMP_ECHO_REPLY
#define ICMP_ECHO_REPLY 0
#endif
#ifndef ICMP6_ECHO_REPLY
#define ICMP6_ECHO_REPLY 129
#endif

#define REQUEST_TIMEOUT 1000000
#define REQUEST_INTERVAL 1000000

#define close_socket close

#pragma pack(push, 1)

struct ip6_pseudo_hdr {
    struct in6_addr src;
    struct in6_addr dst;
    uint8_t unused1[2];
    uint16_t plen;
    uint8_t unused2[3];
    uint8_t nxt;
};

struct icmp6_packet {
    struct ip6_pseudo_hdr ip6_hdr;
    struct icmp icmp;
};

#pragma pack(pop)

#define psockerror perror
/**
 * Returns a timestamp with microsecond resolution.
 */
static uint64_t utime(void);
/**
 * Computes the checksum
 */
static uint16_t compute_checksum(const char *buf, size_t size);

/**
 * ping the given IPV6 address and check its' reachability.
 */
bool ping(string address) ;

#endif //SURVEY6_ACTIVE_PING_H
