#include <survery6_active/scan.h>
#include <survery6_active/ping.h>

ip::address_v6_range generate_ip_addresses(string network_range){
    boost::system::error_code error_code;
    ip::network_v6 subnet = ip::make_network_v6(network_range,error_code);
    if(error_code)
    {
        std::cerr << "Error in the network range argument: " <<error_code.message() << "\n";
    }
    ip::address_v6_range hosts = subnet.hosts();

    return hosts;
}

// TODO (optimizations:
// tests the network ranges where live addresses are found for aliased conditions
// add legitimate discovered IPv6 addresses to an output list
void scan(string network, string output_path)
{
    cout << "Scanning networking rage " << network << endl;
    ofstream results(output_path+"result.csv");
    results << "IP address"<<"," <<"results"<<"\n";
    ip::address_v6_range hosts= generate_ip_addresses(network);
    for (auto addrs: hosts){
        string ip6 = addrs.to_string();
        bool success= ping(ip6);
        cout << ip6<<" "<<(success?"reachable":"unreachable") <<endl;
        results << ip6 << ", "<<(success?"reachable":"unreachable")<<"\n";
    }
    results.close();
    cout << "Scan completed" << endl;

}