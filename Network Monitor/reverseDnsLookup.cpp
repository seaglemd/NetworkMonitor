#define WIN32_LEAN_AND_MEAN

#include "reverseDnsLookup.h"
// link with ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

using namespace std;


string *ReverseDnsLookup::getHostList(string **nTcpList, int nTcpListSize)
{
	tcpListSize = nTcpListSize;
	string *tempHostNameList = new string[tcpListSize];
	tcpList = nTcpList;
	if (hostNameListSize > 0){
		delete[]tcpHostList;		
	}
	hostNameListSize = 0;
	tcpHostList = new string[tcpListSize];
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
	for (int i = 0; i < tcpListSize; i++) {
			found = tcpList[i][1].find(':');
			//cout << (tcpList[i][1].substr(0, (found))).c_str() << endl;
			if (string((tcpList[i][1].substr(0, (found))).c_str()).compare(0, 1, "0") != 0 &&
				string((tcpList[i][1].substr(0, (found))).c_str()).compare(0, 3, "127") != 0 &&
				string((tcpList[i][1].substr(0, (found))).c_str()).compare(0, 3, "192") != 0 &&
				string((tcpList[i][1].substr(0, (found))).c_str()).compare(0, 2, "10") != 0) {

				saGNI.sin_family = AF_INET;
				saGNI.sin_addr.s_addr = inet_addr((tcpList[i][1].substr(0, (found))).c_str());
				saGNI.sin_port = htons(port);
			}

			//-----------------------------------------
			// Call getnameinfo
			dwRetval = getnameinfo((struct sockaddr *) &saGNI,
				sizeof(struct sockaddr),
				hostname,
				NI_MAXHOST, servInfo, NI_MAXSERV, NI_NUMERICSERV);

			if (dwRetval != 0) {
				tempHostNameList[i] = "unreachable";
			}
			else {
				tempHostNameList[i] = hostname;
				cout << hostname << endl;
			}

	}
		for (int i = 0; i < tcpListSize; i++){
			if (tempHostNameList[i].find("unreachable") == string::npos)
				tcpHostList[hostNameListSize++] = tempHostNameList[i];
		}
	return tcpHostList;
}
int ReverseDnsLookup::getTableSize()
{
	return hostNameListSize;
	
}
