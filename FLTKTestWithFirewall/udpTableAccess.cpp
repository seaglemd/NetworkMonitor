
#include "udpTableAccess.h"


//gets the udp table info
void UdpTableAccess::getUdpTable()
{
	FIXED_INFO *pFixedInfo; //object passed to the function
	ULONG ulOutBufLen; //passed to function as well
	//IP_ADDRESS_STRING *pIPAddr; //IP Address to use
	PMIB_UDPSTATS pUDPStats;
	DWORD dwRetVal;
	PMIB_UDPTABLE pUdpTable;
	DWORD dwSize = 0;
	char szLocalAddr[128];
	struct in_addr IpAddr;

	pFixedInfo = (FIXED_INFO *)malloc(sizeof(FIXED_INFO));
	ulOutBufLen = sizeof(FIXED_INFO); //correct the buffer size so it can hold the data;
	pUDPStats = (MIB_UDPSTATS *)malloc(sizeof(MIB_UDPSTATS));
	if (pUDPStats == NULL) {
		cout << "Error allocating memory for UDP" << endl;
	}

	if ((dwRetVal = GetUdpStatistics(pUDPStats)) != NO_ERROR) {
		cout << " Getting the ip stats failed with an error" << endl;
	}
	numofcon = pUDPStats->dwOutDatagrams; //monitors datagrams
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

		udpTableEntryCount = (int)pUdpTable->dwNumEntries;
		udpConnectionList = new string *[udpTableEntryCount];
		for (int i = 0; i < udpTableEntryCount; i++){
			udpConnectionList[i] = new string[2];
		}
		
		for (int i = 0; i <udpTableEntryCount; i++) {
			IpAddr.S_un.S_addr = (u_long)pUdpTable->table[i].dwLocalAddr;
			strcpy_s(szLocalAddr, sizeof(szLocalAddr), inet_ntoa(IpAddr));

			localIp = szLocalAddr;
			udpConnectionList[i][0] = localIp;
			
			localPort = "";
			localPort += std::to_string(ntohs((u_short)pUdpTable->table[i].dwLocalPort));
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

}

string **UdpTableAccess::getUTable() {
	getUdpTable();
	return udpConnectionList;
}

int UdpTableAccess::getTableSize() {
	return udpTableEntryCount;
}