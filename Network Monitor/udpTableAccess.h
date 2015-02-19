/****************************************************************
*Behind in development from tcp table access but still has      *
*threading. There is also less to do because it is not at the   *
*stage where it is detecting where connections actually come    *
*from since UDP on it's own is Fire and Forget. Ultimately      *
*the class looks up the current UDP table connections and       *
*counts the datagrams that have been received.                  *
****************************************************************/

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

//get some memory from the heap
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
	void UdpTableAccess::stopUpdates();
	void UdpTableAccess::startUpdates();

private:
	int numOfDatagrams; //stores # of datagrams
	int curUdpTableEntryCount = 0; //currently displayed count of the udp table
	int udpTableEntryCount = 0; //entry count for the data not displayed
	int currentConnectionStatusNums[13]; //monitoring status of connections
	string numberOfDatagrams;
	string displayTableSize;
	string localIp;
	string localPort;
	string **udpConnectionList = 0;
	string **emptyList = 0;

	int dataState = 0;
	int changeTheDataState = 0;
	int stop = 0;

	void UdpTableAccess::startThread();
	void UdpTableAccess::threadBody();
	static void enterThread(void *p);
};

#endif
