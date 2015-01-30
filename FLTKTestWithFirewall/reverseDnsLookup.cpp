#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <process.h>
#include <windows.h>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <vector>

#include "tcpTableAccess.h"
#include "reverseDnsLookup.h"
// link with ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

using namespace std;


string *ReverseDnsLookup::getHostList(string **tcpList, int listSize)
{
	if (hostNameListSize > 0)
		delete[]tcpHostList;

	tcpHostList = new string[listSize];
    WSADATA wsaData = {0};
    int iResult = 0;

    DWORD dwRetval;

    struct sockaddr_in saGNI;
    char hostname[NI_MAXHOST];
    char servInfo[NI_MAXSERV];
    u_short port = 27015;

        // Validate the parameters

        // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        hostNames = iResult;

    }
	size_t found;
    for(int i = 0; i < listSize;i++) {
		found = tcpList[i][0].find(':');
		c = tcpList[i][0].substr(0, found).c_str();
        if(string(c).compare(0,1,"0") != 0 &&
                string(c).compare(0,3,"127") != 0 &&
                string(c).compare(0,3,"192") != 0 &&
                string(c).compare(0,2,"10") != 0) {

                        saGNI.sin_family = AF_INET;
                        saGNI.sin_addr.s_addr = inet_addr(c);
                        saGNI.sin_port = htons(port);
        }

        //-----------------------------------------
        // Call getnameinfo
    dwRetval = getnameinfo((struct sockaddr *) &saGNI,
                               sizeof (struct sockaddr),
                               hostname,
                               NI_MAXHOST, servInfo, NI_MAXSERV, NI_NUMERICSERV);

        if (dwRetval != 0) {
			tcpHostList[i] = "";
        }
		else {
			tcpHostList[i] = hostname;
		}
			
	}
	hostNameListSize = listSize;
	return tcpHostList;
}
int ReverseDnsLookup::getTableSize()
{
	return hostNameListSize;
}
