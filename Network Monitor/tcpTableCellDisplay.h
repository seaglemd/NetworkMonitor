/******************************************************************
*This calls extends Fl_Table which is how the tables are created  *
*in FLTK. It takes the informationg gathered from tcpTableAccess  *
*and displays it. There is also control for copying the TCPTable  *
*and sending it to other classes such as the reverse DNS lookup   *
*and the Blacklist ip lookup.                                     *                                   
******************************************************************/
#ifndef TCPTABLECELLDISPLAY_H
#define TCPTABLECELLDISPLAY_H

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Tabs.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_PNG_Image.H>
#include <FL/Fl_Table.H>
#include <FL/fl_draw.H>
#include <string>

#include "tcpTableAccess.h"
#include "BlacklistIpChecker.h"

#define MAX_ROWST 10
#define MAX_COLST 3

using namespace std;

class TCPTable : public Fl_Table 
{
public:	
	//inline declaration of the TCP table constructor, standard for this inhereted class
	TCPTable::TCPTable(int X, int Y, int W, int H, const char *L = 0) : Fl_Table(X, Y, W, H, L){
		firstTime = 1; 
		tableSize = 0;
		copyTableSize = 0;
		haveStatusList = 0;
		drawStatusList = 0;
		ipStatusListSize = 0;
		curDrawRow = 0;
		ipStatusLookup = 0;
		sortDirection = 0;
		noRefill = 0;
		tcpConnections = new TcpTableAccess();
		blipl = new BlacklistIpChecker();
		initializeHeaderInformation();
		fillDataArray();

		headings[0] = "Local IP:Port";
		headings[1] = "Remote IP:Port";
		headings[2] = "State";

		rows(tableSize);
		row_header(0);              // enable row headers (along left)
		row_height_all(20);         // default height of rows
		row_resize(1);              // disable row resizing

		cols(MAX_COLST);             // how many columns
		col_header(1);              // enable column headers (along top)
		col_width_all(165);          // default width of columns
		col_resize(1);              // enable column resizing
		end();                        // end the Fl_Table group
	};
	void TCPTable::updateCells(); //calls fillDataArray safely
	TcpTableAccess *TCPTable::getTcpObject(); //passes tcp connection information
	void TCPTable::redrawTable(TCPTable *curTable); //redraws the table within the main thread
	int TCPTable::getDrawState(); //whether or not the table is being drawn
	string **TCPTable::getDataTable(); //actual TCP data being used
	int **TCPTable::getHeaderSizeInformation();
	int TCPTable::getDataTableSize(); //size of actual TCP data
	string **TCPTable::getTableCopy(); //copies the TCP Information
	int TCPTable::getCopyTableSize(); //size of the copy created
	int TCPTable::getIpStatusLookup(); //returns status of ipLookup progress for gui updates
	void TCPTable::flipData();
	void TCPTable::setIpStatusLookup();
	void TCPTable::stopTableRefill(); //stops the table from collecting new data
	void TCPTable::startTableRefill(); //starts the table from collecting new data
	void TCPTable::blacklistChecker(); //checks for blips
	TCPTable::~TCPTable();
private:
	int firstTime; //for cleaning
	int tableSize; 
	int copyTableSize;
	int noDraw;
	int noRefill;
	int haveStatusList;
	int drawStatusList;
	int ipStatusListSize;
	int curDrawRow;
	int *ipStatusList; //ip status list
	int ipStatusLookup;
	int **headerInformation;
	int sortDirection;
	string **data;  // data array for cells
	string headings[MAX_COLST];
	string **tcpList;
	TcpTableAccess *tcpConnections;
	BlacklistIpChecker *blipl;
	void TCPTable::mergeSortArrayLocal();
	void TCPTable::mergeHelper(string *input, int left, int right, string *scratch, string **dataScratch);
	void TCPTable::initializeHeaderInformation();
	void TCPTable::fillDataArray();
	//debug purposes
	void TCPTable::printHeaderInformation();
	//
	void TCPTable::startBlipThread();
	void TCPTable::threadBlipBody();
	static void enterBlipThread(void *p);
	
	//draws the table headers, inline as expected
	void TCPTable::DrawHeader(const char *s, int X, int Y, int W, int H){
		fl_push_clip(X, Y, W, H);
		if (noRefill != 1)
			fl_draw_box(FL_THIN_DOWN_BOX, X, Y, W, H, row_header_color());
		else if (noRefill == 1)
			fl_draw_box(FL_THIN_UP_BOX, X, Y, W, H, row_header_color());
		fl_color(FL_BLACK);
		fl_draw(s, X, Y, W, H, FL_ALIGN_CENTER);
		fl_pop_clip();
	}
	//draws the data, inline as expected
	void TCPTable::DrawData(const char *s, int X, int Y, int W, int H) {
		fl_push_clip(X, Y, W, H);
		// Draw cell bg
		fl_color(FL_WHITE); fl_rectf(X, Y, W, H);
		// Draw cell data
		if (haveStatusList == 1) //if blisted ips were checked for use this
		{
			if (ipStatusList[curDrawRow] == 0){				
				fl_color(FL_GRAY0); fl_draw(s, X, Y, W, H, FL_ALIGN_CENTER);
			}
			else if(ipStatusList[curDrawRow] == 1){
				fl_color(FL_GREEN); fl_draw(s, X, Y, W, H, FL_ALIGN_CENTER);
			}
			else if(ipStatusList[curDrawRow] == 2){
				fl_color(FL_RED); fl_draw(s, X, Y, W, H, FL_ALIGN_CENTER);
			}
		}
		else if (haveStatusList == 0){ //if there has been no check for blips
			fl_color(FL_GRAY0); fl_draw(s, X, Y, W, H, FL_ALIGN_CENTER);
		}
		// Draw box border
		fl_color(color()); fl_rect(X, Y, W, H);
		fl_pop_clip();
	}
	//draws the cells
	void TCPTable::draw_cell(TableContext context, int ROW = 0, int COL = 0, int X = 0, int Y = 0, int W = 0, int H = 0){
		Fl::lock();
		static char s[40];
		switch (context) {
		case CONTEXT_STARTPAGE:                   // before page is drawn..
			fl_font(FL_HELVETICA, 16);              // set the font for our drawing operations
			return;
		case CONTEXT_COL_HEADER:
			//sprintf(s, &(headings[COL])[0]);               // "A", "B", "C", etc.
			headerInformation[COL][0] = X;
			headerInformation[COL][1] = Y;
			headerInformation[COL][2] = W;
			headerInformation[COL][3] = H;
			DrawHeader(headings[COL].c_str(), X, Y, W, H);
			return;
		case CONTEXT_ROW_HEADER:                  // Draw row headers
			//sprintf(s, "%03d:", ROW);                 // "001:", "002:", etc
			//DrawHeader(s, X, Y, W, H);
			return;
		case CONTEXT_CELL:
			if (haveStatusList == 1)
				curDrawRow = ROW;
			if (ROW <= tableSize && noDraw == 0)
				DrawData(data[ROW][COL].c_str(), X, Y, W, H);			
			return;
		default:
			return;
		}

		Fl::unlock();
		return;
	}
	
	
};

//initializes header information array, used to store location data for events
void TCPTable::initializeHeaderInformation()
{
	headerInformation = new int*[3];
	for (int i = 0; i < 3; i++){
			headerInformation[i] = new int[4];
			headerInformation[i][0] = 0;
			headerInformation[i][1] = 0;
			headerInformation[i][2] = 0;
			headerInformation[i][3] = 0;
	}
}
//prevents reading and writing from the data array at the same time
void TCPTable::updateCells()
{
	noDraw = 1;
	if (noRefill == 0)
		fillDataArray();
	noDraw = 0;
}
//fills the array that is actually drawn from the data received from the tcp table
void TCPTable::fillDataArray()
{
	if (firstTime != 1){
		for (int i = 0; i < tableSize; i++)
			delete[]data[i];
		delete[] data;
	}
	
	tcpList = tcpConnections->passTcpTable();
	tableSize = tcpConnections->getTableSize();

	data = new string*[tableSize];
	for (int i = 0; i < tableSize; i++)
		data[i] = new string[3];

	for (int i = 0; i < tableSize; i++)
		for (int j = 0; j < 3; j++)
			data[i][j] = " ";

	for (int i = 0; i < tableSize; i++)
		for (int j = 0; j < 3; j++)
			data[i][j] = tcpList[i][j];

	if (firstTime == 1)
		firstTime = 0;

	mergeSortArrayLocal();
}
//sorts array
void TCPTable::mergeSortArrayLocal()
{
	int locationA = 0;
	int ipAddress = 0;
	string curSubstring;
	string *sortArrayLocal = new string[tableSize];
	string *sortArrayRemote = new string[tableSize];
	string *sortScratch = new string[tableSize * 3];
	string **dataScratch = new string*[tableSize * 3];
	for (int i = 0; i < tableSize * 3; i++)
		dataScratch[i] = new string[3];

	for (int i = 0; i < tableSize; i++){
		curSubstring = data[i][0];
		while (curSubstring.find(".") != string::npos){
			locationA = curSubstring.find(".");
			curSubstring.erase(locationA, 1);
		}
		locationA = curSubstring.find(":");
		curSubstring.erase(locationA, 1);
		sortArrayLocal[i] = curSubstring;
	}

	for (int i = 0; i < tableSize; i++){
		curSubstring = data[i][1];
		while (curSubstring.find(".") != string::npos){
			locationA = curSubstring.find(".");
			curSubstring.erase(locationA, 1);
		}
		locationA = curSubstring.find(":");
		curSubstring.erase(locationA, 1);
		sortArrayRemote[i] = curSubstring;
	}
	for (int i = 0; i < tableSize; i++){
		cout << sortArrayLocal[i] << " " << sortArrayRemote[i] << endl;
	}
	mergeHelper(sortArrayRemote, 0, tableSize, sortScratch, dataScratch);

	for (int i = 0; i < tableSize * 3; i++)
		delete dataScratch[i];
	delete []dataScratch;

}
void TCPTable::mergeHelper(string *input, int left, int right, string *scratch, string **dataScratch)
{
	/* base case: one element */
	if (right == left + 1)
	{
		return;
	}
	else
	{
		int i = 0;
		int length = right - left;
		int midpoint_distance = length / 2;
		/* l and r are to the positions in the left and right subarrays */
		int l = left, r = left + midpoint_distance;

		/* sort each subarray */
		mergeHelper(input, left, left + midpoint_distance, scratch, dataScratch);
		mergeHelper(input, left + midpoint_distance, right, scratch, dataScratch);

		/* merge the arrays together using scratch for temporary storage */
		for (i = 0; i < length; i++)
		{
			/* Check to see if any elements remain in the left array; if so,
			* we check if there are any elements left in the right array; if
			* so, we compare them.  Otherwise, we know that the merge must
			* use take the element from the left array */
			if (l < left + midpoint_distance &&
				(r == right || max(input[l], input[r]) == input[l]))
			{
				scratch[i] = input[l];
				dataScratch[i] = data[l];
				l++;
			}
			else
			{
				scratch[i] = input[r];
				dataScratch[i] = data[r];
				r++;
			}
		}
		/* Copy the sorted subarray back to the input */
		for (i = left; i < right; i++)
		{
			input[i] = scratch[i - left];
			data[i] = dataScratch[i - left];
		}
	}
}
//switch table direction
void TCPTable::flipData()
{
	string *temp;
	for (int i = 0; i < tableSize / 2; i++){
		temp = data[tableSize - i - 1];
		data[tableSize - i - 1] = data[i];
		data[i] = temp;
	}
}

//prints header informaton for debug purposes
void TCPTable::printHeaderInformation()
{
	/*for (int i = 0; i < 3; i++){
		for (int j = 0; j < 4; j++){
			cout << headerInformation[i][j] << " ";
		}
		cout << endl;
	}*/
}
//redraws the table
void TCPTable::redrawTable(TCPTable *curTable)
{
	printHeaderInformation();
	curTable->rows(tableSize);
	curTable->redraw();
}
//returns if permission to drawn is available
int TCPTable::getDrawState()
{
	return noDraw;
}
//returns size of data table
int TCPTable::getDataTableSize()
{
	return tableSize;
}
//returns the currently drawn data array
string **TCPTable::getDataTable()
{
	return data;
}
//returns the bounds for mouse events on the table
int **TCPTable::getHeaderSizeInformation()
{
	return headerInformation;
}
//returns the connection object
TcpTableAccess *TCPTable::getTcpObject()
{
	return tcpConnections;
}
//copies the data table
string **TCPTable::getTableCopy()
{
	noRefill = 1;
	copyTableSize = tableSize;
	string **listCopy = new string*[tableSize];
	for (int i = 0; i < tableSize; i++){
		listCopy[i] = new string[3];
	}
	for (int i = 0; i < tableSize; i++)
		for (int j = 0; j < 3; j++)
     		listCopy[i][j] = data[i][j];
	noRefill = 0;

	return listCopy;
}
//prevents the table from being refilled
void TCPTable::stopTableRefill()
{
	noRefill = 1;
}
//enables the table to be refilled
void TCPTable::startTableRefill()
{
	noRefill = 0;
	haveStatusList = 0;
}
//gets the size of any previous copied data table
int TCPTable::getCopyTableSize()
{
	return copyTableSize;
}
//returns status of the ipLookup for gui function
int TCPTable::getIpStatusLookup()
{
	return ipStatusLookup;
}
void TCPTable::setIpStatusLookup()
{
	ipStatusLookup = 0;
}
//runs the routines for checking for blips
void TCPTable::blacklistChecker()
{
	startBlipThread();
}
//enters the new context for the thread
void TCPTable::startBlipThread()
{
	_beginthread(TCPTable::enterBlipThread, 0, this);
}
//makes object calls to lookup ips
void TCPTable::threadBlipBody()
{
	getTableCopy();
	blipl->getBlacklistResult(getTableCopy(), getCopyTableSize());
	noRefill = 1;
	ipStatusList = blipl->getStatusList();
	ipStatusListSize = blipl->getStatusListSize();
	haveStatusList = 1;
	noRefill = 1;
	ipStatusLookup = 1;	
}
void TCPTable::enterBlipThread(void *p)
{
	((TCPTable *)p)->threadBlipBody();
	_endthread();
	return;
}
//destructor
TCPTable::~TCPTable() { }

#endif