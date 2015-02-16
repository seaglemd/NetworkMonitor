#ifndef BLACKLISTIPCHECKER_H
#define BLACKLISTIPCHECKER_H
#include <iostream>
#include <string>
#include <curl.h>

using namespace std;
class BlacklistIpChecker
{
public:
	BlacklistIpChecker::BlacklistIpChecker();
	void BlacklistIpChecker::getBlacklistResult(string **nTcpList, int nTcpListSize);
	int *BlacklistIpChecker::getStatusList();
	int BlacklistIpChecker::getStatusListSize();
private:
	int *ipStatusList;
	int ipStatusListSize;
	CURL* curl; //our curl object
	CURLcode result;
	char *data;
	string searchIpFirst;
	string searchIpSecond;
	string searchIpFull;

	string statusString;
	string htmlReturned;
	string ipStatus;
};
#endif