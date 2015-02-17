#ifndef MYWINDOW_H
#define MYWINDOW_H

#include "wfStatusAccess.h"
#include "tcpTableCellDisplay.h"
#include "reverseDnsCellDisplay.h"
#include "udpTableCellDisplay.h"


class MyWindow;
void redrawBoxes_cb(void *u);
void rdns_button_cb(Fl_Widget *widget, void *u);
void stop_button_tcp_cb(Fl_Widget *widget, void *u);
void resume_button_tcp_cb(Fl_Widget *widget, void *u);
void stop_button_udp_cb(Fl_Widget *widget, void *u);
void resume_button_udp_cb(Fl_Widget *widget, void *u);
void iplookup_button_cb(Fl_Widget *widget, void *u);
void event_cb(void*);

using namespace std;

int redrawRDNSTable;
int button = 0;
int stopTcp;
int startTcp;
int stopUdp;
int startUdp;
int startIpLookup;
int changeStopTcpLabel;
int changeResumeTcpLabel;
int changeIpLookupLabel;
int changeRDNSLabel;

MyWindow *theWindow;
TCPTable *table;
UDPTable *uTable;
RDNSTable *rTable;
Fl_Box *refreshButtonTextLabel;
Fl_Box *textStatusBox;

Fl_Button *stopButtonTcp;
Fl_Button *refreshButtonBox;
Fl_Button *resumeButtonTcp;
Fl_Button *ipLookupButton;


class MyWindow : public Fl_Double_Window
{
	public:
		MyWindow(int w, int h, const char* title);
		void MyWindow::startRDNSThread();
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

		Fl_Box *hostNameTextBox;
		Fl_Box *domainNameTextBox;
		Fl_Box *dnsServerListTextBox;
		Fl_Box *numberOfConnectionsTextBox;

		/*
		Text and image boxes for firewall
		*/
		Fl_Box *privateFirewallBox;
		Fl_Box *publicFirewallBox;
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

		Fl_Box *numberOfUdpTableEntriesTextBox;
		Fl_Box *numberOfDatagramsTextBox;

		/*
		Text Box for Status
		*/
		
		/*
		Images
		*/
		Fl_PNG_Image *firewallOn;
		Fl_PNG_Image *firewallOff;
		Fl_PNG_Image *refreshImage;		

		Fl_Button *stopButtonUdp;
		Fl_Button *resumeButtonUdp;		
		
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
		
		void MyWindow::rdnsThreadBody();
		static void enterRDNSThread(void *p);
		
		MyWindow *MyWindow::getWindow();
		void MyWindow::checkControlStatus();
		void MyWindow::setCurrentFirewallStatus();
		void MyWindow::getCurrentTCPTableInfo();
		void MyWindow::getCurrentUDPTableInfo();

		void MyWindow::initializeObjects();

};

MyWindow::MyWindow(int w, int h, const char* title):Fl_Double_Window(w, h, title)
{
	stopTcp = 0;
	startTcp = 0;
	stopUdp = 0;
	startUdp = 0;
	redrawRDNSTable = 0;
	changeStopTcpLabel = 0;
	changeResumeTcpLabel = 0;
	changeIpLookupLabel = 0;
	changeRDNSLabel = 0;
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
	      tabSectionTCPTable = new Fl_Group(30, 55, 900 - 20, 500 - 45, "TCP Table");
		     table = new TCPTable(35, 65, 535, 350);

			 stopButtonTcp = new Fl_Button(35, 420, 25, 25);
			 stopButtonTcp->labelcolor(FL_RED);
			 stopButtonTcp->labeltype(FL_SHADOW_LABEL);
			 stopButtonTcp->label("@square");
			 stopButtonTcp->callback(stop_button_tcp_cb);

			 resumeButtonTcp = new Fl_Button(65, 420, 25, 25);
			 resumeButtonTcp->labelcolor(FL_GRAY);
			 resumeButtonTcp->labeltype(FL_SHADOW_LABEL);
			 resumeButtonTcp->label("@+2>");
			 resumeButtonTcp->deactivate();
			 resumeButtonTcp->callback(resume_button_tcp_cb);

			 ipLookupButton = new Fl_Button(95, 420, 25, 25);
			 ipLookupButton->labelcolor(FL_GRAY);
			 ipLookupButton->labeltype(FL_SHADOW_LABEL);
			 ipLookupButton->label("@+28>");
			 ipLookupButton->deactivate();
			 ipLookupButton->callback(iplookup_button_cb);

			 tcpConnectionInfo = table->getTcpObject();


			 rTable = new RDNSTable(table, 600, 200, 270, 215);
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

			 refreshButtonBox = new Fl_Button(845, 420, 25, 25);
			 refreshButtonBox->image(refreshImage);
			 refreshButtonBox->callback(rdns_button_cb);

			 refreshButtonTextLabel = new Fl_Box(775, 420, 40, 25);
			 refreshButtonTextLabel->label("");

			 textStatusBox = new Fl_Box(35, 450, 450, 25);
			 textStatusBox->box(FL_DOWN_BOX);
			 textStatusBox->align(FL_ALIGN_INSIDE);
			 textStatusBox->label("");

	      tabSectionTCPTable->end();
		  tabSectionUDPTable = new Fl_Group(30, 55, 900 - 20, 500 - 45, "UDP Table");
		     uTable = new UDPTable(35, 65, 365, 350);

			 stopButtonUdp = new Fl_Button(35, 420, 25, 25);
			 stopButtonUdp->labelcolor(FL_RED);
			 stopButtonUdp->labeltype(FL_SHADOW_LABEL);
			 stopButtonUdp->label("@square");
			 stopButtonUdp->callback(stop_button_udp_cb);

			 resumeButtonUdp = new Fl_Button(65, 420, 25, 25);
			 resumeButtonUdp->labelcolor(FL_GRAY);
			 resumeButtonUdp->labeltype(FL_SHADOW_LABEL);
			 resumeButtonUdp->label("@+2>");
			 resumeButtonUdp->callback(resume_button_udp_cb);
			 resumeButtonUdp->deactivate();
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
	Fl::add_check(event_cb);
	startThread();
}

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
		checkControlStatus();
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

void MyWindow::checkControlStatus()
{
	if (stopTcp == 1){
		tcpConnectionInfo->stopUpdates();
		table->stopTableRefill();
		stopButtonTcp->labelcolor(FL_GRAY);
		stopButtonTcp->deactivate();
		resumeButtonTcp->labelcolor(FL_GREEN);
		resumeButtonTcp->activate();
		ipLookupButton->labelcolor(FL_BLUE);
		ipLookupButton->activate();
		stopTcp = 0;
	}
	if (startTcp == 1){
		table->startTableRefill();
		tcpConnectionInfo->startUpdates();
		resumeButtonTcp->labelcolor(FL_GRAY);
		resumeButtonTcp->deactivate();
		stopButtonTcp->labelcolor(FL_RED);
		stopButtonTcp->activate();
		ipLookupButton->labelcolor(FL_GRAY);
		ipLookupButton->deactivate();
		startTcp = 0;
	}
	if (stopUdp == 1){
		udpConnectionInfo->stopUpdates();
		stopButtonUdp->labelcolor(FL_GRAY);
		stopButtonUdp->deactivate();
		resumeButtonUdp->labelcolor(FL_GREEN);
		resumeButtonUdp->activate();
		stopUdp = 0;
	}
	if (startUdp == 1){
		udpConnectionInfo->startUpdates();
		resumeButtonUdp->labelcolor(FL_GRAY);
		resumeButtonUdp->deactivate();
		stopButtonUdp->labelcolor(FL_RED);
		stopButtonUdp->activate();
		startUdp = 0;
	}
	if (startIpLookup == 1){
		startIpLookup = 0;
		table->stopTableRefill();
		tcpConnectionInfo->stopUpdates();
		resumeButtonTcp->labelcolor(FL_GRAY);
		resumeButtonTcp->deactivate();
		table->blacklistChecker();
		resumeButtonTcp->labelcolor(FL_GREEN);
		resumeButtonTcp->activate();		
	}
	if (changeStopTcpLabel == 1){
		textStatusBox->label("Stops Watching for TCP table updates.");
		changeStopTcpLabel = 0;
	}
	if (changeStopTcpLabel == 2){
		textStatusBox->label("");
		changeStopTcpLabel = 0;
	}
	if (changeResumeTcpLabel == 1){
		textStatusBox->label("Begins watching for TCP table updates.");
		changeResumeTcpLabel = 0;
	}
	if (changeResumeTcpLabel == 2){
		textStatusBox->label("");
		changeResumeTcpLabel = 0;
	}
	if (changeIpLookupLabel == 1){
		textStatusBox->label("Begins lookup of IP's for blacklistings.");
		changeIpLookupLabel = 0;
	}
	if (changeIpLookupLabel == 2){
		textStatusBox->label("");
		changeIpLookupLabel = 0;
	}
	if (changeRDNSLabel == 1){
		textStatusBox->label("Begins reverse dns lookup from a snapshot of the current TCP table.");
		changeRDNSLabel = 0;
	}
	if (changeRDNSLabel == 2){
		textStatusBox->label("");
		changeRDNSLabel = 0;
	}
}
void redrawBoxes_cb(void *u)
{
	Fl::lock();
	
	if (redrawRDNSTable == 1){
		refreshButtonTextLabel->label("Complete");
		rTable->redrawTable(rTable);
		redrawRDNSTable = 0;
	}
	   table->redrawTable(table);
	   uTable->redrawTable(uTable);
	   theWindow->redraw();
	Fl::unlock();
	Fl::awake();
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
	rTable->updateCells();
	redrawRDNSTable = 1;
}
void rdns_button_cb(Fl_Widget *widget, void *u)
{
	refreshButtonTextLabel->label("Refreshing...");
	theWindow->startRDNSThread();
}
void stop_button_tcp_cb(Fl_Widget *widget, void *u)
{
	stopTcp = 1;
}
void resume_button_tcp_cb(Fl_Widget *widget, void *u)
{
	startTcp = 1;
}
void stop_button_udp_cb(Fl_Widget *widget, void *u)
{
	stopUdp = 1;
}
void resume_button_udp_cb(Fl_Widget *widget, void *u)
{
	startUdp = 1;
}
void iplookup_button_cb(Fl_Widget *widget, void *u)
{
	startIpLookup = 1;
}
void event_cb(void*)
{
	if (Fl::event_inside(stopButtonTcp) != 0){
		changeStopTcpLabel = 1;
	}
	if (Fl::event_inside(stopButtonTcp) == 0
		&& Fl::event_inside(resumeButtonTcp) == 0
		&& Fl::event_inside(ipLookupButton) == 0
		&& Fl::event_inside(refreshButtonBox) == 0){
		changeStopTcpLabel = 2;
	}
	if (Fl::event_inside(resumeButtonTcp) != 0){
		changeResumeTcpLabel = 1;
	}
	if (Fl::event_inside(resumeButtonTcp) == 0
		&& Fl::event_inside(stopButtonTcp) == 0
		&& Fl::event_inside(ipLookupButton) == 0
		&& Fl::event_inside(refreshButtonBox) == 0){
		changeResumeTcpLabel = 2;
	}
	if (Fl::event_inside(ipLookupButton) != 0){
		changeIpLookupLabel = 1;
	}
	if (Fl::event_inside(ipLookupButton) == 0
		&& Fl::event_inside(stopButtonTcp) == 0
		&& Fl::event_inside(resumeButtonTcp) == 0
		&& Fl::event_inside(refreshButtonBox) == 0){
		changeIpLookupLabel = 2;
	}
	if (Fl::event_inside(refreshButtonBox) != 0){
		changeRDNSLabel = 1;
	}
	if (Fl::event_inside(refreshButtonBox) == 0
		&& Fl::event_inside(ipLookupButton) == 0
		&& Fl::event_inside(stopButtonTcp) == 0
		&& Fl::event_inside(resumeButtonTcp) == 0){
		changeRDNSLabel = 2;
	}
}
MyWindow::~MyWindow(){}

#endif