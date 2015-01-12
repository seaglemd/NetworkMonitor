#ifndef WINDOW_H
#define WINDOW_H
#include "wfStatusAccess.h"
#include "tcpTableCellDisplay.h"


using namespace std;

class MyWindow : public Fl_Double_Window{
	public:
		MyWindow(int w, int h, const char* title);
		void startThread();
		~MyWindow();
	private:
		Fl_Tabs *tabGroup;
		Fl_Group *tabSectionFirewall;
		Fl_Group *tabSectionTCPTable;

		Fl_Box *privateFirewallTextBox;
		Fl_Box *publicFirewallTextBox;
		Fl_Box *privateFirewallBox;
		Fl_Box *publicFirewallBox;
		Fl_Box *tcpTableBox;

		Fl_PNG_Image *firewallOn;
		Fl_PNG_Image *firewallOff;
		
		WFStatus* firewallStatus;

		static void enterThread(void *p);
		void threadBody();
		
		void MyWindow::setCurrentFirewallStatus();

		void MyWindow::redrawBoxes();
		void MyWindow::initializeObjects();
};

MyWindow::MyWindow(int w, int h, const char* title):Fl_Double_Window(w, h, title){
	firewallStatus = new WFStatus();
	firewallOn = new Fl_PNG_Image("fWOn.png");
	firewallOff = new Fl_PNG_Image("fwoff.png");

	begin();
	   tabGroup = new Fl_Tabs(10, 10, 900 - 20, 900 - 20);
	      tabSectionFirewall = new Fl_Group(30, 55, 900 - 20, 900 - 45, "Firewall");

		     publicFirewallTextBox = new Fl_Box(50,70,75,25);
		     publicFirewallTextBox->label("Public Firewall Status: ");
		     privateFirewallTextBox = new Fl_Box(50,135,75,25);
			 privateFirewallTextBox->label("Private Firewall Status: ");
			 publicFirewallBox = new Fl_Box(150, 35, 100, 100);
	         privateFirewallBox = new Fl_Box(150, 100, 100, 100);
	         setCurrentFirewallStatus();

	      tabSectionFirewall->end();
	      tabSectionTCPTable = new Fl_Group(30, 55, 900 - 20, 900 - 45, "TCP Table");
	         TCPTable table(50, 70, 800, 300);
	      tabSectionTCPTable->end();
	   tabGroup->end();
	end();
	resizable(this);
	show();
	startThread();
}

void MyWindow::redrawBoxes() {
	setCurrentFirewallStatus();
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

	privateFirewallBox->redraw();
	publicFirewallBox->redraw();
}

void MyWindow::startThread() {
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
	redrawBoxes();
	Sleep(2000);
}

MyWindow::~MyWindow(){}

#endif