#define WIN32_LEAN_AND_MEAN


#include "tcpTableAccess.h"

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Iphlpapi.lib")


#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

using namespace std;

/*
 *accesses the connections in the tcp table
 *many of the connections are local open ones and because of that
 * some of the address show up as all zeros
 */
//Default constructor which initializes everything and starts the thread
TcpTableAccess::TcpTableAccess()
{
	for (int i = 0; i < 13; i++){
		currentConnectionStatusNums[i] = 0;
	}
	getNetworkParameters();
	getTcpTable();
	startThread();
}

//method to get the information about the network
void TcpTableAccess::getNetworkParameters()
{

    FIXED_INFO *pFixedInfo; //object passed to the getnetworkparam function
    ULONG ulOutBufLen; //passed to function as well
    DWORD dwRetVal; //for error checking

    pFixedInfo = (FIXED_INFO *)malloc(sizeof(FIXED_INFO));
    ulOutBufLen = sizeof(FIXED_INFO); //correct the buffer size so it can hold the data;


   if(GetNetworkParams(pFixedInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) {
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
	//placed in variables for useful retrieval
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
		char szLocalAddr[INET6_ADDRSTRLEN];
		char szRemoteAddr[INET6_ADDRSTRLEN];
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
			if (tcpTableEntryCount > 0){ //cleans up from repeated calls
				for (int i = 0; i < tcpTableEntryCount; i++){
					delete[]tcpConnectionList[i];
				}
				delete[]tcpConnectionList;
			}
			//number of entries
			tcpTableEntryCount = (int)pTcpTable->dwNumEntries;
			tcpConnectionList = new string *[tcpTableEntryCount];
			for (int i = 0; i < tcpTableEntryCount; i++){
				tcpConnectionList[i] = new string[3];
			}
			int tcpTablePosition = tcpTableEntryCount;
			for (int i = 0; i < tcpTableEntryCount; i++, tcpTablePosition--) {
				IpAddr.S_un.S_addr = (u_long)pTcpTable->table[tcpTablePosition].dwLocalAddr;//local address info
				inet_ntop(2, &IpAddr, (PSTR)szLocalAddr, sizeof(szLocalAddr));
				IpAddr.S_un.S_addr = (u_long)pTcpTable->table[tcpTablePosition].dwRemoteAddr; //remote address info
				inet_ntop(2, &IpAddr, (PSTR)szRemoteAddr, sizeof(szRemoteAddr));

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
				localIpPort += std::to_string(ntohs((u_short)pTcpTable->table[tcpTablePosition].dwLocalPort));
				tcpConnectionList[i][0] = localIpPort;

				remoteIpPort = szRemoteAddr;
				remoteIpPort += ":";
				remoteIpPort += std::to_string(ntohs((u_short)pTcpTable->table[tcpTablePosition].dwRemotePort));
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
		//this code checks to see if any of the data has changed which will then 
		//trigger the data state to inform there is new data to be drawn
		for (int z = 0; z < 13; z++){
			if (lCurrentConnectionStatusNums[z] != currentConnectionStatusNums[z]){
				changeTheDataState = 1;
				currentConnectionStatusNums[z] = lCurrentConnectionStatusNums[z];
			}
		}
		if (changeTheDataState == 1){
			dataState = 1;
			curTcpTableEntryCount = tcpTableEntryCount;
		}
		if (consChanged == 1){
			dataState = 1;
			curTcpTableEntryCount = tcpTableEntryCount;
			consChanged = 0;
		}
	}
}
//returns host name
const char *TcpTableAccess::getHostName()
{
	getNetworkParameters();
	return hostName.c_str();
}
//returns domain name
const char *TcpTableAccess::getDomainName() {
	getNetworkParameters();
	return domainName.c_str();
}
//returns the dns server
const char *TcpTableAccess::getDnsServerList() {
	getNetworkParameters();
	return dnsServerList.c_str();
}
//returns the number of connections
const char *TcpTableAccess::getNumberOfConnections() {
	numberOfConnections = "";
	numberOfConnections += to_string(numofcon);

	return numberOfConnections.c_str();
}
//returns the size of the table
int TcpTableAccess::getTableSize()
{
	return curTcpTableEntryCount;
}
//returns the list if there is new data else returns an empty list (protection from read/write combo errors
string **TcpTableAccess::passTcpTable()
{
	if (dataState == 1)
		return tcpConnectionList;
	else {
		emptyList = new string*[curTcpTableEntryCount];
		for (int i = 0; i < curTcpTableEntryCount; i++){
			emptyList[i] = new string[3];
			emptyList[i][0] = "";
			emptyList[i][1] = "";
			emptyList[i][2] = "";
		}
		return emptyList;
	}
		
}
//starts the thread
void TcpTableAccess::startThread()
{
	_beginthread(TcpTableAccess::enterThread, 0, this);
}
//now from a next context
void TcpTableAccess::enterThread(void *p)
{
	((TcpTableAccess *)p)->threadBody();
	_endthread();
	return;
}
//goes until the thread is told to stop
void TcpTableAccess::threadBody()
{
	while (stop == 0){
		getTcpTable();
	}
}
//after data is processed for display we can tell it to resume looking for new data
void TcpTableAccess::setDataState(int nDataState){
	dataState = nDataState;
}
//for checking if new data should be displayed
int TcpTableAccess::getDataState(){
	return dataState;
}
//sets the stop variable so the thread will stop
void TcpTableAccess::stopUpdates()
{
	stop = 1;
}
//starts the thread again
void TcpTableAccess::startUpdates()
{
	stop = 0;
	startThread();
}

