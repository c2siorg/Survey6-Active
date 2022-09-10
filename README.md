# Survey Active
Ipv6 is the internet's future, and it necessitated a more scalable survey tool to comprehend how routing and DNS function. The purpose of this project is to create an IPv6 listener that will actively collect IPv6 traffic data as a active data collection tool for cyber security research.

## Technology stack
- c++
- socket
## Folder structure
```

├── inlcude 
│   ├── survey6_active
│   │    │   ├── ping.h
│   │    │   └── scan.h
├── src
│   ├── main
│   │    │   ├── ping.cpp
│   │    │   └── scan.cpp
│   │    │   └── main.cpp
│   └── test
└── CMakeLists.txt

```
## Dependencies
- [Boost](https://www.boost.org/)

## Build process

Run the following commands to create anexcutable file.

```
mkdir build

cd build

cmake ..

make
```

This would create an excutable name `active_probe`

## Excuting the binary

After building the excutable based on the above defined process run `sudo ./active_probe -n network-range -o path-to-save-the-results`

## Screenshots
<img width="613" alt="Capture" src="https://user-images.githubusercontent.com/21036426/189484857-4e44d18d-f3c9-4769-bc98-28bb062578b0.PNG">

