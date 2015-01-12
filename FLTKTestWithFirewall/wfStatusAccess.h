#ifndef WFSTATUSACCESS_H
#define WFSTATUSACCESS_H
#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <string>
#include <windows.h>
#include <stdio.h>
#include <netfw.h>
#include <process.h>

using namespace std;

#pragma comment( lib, "ole32.lib" )

class WFStatus
{
	public:
		void getFirewallInfo();
		bool getPrivateProfile();
		bool getPublicProfile();
		void startThread();
private:

	bool privateProfile;
	bool publicProfile;

	string statusLightPrivate;

    static void enterThread(void *p);
    void threadBody();

    void Get_FirewallSettings_PrivateProfileType(NET_FW_PROFILE_TYPE2 ProfileTypePassed, INetFwPolicy2* pNetFwPolicy2);
    void Get_FirewallSettings_PublicProfileType(NET_FW_PROFILE_TYPE2 ProfileTypePassed, INetFwPolicy2* pNetFwPolicy2);
    HRESULT WFCOMInitialize(INetFwPolicy2** ppNetFwPolicy2);
};

#endif
