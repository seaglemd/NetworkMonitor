/*****************************************************************
*Sets up the table for the reverse dns lookups that take place   *
*and displays the information                                    *
*****************************************************************/
#ifndef REVERSEDNSCELLDISPLAY_H
#define REVERSEDNSCELLDISPLAY_H
#define WIN32_LEAN_AND_MEAN

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Tabs.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_PNG_Image.H>
#include <FL/Fl_Table.H>
#include <FL/fl_draw.H>


#include "reverseDnsLookup.h"

#define MAX_COLSR 1

using namespace std;
//Extending Fl_Table is how tables are made in FLTK
class RDNSTable : public Fl_Table
{
public:
	//inline constructor declartion is typcal for inherited classes in this way
	RDNSTable::RDNSTable(TCPTable *nCurTable, int X, int Y, int W, int H, const char *L = 0) : Fl_Table(X, Y, W, H, L){
		firstTime = 1;
		tableSize = 0;
		curTable = nCurTable; //current table reference for moving out of local scope
		rDNS = new ReverseDnsLookup();//creates a rdns object
		
		headings[0] = "Remote Host";

		rows(tableSize);
		row_header(0);              // enable row headers (along left)
		row_height_all(20);         // default height of rows
		row_resize(1);              // disable row resizing

		cols(MAX_COLSR);             // how many columns
		col_header(1);              // enable column headers (along top)
		col_width_all(450);          // default width of columns
		col_resize(1);              // enable column resizing
		end();                        // end the Fl_Table group
	};
	void RDNSTable::updateCells(); //will update the cells usually by filling the array
	void RDNSTable::redrawTable(RDNSTable *curTable); //tells the table to redraw
	int RDNSTable::getTableSize(); //returns the size of the table
	RDNSTable::~RDNSTable();
private:
	int firstTime; //checks to know if this is the first time all this is happening
	int tableSize; //size of table
	int tcpListSize; //list size of the tcp table
	int noDraw; //guards for not drawing during array fill (reading/writing at the same time is bad)
	string *data;  // data array for cells
	string headings[MAX_COLSR];
	string **tcpList;
	TCPTable *curTable;
	ReverseDnsLookup *rDNS;
	void RDNSTable::waitForData(); //used for calling the fill functions
	void RDNSTable::fillDataArray(); //fills the array so the data can be drawn
	//table header, inline as normal
	void RDNSTable::DrawHeader(const char *s, int X, int Y, int W, int H){
		fl_push_clip(X, Y, W, H);
		fl_draw_box(FL_THIN_UP_BOX, X, Y, W, H, row_header_color());
		fl_color(FL_BLACK);
		fl_draw(s, X, Y, W, H, FL_ALIGN_LEFT);
		fl_pop_clip();
	}
	//draws a cell's data, inline as normal
	void RDNSTable::DrawData(const char *s, int X, int Y, int W, int H) {
		fl_push_clip(X, Y, W, H);
		// Draw cell bg
		fl_color(FL_WHITE); fl_rectf(X, Y, W, H);
		// Draw cell data
		fl_color(FL_GRAY0); fl_draw(s, X, Y, W, H, FL_ALIGN_LEFT);
		// Draw box border
		fl_color(color()); fl_rect(X, Y, W, H);
		fl_pop_clip();
	}
	//for drawing the cells, inline as normal
	void RDNSTable::draw_cell(TableContext context, int ROW = 0, int COL = 0, int X = 0, int Y = 0, int W = 0, int H = 0){
		Fl::lock();
		static char s[40];
		switch (context) {
		case CONTEXT_STARTPAGE:                   // before page is drawn..
			fl_font(FL_HELVETICA, 16);              // set the font for our drawing operations
			return;
		case CONTEXT_COL_HEADER:
              // "A", "B", "C", etc.
			DrawHeader(headings[COL].c_str(), X, Y, W, H);
			return;
		case CONTEXT_ROW_HEADER:                  // Draw row headers
			//sprintf(s, "%03d:", ROW);                 // "001:", "002:", etc
			//DrawHeader(s, X, Y, W, H);
			return;
		case CONTEXT_CELL:
			if (ROW <= tableSize && noDraw == 0) //very important drawing when the array is being written to.
				DrawData(data[ROW].c_str(), X, Y, W, H);
			return;
		default:
			return;
		}

		Fl::unlock();
		return;
	}
};
//Update cells locks the array from being drawn
void RDNSTable::updateCells()
{
	noDraw = 1;
	waitForData();
	noDraw = 0;
}
//for filling the array with rdns data
void RDNSTable::waitForData()
{
	int haveData = 0;
	while (haveData == 0){
			fillDataArray();
			haveData = 1;
		
	}
}
//fills the array to be drawn with the dns server names
void RDNSTable::fillDataArray()
{
	tcpList = curTable->getTableCopy(); //gets an actual copy of the tcparray
	tcpListSize = curTable->getCopyTableSize(); //gets the size of the tcp list that was copied
	data = rDNS->getHostList(tcpList, tcpListSize); //sends this list and its size to the ReverseDNS object
	tableSize = rDNS->getTableSize(); //gets the size of the reverse dns array that was computed in previous function
}
//redraws the RDNS table
void RDNSTable::redrawTable(RDNSTable *curTable)
{ 
	int tempzero = 0;
	rows(tempzero); //to force a redraw from rows table size needs to change
	rows(tableSize);
}
//deconstructor
RDNSTable::~RDNSTable() { }

#endif