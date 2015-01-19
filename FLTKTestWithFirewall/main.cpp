#define WIN32_LEAN_AND_MEAN
#include <Fl/fl_ask.H>

#include "MyWindow.h"

using namespace std;
//gives time for threads to exit and program to close to prevent
//unhandled exceptions
static void window_cb(Fl_Widget* widget, void*) {
	Fl_Window *myWindow = (Fl_Window*)widget;

	//if (fl_ask("Exit?"))
	//{
		myWindow->hide();
		Sleep(2000);
		exit(0);
	//}

}

int main()
{
	MyWindow win(900, 900, "Network Monitor");
	win.callback(window_cb);	
	return Fl::run();
}