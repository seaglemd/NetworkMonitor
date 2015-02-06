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
	UdpTableAccess::UdpTableAccess();
	void UdpTableAccess::getUdpTable();
	const char *UdpTableAccess::getDatagrams();
	int UdpTableAccess::getTableSize();
	const char *UdpTableAccess::getDisplayedTableSize();
	string **UdpTableAccess::passUdpTable();
	void UdpTableAccess::setDataState(int nDataState);
	int UdpTableAccess::getDataState();
	

private:
	int numOfDatagrams;
	int curUdpTableEntryCount = 0;
	int udpTableEntryCount = 0;
	int currentConnectionStatusNums[13];
	string numberOfDatagrams;
	string displayTableSize;
	string localIp;
	string localPort;
	string **udpConnectionList = 0;
	string **emptyList = 0;

	int dataState = 0;
	int changeTheDataState = 0;

	void UdpTableAccess::startThread();
	void UdpTableAccess::threadBody();
	static void enterThread(void *p);
};

#endif
