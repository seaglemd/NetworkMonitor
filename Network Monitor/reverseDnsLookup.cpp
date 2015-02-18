#define WIN32_LEAN_AND_MEAN

#include "reverseDnsLookup.h"
// link with ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

using namespace std;

//returns an array of strings which each element contains the name from the local DNS server
string *ReverseDnsLookup::getHostList(string **nTcpList, int nTcpListSize)
{
	tcpListSize = nTcpListSize;
	string *tempHostNameList = new string[tcpListSize]; //list used for storing all results, even unreachable ones
	tcpList = nTcpList;
	if (hostNameListSize > 0){ //if a list is already there cleans up first
		delete[]tcpHostList;		
	}
	hostNameListSize = 0;
	tcpHostList = new string[tcpListSize];
    WSADATA wsaData = {0}; //default initialization
    int iResult = 0;
	//start setting up for the connection
    DWORD dwRetval; 

    struct sockaddr_in saGNI;
    char hostname[NI_MAXHOST];
    char servInfo[NI_MAXSERV];
    u_short port = 27015;

        // Validate the parameters

        // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) { //checks for a good connection on hostnames with winsock2
        hostNames = iResult;
    }
	size_t found; //for sending actual ip addresses location of the ip will be found with this
	for (int i = 0; i < tcpListSize; i++) {
			found = tcpList[i][1].find(':');
			//ensure we aren't doing meaningless local RDNS searches
			if (string((tcpList[i][1].substr(0, (found))).c_str()).compare(0, 1, "0") != 0 &&
				string((tcpList[i][1].substr(0, (found))).c_str()).compare(0, 3, "127") != 0 &&
				string((tcpList[i][1].substr(0, (found))).c_str()).compare(0, 3, "192") != 0 &&
				string((tcpList[i][1].substr(0, (found))).c_str()).compare(0, 2, "10") != 0) {
				//family of ips
				saGNI.sin_family = AF_INET;
				inet_pton(2, (tcpList[i][1].substr(0, (found))).c_str(), &(saGNI.sin_addr.s_addr)); //puts ip in proper format
				saGNI.sin_port = htons(port);
			}

			//-----------------------------------------
			// Call getnameinfo
			dwRetval = getnameinfo((struct sockaddr *) &saGNI,
				sizeof(struct sockaddr),
				hostname,
				NI_MAXHOST, servInfo, NI_MAXSERV, NI_NUMERICSERV);
			//the return value tests if all went well, if it didn't unreachable is put in the temp list
			if (dwRetval != 0) {
				tempHostNameList[i] = "unreachable";
			}
			//put the hostname into the temp list
			else {
				tempHostNameList[i] = hostname;
			}

	}
	//fills the actual list that will be used without all the unreachables
		for (int i = 0; i < tcpListSize; i++){
			if (tempHostNameList[i].find("unreachable") == string::npos)
				tcpHostList[hostNameListSize++] = tempHostNameList[i];
		}
	return tcpHostList;
}
//returns the size of the list
int ReverseDnsLookup::getTableSize()
{
	return hostNameListSize;
	
}