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

#define MAX_ROWST 10
#define MAX_COLST 3

using namespace std;

class TCPTable : public Fl_Table 
{

public:	
	TCPTable::TCPTable(int X, int Y, int W, int H, const char *L = 0) : Fl_Table(X, Y, W, H, L){
		firstTime = 1;
		tableSize = 0;
		tcpConnections = new TcpTableAccess();
		tcpList = tcpConnections->passTcpTable();
		tableSize = tcpConnections->getTableSize();
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
	void TCPTable::updateCells();
	TcpTableAccess *TCPTable::getTcpObject();
	void TCPTable::redrawTable(TCPTable *curTable);
	TCPTable::~TCPTable();
private:
	int firstTime;
	int tableSize;
	string **data;  // data array for cells
	string headings[MAX_COLST];
	string **tcpList;
	TcpTableAccess *tcpConnections;
	void TCPTable::fillDataArray();

	void TCPTable::DrawHeader(const char *s, int X, int Y, int W, int H){
		fl_push_clip(X, Y, W, H);
		fl_draw_box(FL_THIN_UP_BOX, X, Y, W, H, row_header_color());
		fl_color(FL_BLACK);
		fl_draw(s, X, Y, W, H, FL_ALIGN_CENTER);
		fl_pop_clip();
	}
	void TCPTable::DrawData(const char *s, int X, int Y, int W, int H) {
		fl_push_clip(X, Y, W, H);
		// Draw cell bg
		fl_color(FL_WHITE); fl_rectf(X, Y, W, H);
		// Draw cell data
		fl_color(FL_GRAY0); fl_draw(s, X, Y, W, H, FL_ALIGN_CENTER);
		// Draw box border
		fl_color(color()); fl_rect(X, Y, W, H);
		fl_pop_clip();
	}
	void TCPTable::draw_cell(TableContext context, int ROW = 0, int COL = 0, int X = 0, int Y = 0, int W = 0, int H = 0){
		Fl::lock();
		static char s[40];
		switch (context) {
		case CONTEXT_STARTPAGE:                   // before page is drawn..
			fl_font(FL_HELVETICA, 16);              // set the font for our drawing operations
			return;
		case CONTEXT_COL_HEADER:
			sprintf(s, &(headings[COL])[0]);               // "A", "B", "C", etc.
			DrawHeader(s, X, Y, W, H);
			return;
		case CONTEXT_ROW_HEADER:                  // Draw row headers
			sprintf(s, "%03d:", ROW);                 // "001:", "002:", etc
			DrawHeader(s, X, Y, W, H);
			return;
		case CONTEXT_CELL:
			if (ROW <= tableSize)
				DrawData(data[ROW][COL].c_str(), X, Y, W, H);
			return;
		default:
			return;
		}

		Fl::unlock();
		return;
	}
	
	
};

void TCPTable::updateCells()
{
	fillDataArray();
}

void TCPTable::fillDataArray()
{

	if (firstTime != 1){
		for (int i = 0; i < tableSize; i++){
			delete[]data[i];
		}
		delete[] data;
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
	}
	else{
		data = new string*[tableSize];
		for (int i = 0; i < tableSize; i++){
			data[i] = new string[3];
		}

		for (int i = 0; i < tableSize; i++)
			for (int j = 0; j < 3; j++)
				data[i][j] = " ";

		for (int i = 0; i < tableSize; i++)
			for (int j = 0; j < 3; j++)
				data[i][j] = tcpList[i][j];
		firstTime = 0;
	}
}

void TCPTable::redrawTable(TCPTable *curTable)
{
	curTable->rows(tableSize);
	curTable->redraw();
}

TcpTableAccess *TCPTable::getTcpObject()
{
	return tcpConnections;
}
TCPTable::~TCPTable() { }

#endif