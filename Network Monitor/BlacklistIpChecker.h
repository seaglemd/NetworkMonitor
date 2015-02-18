/****************************************************************
*Header file to forward declare variables/methods for blacklist *
*ip lookup using curl libraries.                                *
****************************************************************/
#ifndef BLACKLISTIPCHECKER_H
#define BLACKLISTIPCHECKER_H
//include needed libraries/.h files
#include <iostream>
#include <string>
#include <curl.h>

using namespace std;
class BlacklistIpChecker
{
public:
	BlacklistIpChecker::BlacklistIpChecker(); //constructor initializes url data and some sentinels
	void BlacklistIpChecker::getBlacklistResult(string **nTcpList, int nTcpListSize);//creates a status list which references information for ip status
	int *BlacklistIpChecker::getStatusList(); //returns the status list, an array of ints
	int BlacklistIpChecker::getStatusListSize(); //returns the size of the status list (mainly for some assurances that things haven't changed elsewhere no out of bounds)
private:
	int *ipStatusList;
	int ipStatusListSize;
	CURL* curl; //our curl object
	CURLcode result;
	char *data; //initial login data
	string searchIpFirst; //first part of post data for ip searching
	string searchIpSecond; //second part of ip post data for ip searching
	string searchIpFull; //will contain the entire post data
 
	string htmlReturned;//contains the returned html page data

};
#endif