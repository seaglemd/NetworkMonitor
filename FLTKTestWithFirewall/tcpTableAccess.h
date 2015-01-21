#ifndef TCPTABLEACCESS_H
#define TCPTABLEACCESS_H
#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <string>
#include <process.h>
#include <windows.h>
#include <vector>
#include <mutex>

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Iphlpapi.lib")


#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

using namespace std;

class TcpTableAccess 
{
    public:		
		void startThread();
        void getTcpTable();
        void getNetworkParameters();
		const char *getHostName();
		const char *getDomainName();
		const char *getDnsServerList();
		const char *getNumberOfConnections();
		int getTableSize();
		string **passTcpTable();
		void changeDataState(int nDataState);
		int getDataState();

    private:
        int numofcon = 0;
		int numoftimes = 0;
		int tcpTableEntryCount = 0;
		int currentConnectionStatusNums[13] = { };
		string numberOfConnections;
		string connectionState;
		string localIpPort;
		string remoteIpPort;
		string hostName;
		string domainName;
		string dnsServerList;
		string errors;
		string **tcpConnectionList = 0;
		string **emptyList = 0;
		int dataState = 0;

		static void enterThread(void *p);
		void threadBody();
};

#endif
