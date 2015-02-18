/***************************************
* Author: Matthew Seagle
* About: This program is meant to track TCP/UDP connections as well as monitor firewall status.
* Integrated is the ability to look up the tcp connections from the Project Honeypot database
* in order to determine potentially risky connections. Black is neutral, many times associated
* with 0.0.0.0 ip's for ports that are simply listening to incoming connections.
* Green currently is okay/good and probably a safe connection.
* Red is a connection that is potentially dangerous or may have warnings/known ties to anything
* from spamming to straight up worm/virus distribution.
* Threading is used to optimize the work done to consistently track the connections but tracking can
* be stopped or started at any time.
* There is a basic reverse DNS lookup which takes the TCP connections and tries to resolve a remote
* host that can be displayed. Many times there is no immediate name server associated with the ip and 
* and ip address is returned.
* 
* Libraries used in this program consist of FLTK 1.3.3 for the gui portion and the curllib to
* do the minimal web crawling needed to look up information on good/bad connections. The account
* used to log into Project Honeypot remotely through curl is one setup specifically for this so the
* ultimate risk of being compromised is low.
* 
* Anyone who sees this code is welcome to use/reproduce it in other ways as an example for others
* or for their own learning. It would be appreciated that it is not taken and reproduced for a profit
* without express consent from the author.
* 
* Any questions, ideas, or concerns can be voiced by emailing the author at networkmonitorblip@gmail.com
*/
#define WIN32_LEAN_AND_MEAN  //defined so libraries not included multiple times.

#include "MyWindow.h" //include main window of program

using namespace std;

//exit cleanly from FLTK
static void window_cb(Fl_Widget* widget, void*) 
{
	Fl_Window *myWindow = (Fl_Double_Window*)widget;
		myWindow->hide();
		exit(0);
}

int main()
{
	Fl::lock(); //threading is used so Fl::lock() is called
	MyWindow win(900, 500, "Network Monitor");
	win.callback(window_cb);	//callback for exit
	return Fl::run();
}