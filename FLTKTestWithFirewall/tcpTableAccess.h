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

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Iphlpapi.lib")


#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

using namespace std;

class TcpTableAccess 
{
    public:
        string** getTcpTable();
        void getNetworkParameters();
		const char *getHostName();
		const char *getDomainName();
		const char *getDnsServerList();
		int getNumberOfConnections();
		int getTableSize();
        static vector<string> addressVector;
		void changeThreadState(int threadState);
		int getThreadState();

    private:
        int numofcon;
		int tcpTableEntryCount = 0;
		string connectionState;
		string localIpPort;
		string remoteIpPort;
		string hostName;
		string domainName;
		string dnsServerList;
		string errors;

		string **tcpConnectionList = 0;
		int in_thread = 0;
};

#endif
