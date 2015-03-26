/*******************************************************************
*This class inherits Fl_Table to draw the data retrieved from      *
*the udp table. Start and stop functionality with threading is     *
*implemented. Overall basic compared to TCP so far                 *
*******************************************************************/
#ifndef UDPTABLECELLDISPLAY_H
#define UDPTABLECELLDISPLAY_H

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Tabs.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_PNG_Image.H>
#include <FL/Fl_Table_Row.H>
#include <FL/fl_draw.H>
#include "udpTableAccess.h"
//#include "stringDynamicArrayTest.h"

#define MAX_ROWS 10
#define MAX_COLS 2                // A-Z

// Derive a class from Fl_Table
using namespace std;

class UDPTable : public Fl_Table_Row
{

public:
	//inherits from Fl_Table as is normal, inline as is standard
	UDPTable::UDPTable(int X, int Y, int W, int H, const char *L = 0) : Fl_Table_Row(X, Y, W, H, L) {
		this->color(FL_WHITE);
		this->vscrollbar->slider(FL_BORDER_BOX);
		this->vscrollbar->color(FL_GRAY, FL_WHITE);
		this->vscrollbar->labelcolor(FL_BLACK);
		this->vscrollbar->labelfont(FL_BOLD);
		firstTime = 1;
		tableSize = 0;
		colToSort = 0;
		noRefill = 0;
		oppositeDir = 1;
		udpConnections = new UdpTableAccess();
		udpList = udpConnections->passUdpTable();
		tableSize = udpConnections->getTableSize();
		initializeHeaderInformation();
		fillDataArray();

		headings[0] = "Local IP";
		headings[1] = "Port";
		type(SELECT_SINGLE);
		// Rows
		rows(tableSize);             // how many rows
		row_header(0);              // enable row headers (along left)
		row_height_all(20);         // default height of rows
		row_resize(0);              // disable row resizing
		// Cols
		cols(MAX_COLS);             // how many columns
		col_header(1);              // enable column headers (along top)
		col_header_color(FL_DARK_BLUE);
		col_width_all(165);          // default width of columns
		col_resize(1);              // enable column resizing
		end();                        // end the Fl_Table group
	};

	void UDPTable::updateCells();
	UdpTableAccess *UDPTable::getUdpObject();
	void UDPTable::redrawTable(UDPTable *curTable);
	int **UDPTable::getHeaderSizeInformation();
	void UDPTable::flipData(int colClicked);
	UDPTable::~UDPTable();
private:
	int firstTime;
	int tableSize;
	int noDraw;
	int noRefill;
	int **headerInformation;
	int colToSort;
	int oppositeDir;
	string **data;
	string headings[MAX_COLS];
	string **udpList;
	UdpTableAccess *udpConnections;
	void UDPTable::prepMergeSort();
	void UDPTable::mergeHelper(string *input, int left, int right, string *scratch, string **dataScratch);
	void UDPTable::initializeHeaderInformation();
	void UDPTable::fillDataArray();
	//draws the headers, inline as is standard
	void UDPTable::DrawHeader(const char *s, int X, int Y, int W, int H) {
		fl_push_clip(X, Y, W, H);
		if (noRefill != 1)
			fl_draw_box(FL_BORDER_BOX, X, Y, W, H, col_header_color());
		else if (noRefill == 1)
			fl_draw_box(FL_BORDER_BOX, X, Y, W, H, col_header_color());
		fl_color(FL_WHITE);
		fl_draw(s, X, Y, W, H, FL_ALIGN_CENTER);
		fl_pop_clip();
	}
	//draws the current cell, inline as is standard
	void UDPTable::DrawData(const char *s, int X, int Y, int W, int H) {
		fl_push_clip(X, Y, W, H);
		// Draw cell bg
		fl_color(FL_WHITE); fl_rectf(X, Y, W, H);
		// Draw cell data
		fl_color(FL_GRAY0); fl_draw(s, X, Y, W, H, FL_ALIGN_CENTER);
		// Draw box border
		fl_color(FL_BLACK); fl_rect(X, Y, W, H);
		fl_pop_clip();
	}
	//draws the cells, inline as is normal
	void UDPTable::draw_cell(TableContext context, int ROW = 0, int COL = 0, int X = 0, int Y = 0, int W = 0, int H = 0) {
		Fl::lock();
		static char s[40];
		switch (context) {
		case CONTEXT_STARTPAGE:                   // before page is drawn..
			fl_font(FL_HELVETICA, 16);              // set the font for our drawing operations
			return;
		case CONTEXT_COL_HEADER:              // "A", "B", "C", etc.
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
			if (ROW <= tableSize && noDraw == 0) //protection from read/writing at same time
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
void UDPTable::initializeHeaderInformation()
{
	headerInformation = new int*[2];
	for (int i = 0; i < 2; i++){
		headerInformation[i] = new int[4];
		headerInformation[i][0] = 0;
		headerInformation[i][1] = 0;
		headerInformation[i][2] = 0;
		headerInformation[i][3] = 0;
	}
}
//fills the data array with new data and protects from read/writes as the same time
void UDPTable::updateCells()
{
	noDraw = 1;
		fillDataArray();
	noDraw = 0;
}
//fills the data array with any new data that has come in
void UDPTable::fillDataArray()
{
	if (firstTime != 1){
		for (int i = 0; i < tableSize; i++)
			delete[]data[i];
		delete[] data;
	}
	
	udpList = udpConnections->passUdpTable();
	tableSize = udpConnections->getTableSize();
	
	data = new string*[tableSize];
	for (int i = 0; i < tableSize; i++)
		data[i] = new string[2];

	for (int i = 0; i < tableSize; i++)
		for (int j = 0; j < 2; j++)
			data[i][j] = " ";

	for (int i = 0; i < tableSize; i++)
		for (int j = 0; j < 2; j++)
			data[i][j] = udpList[i][j];
	
	if (firstTime == 1)
		firstTime = 0;
}
//sorts array
void UDPTable::prepMergeSort()
{
	int locationA = 0;
	int ipAddress = 0;
	string curSubstring;
	string *sortArrayLocal = new string[tableSize];
	string *sortArrayRemote = new string[tableSize];
	string *sortArrayStatus = new string[tableSize];

	string *sortScratch = new string[tableSize * 2];
	string **dataScratch = new string*[tableSize * 2];
	for (int i = 0; i < tableSize * 2; i++)
		dataScratch[i] = new string[2];

	if (colToSort == 1){
		for (int i = 0; i < tableSize; i++){
			curSubstring = data[i][0];
			while (curSubstring.find(".") != string::npos){
				locationA = curSubstring.find(".");
				curSubstring.erase(locationA, 1);
			}
			sortArrayLocal[i] = curSubstring;
		}
		mergeHelper(sortArrayLocal, 0, tableSize, sortScratch, dataScratch);
		colToSort = 0;
	}
	if (colToSort == 2){
		for (int i = 0; i < tableSize; i++){
			curSubstring = data[i][1];
			sortArrayRemote[i] = curSubstring;
		}
		mergeHelper(sortArrayRemote, 0, tableSize, sortScratch, dataScratch);
		colToSort = 0;
	}
	//issues with deleting dataScratch at this point, investigate.
	//Literally worked yesterday and doesn't work today, no other changes (3/2/2015)
	
	redrawTable(this);
}
void UDPTable::mergeHelper(string *input, int left, int right, string *scratch, string **dataScratch)
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
void UDPTable::flipData(int colClicked)
{
	noDraw = 1;
	colToSort = colClicked;
	prepMergeSort();
	cout << " in u flip " << endl;
	if (oppositeDir == 1){
		cout << " chere " << endl;
		string *temp;
		for (int i = 0; i < tableSize / 2; i++){
			temp = data[tableSize - i - 1];
			data[tableSize - i - 1] = data[i];
			data[i] = temp;
		}
		
		oppositeDir = 0;
		for (int i = 0; i < tableSize; i++){
			for (int j = 0; j < 2; j++){
				cout << data[i][j];
			}
			cout << endl;
		}
	}
	else if (oppositeDir == 0){ //this is an else if specifically to continue logic from last if
		oppositeDir = 1;
	}
	noDraw = 0;
	//updateCells();
	redrawTable(this);
}
//redraws the udp table
void UDPTable::redrawTable(UDPTable *curTable)
{
	curTable->rows(tableSize);
	curTable->redraw();
}
//returns the bounds for mouse events on the table
int **UDPTable::getHeaderSizeInformation()
{
	return headerInformation;
}
//returns the udp object used to gather udp information
UdpTableAccess *UDPTable::getUdpObject()
{
	return udpConnections;
}
//destructor
UDPTable::~UDPTable() { }

#endif