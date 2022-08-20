#include <boost/program_options.hpp>
#include <iostream>
#include <survery6_active/scan.h>

using namespace boost::program_options;
using namespace std;

int main(int argc, char **argv)

{
    try{
        options_description desc{"Options"};
        desc.add_options()
                ("help,h", "Help screen")
                ("feature,f", value<string>()->default_value("scan")->required(), "Utility feature")
                ("network,n", value<string>()->required(), "Network range")
                ("output_path,o", value<string>()->required(), "Output path");

        variables_map vm;
        store(parse_command_line(argc, argv, desc), vm);
        notify(vm);

        if (vm.count("help"))
            cout << desc << '\n';
        if(vm.count("feature") && vm.count("network") && vm.count("output_path")){
            if(vm["feature"].as<string>() == "scan"){
                const string network_address = vm["network"].as<string>();
                const string output_path = vm["output_path"].as<string>();
                scan(network_address,output_path);
            }
        }

    }
    catch(exception& e)
    {
       cerr << "Error: " << e.what() << "\n";
    }
    catch(...)
    {
       cerr << "Unknown error!" << "\n";
    }

}