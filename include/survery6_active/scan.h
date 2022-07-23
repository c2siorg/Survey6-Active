#include <string>
#include <boost/asio.hpp>

using namespace boost::asio;
using namespace std;
#ifndef SCAN_H
#define SCAN_H


/**
 * Usage:
* This utility generates the ip addresses in the given network address range for scan purposes
*
* arguments
* network_range string     The IPv6 CIDR range to scan.

*/

ip::address_v6_range generate_ip_addresses(string network_range);



/**
 * Usage:
* This utility scans for live hosts over IPv6 based on the network range you specify. If no range is
* specified, then this utility scans the global IPv6 address space (e.g. 2000::/4). The scanning process
* generates candidate addresses, scans for them, tests the network ranges where live addresses are found
* for aliased conditions, and adds legitimate discovered IPv6 addresses to an output list
*
* arguments
* network string     The IPv6 CIDR range to scan.

*/
void scan(string network);

#endif