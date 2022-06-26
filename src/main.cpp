#include <iostream>
#include <survery6_active/scan.h>

using namespace std;

int main(int argc, char **argv)
{
    if (argc >= 2)
    {
        const string option = argv[1];
        if (option == "scan")
            scan(argv[2]);
        else
            cout << "Its not an valid option" << endl;
    }

    else
        cout << "No arguments found" << endl;
    return 0;
}