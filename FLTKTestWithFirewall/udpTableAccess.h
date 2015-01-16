#ifndef UDPTABLEACCESS_H
#define UDPTABLEACCESS_H
#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <string>
#include <process.h>
#include <windows.h>


#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Iphlpapi.lib")


#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

using namespace std;

class UdpTableAccess
{
public:
	void getUdpTable();
	string **getUTable();
	int getTableSize();

private:
	int numofcon;
	string localIp;
	string localPort;
	int udpTableEntryCount;
	string **udpConnectionList;
};

#endif
