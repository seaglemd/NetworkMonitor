#define WIN32_LEAN_AND_MEAN

#include "MyWindow.h"

using namespace std;

int main()
{
	MyWindow win(900, 900, "Network Monitor");
	
	return Fl::run();
}