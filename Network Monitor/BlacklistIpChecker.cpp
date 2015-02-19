#include "BlacklistIpChecker.h"

using namespace std;
//callback which is used to store the data that is retrieved from the http request
static size_t writeCallback(char* buf, size_t size, size_t nmemb, void* up){
	((string*)up)->append((char*)buf, size*nmemb);
	return size * nmemb;
}
//Constructor which initializes data
BlacklistIpChecker::BlacklistIpChecker()
{
	//login post data
	data = "USER_LOGIN=networkmonitorblip%40gmail.com&USER_LOGIN_PASS=networkblip&USER_AUTOLOGIN=Y&submit_form=SIGN+IN"; 
	searchIpFirst = "ip=";
	searchIpSecond = "&submit_form=Search";
	searchIpFull = "";//will contain full post data for the current ip address to lookup
	htmlReturned = ""; //will contain the html data that was retrieved from the http request
	ipStatusListSize = 0; 
}
/*
Fills a status array with whether each ip is good/bad/neutral etc
0 is neutral
1 is good
2 is bad
*/
void BlacklistIpChecker::getBlacklistResult(string **nTcpList, int nTcpListSize)
{
	string ip = ""; //will contain the ip to be put in the post data for search
	size_t found; //used for finding the position after the ip address that will looked up

	if (ipStatusListSize > 0){ //if there is already a status list it is deleted and then recreated
		delete[]ipStatusList;
	}
	ipStatusListSize = nTcpListSize;
	ipStatusList = new int[ipStatusListSize];//setting the status list
	curl_global_init(CURL_GLOBAL_DEFAULT); //pretty obvious
	curl = curl_easy_init(); //init curl

	curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0"); //faking mozilla
	curl_easy_setopt(curl, CURLOPT_AUTOREFERER, 1); //for website that referred
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);//tells curl to follow to whatever location it is sent to
	curl_easy_setopt(curl, CURLOPT_COOKIEFILE, ""); //fakes a cookie file so it thinks we accept cookies

	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); //used for SSL login
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L); //used for SSL login
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback); //gives curl the address of the function that should collect html data
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &htmlReturned); //writes the html returned into the string for processing

	curl_easy_setopt(curl, CURLOPT_REFERER, "http://www.projecthoneypot.org/index.php"); //where did we come from?
	curl_easy_setopt(curl, CURLOPT_URL, "http://www.projecthoneypot.org/account_login.php"); //page we are going to
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);//attach login data on the end 
	result = curl_easy_perform(curl); //tell it to perform the operations
	if(htmlReturned.find("Dashboard") != string::npos){ //as long as Dashboard exists in the html we know we are logged in
		htmlReturned.clear(); //don't care about first page so we dump this html information
		for (int i = 0; i < ipStatusListSize; i++){ //looks at the size of the ipAddress array to determine how many lookups will be done
			found =nTcpList[i][1].find(':'); //finds the end of the ip address
			//this if statement check to make sure we don't look up a local address
			if (string((nTcpList[i][1].substr(0, (found))).c_str()).compare(0, 1, "0") != 0 &&
				string((nTcpList[i][1].substr(0, (found))).c_str()).compare(0, 3, "127") != 0 &&
				string((nTcpList[i][1].substr(0, (found))).c_str()).compare(0, 3, "192") != 0 &&
				string((nTcpList[i][1].substr(0, (found))).c_str()).compare(0, 2, "10") != 0){
				ip = nTcpList[i][1].substr(0, (found)); //finally have the actual ip

				searchIpFull = searchIpFirst + ip + searchIpSecond; //creates the post string that will be given to the website
				curl_easy_setopt(curl, CURLOPT_REFERER, "http://www.projecthoneypot.org/home.php"); //where we came from
				curl_easy_setopt(curl, CURLOPT_URL, "http://www.projecthoneypot.org/search_ip.php"); //where we are going to give the post information
				curl_easy_setopt(curl, CURLOPT_POSTFIELDS, (searchIpFull.c_str())); //data must be pasted in a c style terminated string, so string is c_str() now
				result = curl_easy_perform(curl); //perform lookup
				//uses a general text data to determine ip status in the following if-else clauses
				if (htmlReturned.find("The Project Honey Pot system") != string::npos){
					ipStatusList[i] = 2; //bad					
				}
				if (htmlReturned.find("(Not yet scored. Check again soon.)") != string::npos){
					ipStatusList[i] = 1; //good 					
				}
				
				if (htmlReturned.find("This IP addresses has been seen by at least one Honey Pot. However, none of its visits have resulted in any bad events yet. It's possible that this IP is just a harmless web spider or Internet user.") != string::npos){
					ipStatusList[i] = 1; //good					
				}
				if (htmlReturned.find("If you know something about this search engine IP") != string::npos){
					ipStatusList[i] = 1; //good					
				}
				htmlReturned.clear(); //clears html string and loop returns to stop and run next address
			}
			else
			{
				ipStatusList[i] = 0; //neutral
				htmlReturned.clear();
			}			
		}
	}
	else
	{
		for (int i = 0; i < ipStatusListSize; i++) //if we didnt' login correctly creates a neutral list
			ipStatusList[i] = 0;
	}
	//cleans everything up
	htmlReturned.clear();
	curl_easy_cleanup(curl);
	curl_global_cleanup();
}
//returns the array of ints relating to each ip address
int *BlacklistIpChecker::getStatusList()
{
	return ipStatusList;
}
//returns size of the status list
int BlacklistIpChecker::getStatusListSize()
{
	return ipStatusListSize;
}