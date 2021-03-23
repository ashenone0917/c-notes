# Windows下C++得到一个空闲的端口API
这个东西，并不太常用，客户端弄一个Tcp连接，也不需要你去指定端口，系统会自动找一个空闲的端口给你；\
但是，如果在服务器端，手动得到一个空闲端口是很重要的，毕竟这个端口要固定，因此需要你自己指定一个空闲的端口。\
那么在Windows下，如何利用C++得到一个空闲的端口呢，答案是Windows的API。\
要用的Windows系统API如下：
```cpp
#include <tcpmib.h>
#include <IPHlpApi.h>

#pragma comment(lib, "IPHlpApi.lib")

typedef struct _MIB_TCPTABLE {
  DWORD      dwNumEntries;
  MIB_TCPROW table[ANY_SIZE];
} MIB_TCPTABLE, *PMIB_TCPTABLE;

typedef struct _MIB_TCPROW_LH {
  union {
    DWORD         dwState;
    MIB_TCP_STATE State;
  };
  DWORD dwLocalAddr;
  DWORD dwLocalPort;
  DWORD dwRemoteAddr;
  DWORD dwRemotePort;
} MIB_TCPROW_LH, *PMIB_TCPROW_LH;

ULONG GetTcpTable(
  PMIB_TCPTABLE TcpTable,
  PULONG        SizePointer,
  BOOL          Order
);

// UDP略
```
这些API可以得到已经占用的端口列表，想继续了解这些API可以自己查MSDN，这里不再赘述。\
我们要封装成三个接口：
- uint16_t FindAvailableTcpPort(uint16_t begin = PORT_DOWN, uint16_t end = PORT_UP);
- uint16_t FindAvailableUdpPort(uint16_t begin = PORT_DOWN, uint16_t end = PORT_UP);
- uint16_t FindAvailablePort(uint16_t begin = PORT_DOWN, uint16_t end = PORT_UP);

这三个接口分别的作用是：获得Tcp空闲端口，获得Udp空闲端口，获得Tcp和Udp都空闲的端口。\
代码和测试如下：
```cpp
#include <Windows.h>
#include <WinSock.h>
#include <tcpmib.h>
#include <IPHlpApi.h>

#include <vector>
#include <memory>
#include <algorithm>
#include <iostream>

#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "IPHlpApi.lib")

#define PORT_DOWN 49152
#define PORT_UP 65535

static std::vector<uint16_t> GetAllTcpConnectionsPort()
{
    std::vector<uint16_t> ret;
    ULONG size = 0;
    GetTcpTable(NULL, &size, TRUE);
    std::unique_ptr<char[]> buffer(new char[size]);

    PMIB_TCPTABLE tcpTable = reinterpret_cast<PMIB_TCPTABLE>(buffer.get());
    if (GetTcpTable(tcpTable, &size, FALSE) == NO_ERROR)
        for (size_t i = 0; i < tcpTable->dwNumEntries; i++)
            ret.push_back(ntohs((uint16_t)tcpTable->table[i].dwLocalPort));
    std::sort(std::begin(ret), std::end(ret));
    return ret;
}

static std::vector<uint16_t> GetAllUdpConnectionsPort()
{
    std::vector<uint16_t> ret;
    ULONG size = 0;
    GetUdpTable(NULL, &size, TRUE);
    std::unique_ptr<char[]> buffer(new char[size]);

    PMIB_UDPTABLE udpTable = reinterpret_cast<PMIB_UDPTABLE>(buffer.get());
    if (GetUdpTable(udpTable, &size, FALSE) == NO_ERROR)
        for (size_t i = 0; i < udpTable->dwNumEntries; i++)
            ret.push_back(ntohs((uint16_t)udpTable->table[i].dwLocalPort));
    std::sort(std::begin(ret), std::end(ret));
    return ret;
}

uint16_t FindAvailableTcpPort(uint16_t begin = PORT_DOWN, uint16_t end = PORT_UP)
{
    auto vec = GetAllTcpConnectionsPort();
    for (uint16_t port = begin; port != end; ++port)
        if (!std::binary_search(std::begin(vec), std::end(vec), port))
            return port;
    return 0;
}

uint16_t FindAvailableUdpPort(uint16_t begin = PORT_DOWN, uint16_t end = PORT_UP)
{
    auto vec = GetAllUdpConnectionsPort();
    for (uint16_t port = begin; port != end; ++port)
        if (!std::binary_search(std::begin(vec), std::end(vec), port))
            return port;
    return 0;
}

uint16_t FindAvailablePort(uint16_t begin = PORT_DOWN, uint16_t end = PORT_UP)
{
    auto vecTcp = GetAllTcpConnectionsPort(),
        vecUdp = GetAllUdpConnectionsPort();
    for (uint16_t port = begin; port != end; ++port)
        if (!std::binary_search(std::begin(vecTcp), std::end(vecTcp), port) &&
            !std::binary_search(std::begin(vecUdp), std::end(vecUdp), port))
            return port;
    return 0;
}

int main()
{
    {
        WSADATA wsaData;
        WSAStartup(0x0201, &wsaData);
    }
    std::cout << "Tcp Available port : " << FindAvailableTcpPort() << "\n";
    std::cout << "Udp Available port : " << FindAvailableUdpPort() << "\n";
    std::cout << "Available port : " << FindAvailablePort() << "\n";
    {
        WSACleanup();
    }
    return 0;
}
```
端口[49152, 65535]是动态端口范围，因此可以作为区间的默认值。\
而且还要注意，网上说的利用bind来暴力枚举空闲端口没有作用，我试过。。。
