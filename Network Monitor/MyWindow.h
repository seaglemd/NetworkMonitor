#ifndef MYWINDOW_H
#define MYWINDOW_H

#include <iostream>

#include "wfStatusAccess.h"
#include "tcpTableCellDisplay.h"
#include "reverseDnsCellDisplay.h"
//#include "tcpTableAccess.h"
#include "udpTableCellDisplay.h"
//#include "udpTableAccess.h"

class MyWindow;
void redrawBoxes_cb(void *u);
void button_cb(Fl_Widget *widget, void *u);
using namespace std;

int redrawRDNSTable;
int button = 0;

MyWindow *theWindow;
TCPTable *table;
UDPTable *uTable;
//RDNSTable *rTable;

Fl_Box *privateFirewallBox;
Fl_Box *publicFirewallBox;
Fl_Box *hostNameTextBox;
Fl_Box *domainNameTextBox;
Fl_Box *dnsServerListTextBox;
Fl_Box *numberOfConnectionsTextBox;
Fl_Button *refreshButtonBox;
Fl_Box *numberOfUdpTableEntriesTextBox;
Fl_Box *numberOfDatagramsTextBox;


class MyWindow : public Fl_Double_Window
{
	public:
		MyWindow(int w, int h, const char* title);
		//int MyWindow::handle(int e);
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
		Fl_Box *numberOfUdpTableEntriesTextBoxLabel;
		Fl_Box *numberOfDatagramsTextBoxLabel;

		/*
		Images
		*/
		Fl_PNG_Image *firewallOn;
		Fl_PNG_Image *firewallOff;
		Fl_PNG_Image *refreshImage;
		
		//Firewall class
		WFStatus *firewallStatus;
		//Connection information
		TcpTableAccess *tcpConnectionInfo;
		UdpTableAccess *udpConnectionInfo;
		//thread for regular update intervals
		void MyWindow::startThread();
		void MyWindow::threadBody();		
		static void enterThread(void *p);
		//reverse dns thread
		//reverse dns has it's own thread because of conflicts
		//with the time it takes to get the results and 
		//the speed at which the ui should react.
		void MyWindow::startRDNSThread();
		void MyWindow::rdnsThreadBody();
		static void enterRDNSThread(void *p);
		
		MyWindow *MyWindow::getWindow();
		void MyWindow::setCurrentFirewallStatus();
		void MyWindow::getCurrentTCPTableInfo();
		void MyWindow::getCurrentUDPTableInfo();

		void MyWindow::initializeObjects();

};

MyWindow::MyWindow(int w, int h, const char* title):Fl_Double_Window(w, h, title)
{
	redrawRDNSTable = 0;
	firewallStatus = new WFStatus();
	firewallOn = new Fl_PNG_Image("fWOn.png");
	firewallOff = new Fl_PNG_Image("fwoff.png");
	refreshImage = new Fl_PNG_Image("refresh.png");
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
			 //rTable = new RDNSTable(table, 600, 200, 270, 215);
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
			 numberOfConnectionsTextBox = new Fl_Box(700, 170, 75, 25);
			 numberOfConnectionsTextBox->box(FL_DOWN_BOX);
			 numberOfConnectionsTextBox->label("Pending...");

			 refreshButtonBox = new Fl_Button(825, 225, 25, 25, "re");
			 //refreshButtonBox->image(refreshImage);
			 refreshButtonBox->callback(button_cb);

	      tabSectionTCPTable->end();
		  tabSectionUDPTable = new Fl_Group(30, 55, 500 - 20, 200 - 45, "UDP Table");
		     uTable = new UDPTable(35, 65, 365, 350);
		     udpConnectionInfo = uTable->getUdpObject();

			 numberOfUdpTableEntriesTextBoxLabel = new Fl_Box(400, 65, 100, 25);
			 numberOfUdpTableEntriesTextBoxLabel->label("Udp Entries: ");
			 numberOfUdpTableEntriesTextBox = new Fl_Box(500, 65, 50, 25);
			 numberOfUdpTableEntriesTextBox->box(FL_DOWN_BOX);
			 numberOfUdpTableEntriesTextBox->label("Pending...");

		     numberOfDatagramsTextBoxLabel = new Fl_Box(400, 100, 100, 25);
		     numberOfDatagramsTextBoxLabel->label("Datagrams: ");
		     numberOfDatagramsTextBox = new Fl_Box(500, 100, 75, 25);
		     numberOfDatagramsTextBox->box(FL_DOWN_BOX);
		     numberOfDatagramsTextBox->label("Pending...");

		  tabSectionUDPTable->end();
	   tabGroup->end();
	end();
	setCurrentFirewallStatus();
	resizable(this);
	this->icon((char*)LoadIcon(fl_display, MAKEINTRESOURCE(101)));
	show();
	theWindow = this;
	startThread();
	//startRDNSThread();
}
/*
int MyWindow::handle(int e)
{/*
	if (e == FL_PUSH){
		if (Fl::event_button() == FL_LEFT_MOUSE)
		{
			button_cb();
			return 1;
		}
	}
	return 0;
}
*/
void MyWindow::setCurrentFirewallStatus() 
{
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
		hostNameTextBox->label(tcpConnectionInfo->getHostName());
		domainNameTextBox->label(tcpConnectionInfo->getDomainName());
		dnsServerListTextBox->label(tcpConnectionInfo->getDnsServerList());
		numberOfConnectionsTextBox->label(tcpConnectionInfo->getNumberOfConnections());
}

void MyWindow::getCurrentUDPTableInfo() 
{
	numberOfUdpTableEntriesTextBox->label(udpConnectionInfo->getDisplayedTableSize());
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
	cout << button << endl;
	//Fl::lock();
	   table->redrawTable(table);
	   //if (redrawRDNSTable == 1)
	      //rTable->redrawTable(rTable);

	   uTable->redrawTable(uTable);
	   theWindow->redraw();
	//Fl::unlock();
	Fl::awake();
}

void button_cb(Fl_Widget *widget, void *u)
{
	button++;
}

void MyWindow::startRDNSThread()
{
	_beginthread(MyWindow::enterRDNSThread, 0, this);
}

void MyWindow::enterRDNSThread(void *p)
{
	((MyWindow *)p)->rdnsThreadBody();
	_endthread();
	return;
}

void MyWindow::rdnsThreadBody()
{
	while (true){
		Sleep(30000);
		if (tcpConnectionInfo->getDataState() != 1){
			//rTable->updateCells();
		}
		
		redrawRDNSTable = 1;
	}
}

MyWindow::~MyWindow(){}

#endif