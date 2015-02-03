#define WIN32_LEAN_AND_MEAN


#include "wfStatusAccess.h"
using namespace std;

#pragma comment( lib, "ole32.lib" )
/*
 * monitors the status of the firewall on windows
 * warns if the firewall is off
 * changes dynamically
 */
void WFStatus::startThread()
{
    _beginthread(WFStatus::enterThread, 0, this);
}

void WFStatus::enterThread(void *p)
{
    ((WFStatus *) p)->threadBody();
    _endthread();
    return;
}
//sleeps in order to prevent thread from running too quickly
//it eventually catches firwall updates
void WFStatus::threadBody()
{
	getFirewallInfo();
    Sleep(20000);
}
//gets information on the firewall including different policies
void WFStatus::getFirewallInfo()
{
    HRESULT hrComInit = S_OK;
    HRESULT hr = S_OK;

    INetFwPolicy2 *pNetFwPolicy2 = NULL;

    // Initialize COM.
    hrComInit = CoInitializeEx(0, COINIT_APARTMENTTHREADED);

    // Ignore RPC_E_CHANGED_MODE; this just means that COM has already been
    // initialized with a different mode. Since we don't care what the mode is,
    // we'll just use the existing mode.
    if (hrComInit != RPC_E_CHANGED_MODE)
    {
        if (FAILED(hrComInit))
        {
            printf("CoInitializeEx failed: 0x%08lx\n", hrComInit);
            goto Cleanup;
        }
    }

    // Retrieve INetFwPolicy2
    hr = WFCOMInitialize(&pNetFwPolicy2);
    if (FAILED(hr))
    {
        goto Cleanup;
    }

    Get_FirewallSettings_PrivateProfileType(NET_FW_PROFILE2_PRIVATE, pNetFwPolicy2);
    Get_FirewallSettings_PublicProfileType(NET_FW_PROFILE2_PUBLIC, pNetFwPolicy2);

Cleanup:

    // Release INetFwPolicy2
    if (pNetFwPolicy2 != NULL)
    {
        pNetFwPolicy2->Release();
    }

    // Uninitialize COM.
    if (SUCCEEDED(hrComInit))
    {
        CoUninitialize();
    }
}

// Forward declarations

void WFStatus::Get_FirewallSettings_PrivateProfileType(NET_FW_PROFILE_TYPE2 ProfileTypePassed, INetFwPolicy2* pNetFwPolicy2)
{
    VARIANT_BOOL bIsEnabled = FALSE;
    NET_FW_ACTION action;

    if(SUCCEEDED(pNetFwPolicy2->get_FirewallEnabled(ProfileTypePassed, &bIsEnabled)))
    {
		if (bIsEnabled) {
			privateProfile = true;
		}
		else
			privateProfile = false;
    }
}

void WFStatus::Get_FirewallSettings_PublicProfileType(NET_FW_PROFILE_TYPE2 ProfileTypePassed, INetFwPolicy2* pNetFwPolicy2)
{
    VARIANT_BOOL bIsEnabled = FALSE;
    NET_FW_ACTION action;

    if(SUCCEEDED(pNetFwPolicy2->get_FirewallEnabled(ProfileTypePassed, &bIsEnabled)))
    {
		if (bIsEnabled) {
			publicProfile = true;
		}
		else
			publicProfile = false;
	}
}
HRESULT WFStatus::WFCOMInitialize(INetFwPolicy2** ppNetFwPolicy2)
{
    HRESULT hr = S_OK;

    hr = CoCreateInstance(
        __uuidof(NetFwPolicy2),
        NULL,
        CLSCTX_INPROC_SERVER,
        __uuidof(INetFwPolicy2),
        (void**)ppNetFwPolicy2);

    if (FAILED(hr))
    {
        printf("CoCreateInstance for INetFwPolicy2 failed: 0x%08lx\n", hr);
        goto Cleanup;
    }

Cleanup:
    return hr;
}

bool WFStatus::getPrivateProfile()
{
    getFirewallInfo();
    return privateProfile;
}
bool WFStatus::getPublicProfile()
{
	getFirewallInfo();
	return publicProfile;
}
