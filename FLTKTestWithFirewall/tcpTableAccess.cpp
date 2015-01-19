#define WIN32_LEAN_AND_MEAN

//#include "programLogTime.h"
//#include "fioLogger.h"
#include "tcpTableAccess.h"

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Iphlpapi.lib")


#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

using namespace std;

/*
 *accesses the connections in the tcp table
 *many of the connections are local ones and because of that
 * the address shows up as all zeros
 */

//method to get the information about the network
void TcpTableAccess::getNetworkParameters()
{

    FIXED_INFO *pFixedInfo; //object passed to the getnetworkparam function
    ULONG ulOutBufLen; //passed to function as well
    DWORD dwRetVal; //for error checking

    pFixedInfo = (FIXED_INFO *)malloc(sizeof(FIXED_INFO));
    ulOutBufLen = sizeof(FIXED_INFO); //correct the buffer size so it can hold the data;


   if(GetNetworkParams(pFixedInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) {
        cout << "Made it here";
        free(pFixedInfo);
        pFixedInfo = (FIXED_INFO *)malloc(ulOutBufLen);
        if (pFixedInfo == NULL) {
            cout << "Error Allocating the memory for ulOutBufLen";
        }
    }

    if ((dwRetVal = GetNetworkParams(pFixedInfo, &ulOutBufLen)) != NO_ERROR) {
        cout << "GetNetworkParams failed with error" << dwRetVal;
        if (pFixedInfo)
            free(pFixedInfo);
    }

    hostName = pFixedInfo->HostName;
    domainName = pFixedInfo->DomainName;
    dnsServerList = pFixedInfo->DnsServerList.IpAddress.String;

    if (pFixedInfo) {
        free(pFixedInfo);
        pFixedInfo = NULL;
    }

}
//retrieves the tcp table
void TcpTableAccess::getTcpTable()
{
	if (dataState == 0) {
		FIXED_INFO *pFixedInfo; //object passed to the function
		ULONG ulOutBufLen; //passed to function as well    
		PMIB_TCPSTATS pTCPStats;
		DWORD dwRetVal;
		PMIB_TCPTABLE pTcpTable;
		DWORD dwSize = 0;
		char szLocalAddr[128];
		char szRemoteAddr[128];
		struct in_addr IpAddr;
		int lCurrentConnectionStatusNums[13] = {};
		int consChanged = 0;

		pFixedInfo = (FIXED_INFO *)malloc(sizeof(FIXED_INFO));
		ulOutBufLen = sizeof(FIXED_INFO); //correct the buffer size so it can hold the data;

		pTCPStats = (MIB_TCPSTATS *)malloc(sizeof(MIB_TCPSTATS));
		if (pTCPStats == NULL)
			cout << "Error allocating memory for TCP" << endl;

		if ((dwRetVal = GetTcpStatistics(pTCPStats)) != NO_ERROR)
			cout << " Getting the ip stats failed with an error" << endl;
		if (numofcon != (int)pTCPStats->dwNumConns) {
			numofcon = (int)pTCPStats->dwNumConns;
			consChanged = 1;
		}

		pTcpTable = (MIB_TCPTABLE *)malloc(sizeof(MIB_TCPTABLE));
		if (pTcpTable == NULL)
			printf("Error allocating memory\n");

		dwSize = sizeof(MIB_TCPTABLE);
		// Make an initial call to GetTcpTable to
		// get the necessary size into the dwSize variable
		if ((dwRetVal = GetTcpTable(pTcpTable, &dwSize, TRUE)) == ERROR_INSUFFICIENT_BUFFER) {
			free(pTcpTable);
			pTcpTable = (MIB_TCPTABLE *)malloc(dwSize);
			if (pTcpTable == NULL)
				errors += "Error allocating memory<br>";
		}

		// Make a second call to GetTcpTable to get
		// the actual data we require

		if ((dwRetVal = GetTcpTable(pTcpTable, &dwSize, TRUE)) == NO_ERROR) {

			if (tcpTableEntryCount > 0){
				for (int i = 0; i < tcpTableEntryCount; i++){
					delete[]tcpConnectionList[i];
				}
				delete[]tcpConnectionList;
			}

			tcpTableEntryCount = (int)pTcpTable->dwNumEntries;
			tcpConnectionList = new string *[tcpTableEntryCount];
			for (int i = 0; i < tcpTableEntryCount; i++){
				tcpConnectionList[i] = new string[3];
			}
			for (int i = 0; i < tcpTableEntryCount; i++) {
				IpAddr.S_un.S_addr = (u_long)pTcpTable->table[i].dwLocalAddr;
				strcpy_s(szLocalAddr, sizeof(szLocalAddr), inet_ntoa(IpAddr)); //local address
				IpAddr.S_un.S_addr = (u_long)pTcpTable->table[i].dwRemoteAddr;
				strcpy_s(szRemoteAddr, sizeof(szRemoteAddr), inet_ntoa(IpAddr)); //remote address

				switch (pTcpTable->table[i].dwState) { //switch statement gets state of connection
				case MIB_TCP_STATE_CLOSED:
					connectionState = "CLOSED";
					lCurrentConnectionStatusNums[1]++;
					break;
				case MIB_TCP_STATE_LISTEN:
					connectionState = "LISTEN";
					lCurrentConnectionStatusNums[2]++;
					break;
				case MIB_TCP_STATE_SYN_SENT:
					connectionState = "SYN-SENT";
					lCurrentConnectionStatusNums[3]++;
					break;
				case MIB_TCP_STATE_SYN_RCVD:
					connectionState = "SYN-RECEIVED";
					lCurrentConnectionStatusNums[4]++;
					break;
				case MIB_TCP_STATE_ESTAB:
					connectionState = "ESTABLISHED";
					lCurrentConnectionStatusNums[5]++;
					break;
				case MIB_TCP_STATE_FIN_WAIT1:
					connectionState = "FIN-WAIT-1";
					lCurrentConnectionStatusNums[6]++;
					break;
				case MIB_TCP_STATE_FIN_WAIT2:
					connectionState = "FIN-WAIT-2";
					lCurrentConnectionStatusNums[7]++;
					break;
				case MIB_TCP_STATE_CLOSE_WAIT:
					connectionState = "CLOSE-WAIT";
					lCurrentConnectionStatusNums[8]++;
					break;
				case MIB_TCP_STATE_CLOSING:
					connectionState = "CLOSING";
					lCurrentConnectionStatusNums[9]++;
					break;
				case MIB_TCP_STATE_LAST_ACK:
					connectionState = "LAST-ACK";
					lCurrentConnectionStatusNums[10]++;
					break;
				case MIB_TCP_STATE_TIME_WAIT:
					connectionState = "TIME-WAIT";
					lCurrentConnectionStatusNums[11]++;
					break;
				case MIB_TCP_STATE_DELETE_TCB:
					connectionState = "DELETE-TCB";
					lCurrentConnectionStatusNums[12]++;
					break;
				default:
					connectionState = "Unknown State";
					lCurrentConnectionStatusNums[13]++;
					break;
				}

				//issues with inline immediate conversion causing compiler errors.
				//separate string conversion is then added to a parent string
				localIpPort = szLocalAddr;
				localIpPort += ":";
				localIpPort += std::to_string(ntohs((u_short)pTcpTable->table[i].dwLocalPort));
				tcpConnectionList[i][0] = localIpPort;

				remoteIpPort = szRemoteAddr;
				remoteIpPort += ":";
				remoteIpPort += std::to_string(ntohs((u_short)pTcpTable->table[i].dwRemotePort));
				tcpConnectionList[i][1] = remoteIpPort;

				tcpConnectionList[i][2] = connectionState;

			}
		}
		else {
			errors += "GetTcpTable failed with <br>" + std::to_string(dwRetVal);
			free(pTcpTable);

		}

		if (pTcpTable != NULL) {
			free(pTcpTable);
			pTcpTable = NULL;
		}
		
		for (int z = 0; z < 13 && dataState != 1; z++){
			if (lCurrentConnectionStatusNums[z] != currentConnectionStatusNums[z]){
				dataState = 1;
				numoftimes++;
				currentConnectionStatusNums[z] = lCurrentConnectionStatusNums[z];
			}
		}
		if (consChanged == 1){
			dataState = 1;
			numoftimes++;
			consChanged = 0;
		}
	}
	startThread();
}
void TcpTableAccess::startThread() 
{
	_beginthread(TcpTableAccess::enterThread, 0, this);
}

void TcpTableAccess::enterThread(void *p)
{
	((TcpTableAccess *)p)->threadBody();
	_endthread();
	return;
}

void TcpTableAccess::threadBody()
{
	getTcpTable();
}

const char *TcpTableAccess::getHostName()
{
	getNetworkParameters();
	return hostName.c_str();
}

const char *TcpTableAccess::getDomainName() {
	getNetworkParameters();
	return domainName.c_str();
}

const char *TcpTableAccess::getDnsServerList() {
	getNetworkParameters();
	return dnsServerList.c_str();
}

const char *TcpTableAccess::getNumberOfConnections() {
	numberOfConnections = "";
	numberOfConnections += to_string(numofcon);
	numberOfConnections += to_string(numoftimes);
	numberOfConnections += to_string(dataState);
	return numberOfConnections.c_str();
}

int TcpTableAccess::getTableSize()
{
	return tcpTableEntryCount;
}
string **TcpTableAccess::passTcpTable()
{
	if (dataState == 1)
		return tcpConnectionList;
	else {
		emptyList = new string*[tcpTableEntryCount];
		for (int i = 0; i < tcpTableEntryCount; i++){
			emptyList[i] = new string[3];
			emptyList[i][0] = "";
			emptyList[i][1] = "";
			emptyList[i][2] = "";
		}
		return emptyList;
	}
		
}

void TcpTableAccess::changeDataState(int nDataState){
	dataState = nDataState;
}
int TcpTableAccess::getDataState(){
	return dataState;
}

