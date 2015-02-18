/*************************************************************
*This large and complicated class starts                     *
*and stops a thread which monitors for changes               *
*in the tcp table and triggers them for updating the display *
*with the results that are found.                            *
*************************************************************/

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
#include <thread>

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Iphlpapi.lib")

//heap space
#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

using namespace std;

class TcpTableAccess 
{
    public:	
		TcpTableAccess::TcpTableAccess();
        void getTcpTable(); //returns the tcpconnections
        void getNetworkParameters(); //collects network parameter information such as hostname, domainname, connection #'s
		const char *getHostName(); //returns hostname of computer
		const char *getDomainName(); //returns domain name if on one
		const char *getDnsServerList(); //returns the Dns server currently attached to
		const char *getNumberOfConnections(); //return the number of connections
		int getTableSize(); //returns the size of the tcpTable that was found
		string **passTcpTable(); //returns the tcpTable
		void setDataState(int nDataState); //is used to tell the object the new data has been processed
		int getDataState(); //returns the data state which states whether there have been updates which need to be processed

		void stopUpdates(); //stops the thread
		void startUpdates(); //starts the thread

    private:
        int numofcon = 0;
		int curTcpTableEntryCount = 0; //tracks the # of entries currently stored
		int tcpTableEntryCount = 0; //entries found now
		int currentConnectionStatusNums[13]; //stores previous connection information to see if states have changed
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
		
		int dataState = 0; //says there is new data
		int changeTheDataState = 0; //if things should be updated
		int stop = 0; //triggers the thread to stop

		void TcpTableAccess::startThread();
		void TcpTableAccess::threadBody();
		static void enterThread(void *p);
};

#endif
