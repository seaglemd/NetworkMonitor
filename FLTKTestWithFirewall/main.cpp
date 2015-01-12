#define WIN32_LEAN_AND_MEAN

#include "MyWindow.h"

using namespace std;

int main()
{
	MyWindow win(900, 900, "Tab Window Firewall Test");
	
	return Fl::run();
}