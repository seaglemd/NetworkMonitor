#ifndef MYWINDOW_H
#define MYWINDOW_H

#include <iostream>

#include "wfStatusAccess.h"
#include "tcpTableCellDisplay.h"
#include "tcpTableAccess.h"
#include "udpTableCellDisplay.h"
#include "udpTableAccess.h"

class MyWindow;
void redrawBoxes_cb(void *u);

using namespace std;

MyWindow *theWindow;
TCPTable *table;
UDPTable *uTable;

Fl_Box *privateFirewallBox;
Fl_Box *publicFirewallBox;
Fl_Box *hostNameTextBox;
Fl_Box *domainNameTextBox;
Fl_Box *dnsServerListTextBox;
Fl_Box *numberOfConnectionsTextBox;
Fl_Box *numberOfDatagramsTextBox;


class MyWindow : public Fl_Double_Window{
	public:
		MyWindow(int w, int h, const char* title);
		~MyWindow();
		int refreshCount = 0;
	private:
		/*
		Tab group setup and tab sections setup
		*/
		Fl_Tabs *tabGroup;
		Fl_Group *tabSectionFirewall;
		Fl_Group *tabSectionTCPTable;
		Fl_Group *tabSectionUDPTable;

		/*
		Text and image boxes for firewall
		*/
		Fl_Box *privateFirewallTextBox;
		Fl_Box *publicFirewallTextBox;
		
		/*
		Text boxes for TCP Table information
		*/
		Fl_Box *hostNameTextBoxLabel;

		Fl_Box *domainNameTextBoxLabel;

		Fl_Box *dnsServerListTextBoxLabel;
		Fl_Box *numberOfConnectionsTextBoxLabel;
		/*
		Text boxes for UDP Table information
		*/
		Fl_Box *numberOfDatagramsTextBoxLabel;

		/*
		Images
		*/
		Fl_PNG_Image *firewallOn;
		Fl_PNG_Image *firewallOff;
		
		//Firewall class
		WFStatus *firewallStatus;
		//Connection information
		TcpTableAccess *tcpConnectionInfo;
		UdpTableAccess *udpConnectionInfo;

		void MyWindow::startThread();
		void MyWindow::threadBody();
		MyWindow *MyWindow::getWindow();
		static void enterThread(void *p);
		
		void MyWindow::setCurrentFirewallStatus();
		void MyWindow::getCurrentTCPTableInfo();
		void MyWindow::getCurrentUDPTableInfo();

		void MyWindow::initializeObjects();

};

MyWindow::MyWindow(int w, int h, const char* title):Fl_Double_Window(w, h, title){
	firewallStatus = new WFStatus();
	firewallOn = new Fl_PNG_Image("fWOn.png");
	firewallOff = new Fl_PNG_Image("fwoff.png");	
	begin();
	
	   tabGroup = new Fl_Tabs(10, 10, 900 - 20, 500 - 20);
	      tabSectionFirewall = new Fl_Group(30, 55, 900 - 20, 500 - 45, "Firewall");
		  
		     publicFirewallTextBox = new Fl_Box(50,70,75,25);
		     publicFirewallTextBox->label("Public Firewall Status: ");
		     privateFirewallTextBox = new Fl_Box(50,135,75,25);
			 privateFirewallTextBox->label("Private Firewall Status: ");
		     publicFirewallBox = new Fl_Box(150, 35, 100, 100);
	         privateFirewallBox = new Fl_Box(150, 100, 100, 100);	         

	      tabSectionFirewall->end();
	      tabSectionTCPTable = new Fl_Group(30, 55, 500 - 20, 200 - 45, "TCP Table");
		     table = new TCPTable(35, 65, 535, 350);
			 tcpConnectionInfo = table->getTcpObject();
			 hostNameTextBoxLabel = new Fl_Box(600, 65, 100, 25);
			 hostNameTextBoxLabel->label("Host Name: ");
			 hostNameTextBox = new Fl_Box(700, 65, 150, 25);
			 hostNameTextBox->box(FL_DOWN_BOX);
			 hostNameTextBox->label("Pending...");		

			 domainNameTextBoxLabel = new Fl_Box(600, 100, 100, 25);
			 domainNameTextBoxLabel->label("Domain Name: ");
			 domainNameTextBox = new Fl_Box(700, 100, 100, 25);
			 domainNameTextBox->box(FL_DOWN_BOX);
			 domainNameTextBox->label("Pending...");

			 dnsServerListTextBoxLabel = new Fl_Box(600, 135, 100, 25);
			 dnsServerListTextBoxLabel->label("DNS Servers: ");
			 dnsServerListTextBox = new Fl_Box(700, 135, 100, 25);
			 dnsServerListTextBox->box(FL_DOWN_BOX);
			 dnsServerListTextBox->label("Pending...");

			 numberOfConnectionsTextBoxLabel = new Fl_Box(600, 170, 100, 25);
			 numberOfConnectionsTextBoxLabel->label("Num of Cons: ");
			 numberOfConnectionsTextBox = new Fl_Box(700, 170, 100, 25);
			 numberOfConnectionsTextBox->box(FL_DOWN_BOX);
			 numberOfConnectionsTextBox->label("Pending...");

	      tabSectionTCPTable->end();
		  tabSectionUDPTable = new Fl_Group(30, 55, 500 - 20, 200 - 45, "UDP Table");
		     uTable = new UDPTable(35, 65, 535, 350);
		     udpConnectionInfo = uTable->getUdpObject();
		     numberOfDatagramsTextBoxLabel = new Fl_Box(600, 65, 100, 25);
		     numberOfDatagramsTextBoxLabel->label("Datagrams: ");
		     numberOfDatagramsTextBox = new Fl_Box(700, 65, 150, 25);
		     numberOfDatagramsTextBox->box(FL_DOWN_BOX);
		     numberOfDatagramsTextBox->label("Pending...");

		  tabSectionUDPTable->end();
	   tabGroup->end();
	end();
	setCurrentFirewallStatus();
	resizable(this);
	show();
	theWindow = this;
	startThread();
}

void MyWindow::setCurrentFirewallStatus() {
	if (!firewallStatus->getPrivateProfile())
		privateFirewallBox->image(firewallOff);
	else
		privateFirewallBox->image(firewallOn);

	if (!firewallStatus->getPublicProfile())
		publicFirewallBox->image(firewallOff);
	else
		publicFirewallBox->image(firewallOn);
}

void MyWindow::getCurrentTCPTableInfo() 
{
		cout << tcpConnectionInfo->getNumberOfConnections() << endl;
		hostNameTextBox->label(tcpConnectionInfo->getHostName());
		domainNameTextBox->label(tcpConnectionInfo->getDomainName());
		dnsServerListTextBox->label(tcpConnectionInfo->getDnsServerList());
		numberOfConnectionsTextBox->label(tcpConnectionInfo->getNumberOfConnections());
}

void MyWindow::getCurrentUDPTableInfo() {
	numberOfDatagramsTextBox->label(udpConnectionInfo->getDatagrams());
}

void MyWindow::startThread()
{
	_beginthread(MyWindow::enterThread, 0, this);
}

void MyWindow::enterThread(void *p)
{
	((MyWindow *)p)->threadBody();
	_endthread();
	return;
}

void MyWindow::threadBody()
{
	while (true){
		setCurrentFirewallStatus();
		getCurrentTCPTableInfo();
		getCurrentUDPTableInfo();
		if (tcpConnectionInfo->getDataState() == 1){			
			table->updateCells();
			tcpConnectionInfo->setDataState(0);
		}
		if (udpConnectionInfo->getDataState() == 1){
			uTable->updateCells();
			udpConnectionInfo->setDataState(0);
		}
		Fl::awake();
		Fl::awake(redrawBoxes_cb);
	}
}

MyWindow *MyWindow::getWindow()
{
	return this;
}

void redrawBoxes_cb(void *u)
{
	//cout << "made it here" << endl;
	Fl::lock();
	table->redrawTable(table);
	uTable->redrawTable(uTable);
	theWindow->redraw();
	Fl::unlock();
	Fl::awake();
}

MyWindow::~MyWindow(){}

#endif