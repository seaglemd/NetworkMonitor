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

//#include "tcpTableCellDisplay.h"
// link with ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

using namespace std;

class ReverseDnsLookup
{
public:
	string *ReverseDnsLookup::getHostList(string **nTcpList, int nTcpListSize);
	int ReverseDnsLookup::getTableSize();

private:
	int tcpListSize = 0;
	int hostNameListSize = 0;
	const char *c;
	string hostNames;
	string *tcpHostList;
	string **tcpList;
};

#endif // REVERSEDNSLOOKUP_H
