#include "BlacklistIpChecker.h"

using namespace std;

static size_t writeCallback(char* buf, size_t size, size_t nmemb, void* up){
	((string*)up)->append((char*)buf, size*nmemb);
	return size * nmemb;
}

BlacklistIpChecker::BlacklistIpChecker()
{
	data = "USER_LOGIN=networkmonitorblip%40gmail.com&USER_LOGIN_PASS=networkblip&USER_AUTOLOGIN=Y&submit_form=SIGN+IN";
	searchIpFirst = "ip=";
	searchIpSecond = "&submit_form=Search";
	searchIpFull = "";
	htmlReturned = "";
	ipStatus = "";
	ipStatusListSize = 0;
}

void BlacklistIpChecker::getBlacklistResult(string **nTcpList, int nTcpListSize)
{
	cout << "in getBlacklistResult()" << endl;
	string ip = "";
	size_t found;

	if (ipStatusListSize > 0){
		delete[]ipStatusList;
	}
	ipStatusListSize = nTcpListSize;
	ipStatusList = new int[ipStatusListSize];
	curl_global_init(CURL_GLOBAL_DEFAULT); //pretty obvious
	curl = curl_easy_init(); //init curl

	curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0");
	curl_easy_setopt(curl, CURLOPT_AUTOREFERER, 1);
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
	curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "");

	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &htmlReturned);

	curl_easy_setopt(curl, CURLOPT_REFERER, "http://www.projecthoneypot.org/index.php");
	curl_easy_setopt(curl, CURLOPT_URL, "http://www.projecthoneypot.org/account_login.php");
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
	result = curl_easy_perform(curl);
	cout << "hair!" << endl;
	if(htmlReturned.find("Dashboard") != string::npos){
		htmlReturned.clear();
		cout << ipStatusListSize << endl;
		for (int i = 0; i < ipStatusListSize; i++){
			found =nTcpList[i][1].find(':');
			cout << found << "heh" << endl;
			if (string((nTcpList[i][1].substr(0, (found))).c_str()).compare(0, 1, "0") != 0 &&
				string((nTcpList[i][1].substr(0, (found))).c_str()).compare(0, 3, "127") != 0 &&
				string((nTcpList[i][1].substr(0, (found))).c_str()).compare(0, 3, "192") != 0 &&
				string((nTcpList[i][1].substr(0, (found))).c_str()).compare(0, 2, "10") != 0){
				ip = nTcpList[i][1].substr(0, (found));

				searchIpFull = searchIpFirst + ip + searchIpSecond;
				curl_easy_setopt(curl, CURLOPT_REFERER, "http://www.projecthoneypot.org/home.php");
				curl_easy_setopt(curl, CURLOPT_URL, "http://www.projecthoneypot.org/search_ip.php");
				curl_easy_setopt(curl, CURLOPT_POSTFIELDS, (searchIpFull.c_str()));
				result = curl_easy_perform(curl);

				if (htmlReturned.find("We don't have data on this IP currently. If you know something, you may") != string::npos){
					ipStatusList[i] = 1;
				}
				else if (htmlReturned.find("The Project Honey Pot system") != string::npos){
					ipStatusList[i] = 2;
				}
				else if (htmlReturned.find("none of its visits") != string::npos){
					ipStatusList[i] = 1;
				}
				else if (htmlReturned.find("search engine") != string::npos){
					ipStatusList[i] = 1;
				}
				htmlReturned.clear();
			}
			else
			{
				ipStatusList[i] = 0;
				cout << i << endl;
				htmlReturned.clear();
			}			
		}
	}
	else
	{
		for (int i = 0; i < ipStatusListSize; i++)
			ipStatusList[i] = 0;
		cout << ":(" << endl;
	}
	htmlReturned.clear();
	curl_easy_cleanup(curl);
	curl_global_cleanup();
}

int *BlacklistIpChecker::getStatusList()
{
	return ipStatusList;
}

int BlacklistIpChecker::getStatusListSize()
{
	return ipStatusListSize;
}