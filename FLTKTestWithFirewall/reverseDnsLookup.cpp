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

/*
 * class for doing reverse dns lookups
 */

//starts a new process thread
void ReverseDnsLookup::startThread()
{
    _beginthread(ReverseDnsLookup::enterThread, 0, this);
}

void ReverseDnsLookup::enterThread(void *p)
{
    //threads are used to monitor for changes, in this case dns lookup updates
    ((ReverseDnsLookup *) p)->threadBody();
    _endthread();
    return;
}

void ReverseDnsLookup::threadBody()
{
    //rechecks all the current active connections
    recheckConnections();
}

void ReverseDnsLookup::getHostnameString()
{
    //if there is a hostname string to retrieve this function finds them
    //then compiles the information into a string for long writing
    //formatting is done in html

    hostnames = "<font color=red>Currently connected to: </font>";
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
        hostnames = iResult;

    }
    int size = TcpTableAccess::addressVector.size();
    for(int i = 0; i < size;i++) {
        c = TcpTableAccess::addressVector[i].c_str();
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
            hostnames += "";
        } else {
            if(string(hostname).compare("trust")!= 0){
            hostnames += "<br>" + string(hostname);
            }
        }
    }
    hnfname = "hostnameinfo";
    hnfnametemp = "temphostnameinfo";
    ftpo.writeToTempLog(hnfnametemp,"<font color=red><h2>" + tpo.getCurrentDate() + "</h2></font>"
                        + "<font color=blue>" + hostnames + "</font>");
    ftpo.writeToLog(hnfname,"<br><font color=red>" + tpo.getCurrentDate() + "</font><br><br>"
                    + "<font color=blue>" + hostnames + "</font>");
    //sleep times are to prevent overload in threads
    Sleep(5000);
    hostnames = "";
    startThread();
}
void ReverseDnsLookup::recheckConnections()
{
    connectionsUpdated();

}
//signals are emitted to make the program aware of changes
void ReverseDnsLookup::connectionsUpdated()
{
    getHostnameString();
    emit hostnameChange();
}
