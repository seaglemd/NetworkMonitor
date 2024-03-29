#define WIN32_LEAN_AND_MEAN

#include "udpTableAccess.h"
//constructor which sets up initial data
UdpTableAccess::UdpTableAccess()
{
	for (int i = 0; i < 13; i++){
		currentConnectionStatusNums[i] = 0;
	}
	getDatagrams();
	getUdpTable();
	startThread();
}
//gets the udp table info
void UdpTableAccess::getUdpTable()
{
	if (dataState == 0) {

		DWORD dwRetVal;
		PMIB_UDPTABLE pUdpTable;
		DWORD dwSize = 0;
		char szLocalAddr[INET6_ADDRSTRLEN];
		struct in_addr IpAddr;

		pUdpTable = (MIB_UDPTABLE *)malloc(sizeof(MIB_UDPTABLE));
		if (pUdpTable == NULL) {
			printf("Error allocating memory<br>");
		}

		dwSize = sizeof(MIB_UDPTABLE);
		// Make an initial call to GetTcpTable to
		// get the necessary size into the dwSize variable
		if ((dwRetVal = GetUdpTable(pUdpTable, &dwSize, TRUE)) == ERROR_INSUFFICIENT_BUFFER) {
			free(pUdpTable);
			pUdpTable = (MIB_UDPTABLE *)malloc(dwSize);
			if (pUdpTable == NULL) {
				//udpTableInformation = "Error allocating memory</br>";

			}
		}

		// Make a second call to GetTcpTable to get
		// the actual data we require

		if ((dwRetVal = GetUdpTable(pUdpTable, &dwSize, TRUE)) == NO_ERROR) {
			if (udpTableEntryCount > 0){
				for (int i = 0; i < udpTableEntryCount; i++){
					delete[]udpConnectionList[i];
				}
				delete[]udpConnectionList;
			}

			udpTableEntryCount = (int)pUdpTable->dwNumEntries;
			udpConnectionList = new string *[udpTableEntryCount];
			int udpTablePosition = udpTableEntryCount;
			for (int i = 0; i < udpTableEntryCount; i++){
				udpConnectionList[i] = new string[2];
			}

			for (int i = 0; i < udpTableEntryCount; i++, udpTablePosition--) {
				IpAddr.S_un.S_addr = (u_long)pUdpTable->table[udpTablePosition].dwLocalAddr;
				inet_ntop(2, &IpAddr, (PSTR)szLocalAddr, sizeof(szLocalAddr));

				localIp = szLocalAddr;
				udpConnectionList[i][0] = localIp;

				localPort = "";
				localPort += std::to_string(ntohs((u_short)pUdpTable->table[udpTablePosition].dwLocalPort));
				udpConnectionList[i][1] = localPort;
			}
		}
		else {
			//udpTableInformation += string("GetUdpTable failed with <br>") + std::to_string(dwRetVal);
			free(pUdpTable);
		}

		if (pUdpTable != NULL) {
			free(pUdpTable);
			pUdpTable = NULL;
		}
		if (curUdpTableEntryCount != udpTableEntryCount){
			dataState = 1;
			curUdpTableEntryCount = udpTableEntryCount;
		}


	}
}
//gets the number of datagrams received
const char *UdpTableAccess::getDatagrams()
{
	FIXED_INFO *pFixedInfo; //object passed to the function
	ULONG ulOutBufLen; //passed to function as well
	//IP_ADDRESS_STRING *pIPAddr; //IP Address to use
	PMIB_UDPSTATS pUDPStats;
	DWORD dwRetVal;
	pFixedInfo = (FIXED_INFO *)malloc(sizeof(FIXED_INFO));
	ulOutBufLen = sizeof(FIXED_INFO); //correct the buffer size so it can hold the data;

	pUDPStats = (MIB_UDPSTATS *)malloc(sizeof(MIB_UDPSTATS));
	if (pUDPStats == NULL) {
		cout << "Error allocating memory for UDP" << endl;
	}

	if ((dwRetVal = GetUdpStatistics(pUDPStats)) != NO_ERROR) {
		cout << " Getting the ip stats failed with an error" << endl;
	}
	numOfDatagrams = pUDPStats->dwOutDatagrams; //monitors datagrams
	
	numberOfDatagrams = "";
	numberOfDatagrams += to_string(numOfDatagrams);

	return numberOfDatagrams.c_str();
}
//get the size of the current table
int UdpTableAccess::getTableSize()
{
	return curUdpTableEntryCount;
}
//get the size of the displayed table
const char *UdpTableAccess::getDisplayedTableSize()
{
	displayTableSize = "";
	displayTableSize += to_string(getTableSize());

	return displayTableSize.c_str();
}
//returns the data table, protects from reading/writing same time
string **UdpTableAccess::passUdpTable()
{
	if (dataState == 1)
		return udpConnectionList;
	else {
		emptyList = new string*[curUdpTableEntryCount];
		for (int i = 0; i < curUdpTableEntryCount; i++){
			emptyList[i] = new string[3];
			emptyList[i][0] = "";
			emptyList[i][1] = "";
			emptyList[i][2] = "";
		}
		return emptyList;
	}

}
//starts the thread
void UdpTableAccess::startThread()
{
	_beginthread(UdpTableAccess::enterThread, 0, this);
}
//enters the next context for threading
void UdpTableAccess::enterThread(void *p)
{
	((UdpTableAccess *)p)->threadBody();
	_endthread();
	return;
}
//gets the udp table until it is told to stop the thread
void UdpTableAccess::threadBody()
{
	while (stop == 0){
		getUdpTable();
	}
}
//for telling it the data has been retrieved
void UdpTableAccess::setDataState(int nDataState){
	dataState = nDataState;
}
//sends whether there is new data to be pocessed to display
int UdpTableAccess::getDataState(){
	return dataState;
}
//stops the thread
void UdpTableAccess::stopUpdates()
{
	stop = 1;
}
//starts the thread
void UdpTableAccess::startUpdates()
{
	stop = 0;
	startThread();
}