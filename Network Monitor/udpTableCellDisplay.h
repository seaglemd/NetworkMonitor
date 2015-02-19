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
#include <FL/Fl_Table.H>
#include <FL/fl_draw.H>
#include "udpTableAccess.h"
//#include "stringDynamicArrayTest.h"

#define MAX_ROWS 10
#define MAX_COLS 2                // A-Z

// Derive a class from Fl_Table
using namespace std;

class UDPTable : public Fl_Table
{

public:
	//inherits from Fl_Table as is normal, inline as is standard
	UDPTable::UDPTable(int X, int Y, int W, int H, const char *L = 0) : Fl_Table(X, Y, W, H, L) {
		firstTime = 1;
		tableSize = 0;
		udpConnections = new UdpTableAccess();
		udpList = udpConnections->passUdpTable();
		tableSize = udpConnections->getTableSize();
		fillDataArray();

		headings[0] = "Local IP";
		headings[1] = "Port";

		// Rows
		rows(tableSize);             // how many rows
		row_header(0);              // enable row headers (along left)
		row_height_all(20);         // default height of rows
		row_resize(0);              // disable row resizing
		// Cols
		cols(MAX_COLS);             // how many columns
		col_header(1);              // enable column headers (along top)
		col_width_all(165);          // default width of columns
		col_resize(1);              // enable column resizing
		end();                        // end the Fl_Table group
	};

	void UDPTable::updateCells();
	UdpTableAccess *UDPTable::getUdpObject();
	void UDPTable::redrawTable(UDPTable *curTable);
	UDPTable::~UDPTable();
private:
	int firstTime;
	int tableSize;
	int noDraw;
	string **data;
	string headings[MAX_COLS];
	string **udpList;
	UdpTableAccess *udpConnections;
	void UDPTable::fillDataArray();
	//draws the headers, inline as is standard
	void UDPTable::DrawHeader(const char *s, int X, int Y, int W, int H) {
		fl_push_clip(X, Y, W, H);
		fl_draw_box(FL_THIN_UP_BOX, X, Y, W, H, row_header_color());
		fl_color(FL_BLACK);
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
		fl_color(color()); fl_rect(X, Y, W, H);
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
		for (int i = 0; i < tableSize; i++){
			delete[]data[i];
		}
		delete[] data;
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
	}
	else{
		data = new string*[tableSize];
		for (int i = 0; i < tableSize; i++){
			data[i] = new string[3];
		}

		for (int i = 0; i < tableSize; i++)
			for (int j = 0; j < 2; j++)
				data[i][j] = " ";

		for (int i = 0; i < tableSize; i++)
			for (int j = 0; j < 2; j++)
				data[i][j] = udpList[i][j];
		firstTime = 0;
	}
}
//redraws the udp table
void UDPTable::redrawTable(UDPTable *curTable)
{
	curTable->rows(tableSize);
	curTable->redraw();
}
//returns the udp object used to gather udp information
UdpTableAccess *UDPTable::getUdpObject()
{
	return udpConnections;
}
//destructor
UDPTable::~UDPTable() { }

#endif