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
//starts the thread
void TcpTableAccess::startThread()
{
    _beginthread(TcpTableAccess::enterThread, 0, this);
}

void TcpTableAccess::enterThread(void *p)
{
    ((TcpTableAccess *) p)->threadBody();
    _endthread();
    return;
}

void TcpTableAccess::threadBody()
{
	getNetworkParameters();
	//getTcpTable();
	exitThread();
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
    FIXED_INFO *pFixedInfo; //object passed to the function
    ULONG ulOutBufLen; //passed to function as well
    //IP_ADDRESS_STRING *pIPAddr; //IP Address to use

    PMIB_TCPSTATS pTCPStats;

    DWORD dwRetVal;
    PMIB_TCPTABLE pTcpTable;
    DWORD dwSize = 0;

    char szLocalAddr[128];
    char szRemoteAddr[128];


    struct in_addr IpAddr;

    pFixedInfo = (FIXED_INFO *)malloc(sizeof(FIXED_INFO));
    ulOutBufLen = sizeof(FIXED_INFO); //correct the buffer size so it can hold the data;

    pTCPStats = (MIB_TCPSTATS *)malloc(sizeof (MIB_TCPSTATS));
    if (pTCPStats == NULL) {
        cout << "Error allocating memory for TCP" << endl;
    }

    if ((dwRetVal = GetTcpStatistics(pTCPStats)) != NO_ERROR) {
        cout << " Getting the ip stats failed with an error" << endl;
    }
    numofcon = pTCPStats->dwNumConns;
    //numberOfConnections = (string("<font color=darkslategray>Number of connections:</font> ") + string(std::to_string(pTCPStats->dwNumConns)) + string("<br>"));
   // tcpTableInformation += numberOfConnections;


    pTcpTable = (MIB_TCPTABLE *) malloc(sizeof (MIB_TCPTABLE));
    if (pTcpTable == NULL) {
        printf("Error allocating memory\n");

    }


    dwSize = sizeof (MIB_TCPTABLE);
    // Make an initial call to GetTcpTable to
    // get the necessary size into the dwSize variable
    if ((dwRetVal = GetTcpTable(pTcpTable, &dwSize, TRUE)) == ERROR_INSUFFICIENT_BUFFER) {
        free(pTcpTable);
        pTcpTable = (MIB_TCPTABLE *) malloc(dwSize);
        if (pTcpTable == NULL) {
            tcpTableInformation += "Error allocating memory<br>";

        }
    }

    // Make a second call to GetTcpTable to get
    // the actual data we require

    if ((dwRetVal = GetTcpTable(pTcpTable, &dwSize, TRUE)) == NO_ERROR) {
       // tcpTableInformation += "<font color=darkslategray>Number of entries: </font>" + string(std::to_string((int)pTcpTable->dwNumEntries)) + "<br><br>";

        for (int i = 0; i < (int)pTcpTable->dwNumEntries; i++) {
            IpAddr.S_un.S_addr = (u_long)pTcpTable->table[i].dwLocalAddr;
            strcpy_s(szLocalAddr, sizeof (szLocalAddr), inet_ntoa(IpAddr)); //local address
            IpAddr.S_un.S_addr = (u_long)pTcpTable->table[i].dwRemoteAddr;
            strcpy_s(szRemoteAddr, sizeof (szRemoteAddr), inet_ntoa(IpAddr)); //remote address

         //   tcpTableInformation += "<font color=purple>TCP State: </font> - ";
            switch (pTcpTable->table[i].dwState) { //switch statement gets state of connection
            case MIB_TCP_STATE_CLOSED:
        //        tcpTableInformation += string("CLOSED<br>");
                break;
            case MIB_TCP_STATE_LISTEN:
         //       tcpTableInformation += string("LISTEN<br>");
                break;
            case MIB_TCP_STATE_SYN_SENT:
         //       tcpTableInformation += string("SYN-SENT<br>");
                break;
            case MIB_TCP_STATE_SYN_RCVD:
        //        tcpTableInformation += string("SYN-RECEIVED<br>");
                break;
            case MIB_TCP_STATE_ESTAB:
         //       tcpTableInformation += string("ESTABLISHED<br>");
                break;
            case MIB_TCP_STATE_FIN_WAIT1:
          //      tcpTableInformation += string("FIN-WAIT-1<br>");
                break;
            case MIB_TCP_STATE_FIN_WAIT2:
          //      tcpTableInformation += string("FIN-WAIT-2<br>");
                break;
            case MIB_TCP_STATE_CLOSE_WAIT:
         //       tcpTableInformation += string("CLOSE-WAIT<br>");
                break;
            case MIB_TCP_STATE_CLOSING:
         //       tcpTableInformation += string("CLOSING<br>");
                break;
            case MIB_TCP_STATE_LAST_ACK:
          //      tcpTableInformation += string("LAST-ACK<br>");
                break;
            case MIB_TCP_STATE_TIME_WAIT:
          //      tcpTableInformation += string("TIME-WAIT<br>");
                break;
            case MIB_TCP_STATE_DELETE_TCB:
           //     tcpTableInformation += string("DELETE-TCB<br>");
                break;
            default:
          //      tcpTableInformation += string("UNKNOWN dwState value<br>");
                break;
            }
           // tcpTableInformation += string("<font color=green>TCP Local Addr:</font> ") + string(szLocalAddr) + "<br>";
           // tcpTableInformation += string("<font color=green>TCP Local Port: </font> ") + std::to_string(ntohs((u_short)pTcpTable->table[i].dwLocalPort)) + "<br>";
           // tcpTableInformation += string("<font color=blue>TCP Remote Addr:</font> ") + szRemoteAddr + "<br>";
            TcpTableAccess::addressVector.push_back(string(szRemoteAddr));
           // tcpTableInformation += string("<font color=blue>TCP Remote Port:</font> ") + std::to_string(ntohs((u_short)pTcpTable->table[i].dwRemotePort)) + "<br>";
          //  tcpTableInformation += "<br>";
        }
    }
    else {
        tcpTableInformation += string("GetTcpTable failed with <br>") + std::to_string(dwRetVal);
        free(pTcpTable);

    }

    if (pTcpTable != NULL) {
        free(pTcpTable);
        pTcpTable = NULL;
    }
  //  tcpfname = "tcptableinfo";
  //  tcpfnametemp = "temptcptableinfo";
   // ftpo.writeToTempLog(tcpfnametemp, tcpTableInformation);
  //  ftpo.writeToLog(tcpfname,tcpTableInformation);
    
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

const char *TcpTableAccess::getSomething()
{
	getNetworkParameters();
	return tcpTableInformation.c_str();
}

void TcpTableAccess::exitThread()
{	
	startThread();
	Sleep(20000);
}
vector<string> TcpTableAccess::addressVector;
