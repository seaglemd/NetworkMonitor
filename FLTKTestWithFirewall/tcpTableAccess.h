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

//#include "programLogTime.h"
//#include "fioLogger.h"


#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Iphlpapi.lib")


#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

using namespace std;

class TcpTableAccess 
{
    public:
        void startThread();
        string** getTcpTable();
        void getNetworkParameters();
		const char *getHostName();
		const char *getDomainName();
		const char *getDnsServerList();
		int getTableSize();
        static vector<string> addressVector;

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
       // ProgramLogTime tpo;
       // FioLogger ftpo;


       static void enterThread(void *p);
       void threadBody();
       void exitThread();

};


#endif
