/*******************************************************************
*This class performce a reverse DNS lookup to try and retrieve an  *
*actual server name from the TCP connections on this computer      *
*******************************************************************/
#ifndef REVERSEDNSLOOKUP_H
#define REVERSEDNSLOOKUP_H
#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <string>
#include <iphlpapi.h>
#include <stdio.h>
#include <string>
#include <process.h>
#include <windows.h>
#include <vector>
#include <mutex>
#include <thread>

// link with ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

using namespace std;

class ReverseDnsLookup
{
public:
	//function will return an array filled with the names from the DNS server
	string *ReverseDnsLookup::getHostList(string **nTcpList, int nTcpListSize); 
	int ReverseDnsLookup::getTableSize(); //returns size of the array previous created

private:
	int tcpListSize = 0; //initial sizing
	int hostNameListSize = 0; //initial sizing
	string hostNames;
	string *tcpHostList; //list of hosts created
	string **tcpList; //tcp list to be checked against
};

#endif // REVERSEDNSLOOKUP_H
