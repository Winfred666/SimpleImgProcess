#ifndef  GRIDWINDOW_H
#define GRIDWINDOW_H

#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Group.H>
#include <stack>

#include "gridFormat.h"
#include "widgetMaker.h"

class GridWindow
{
private:
	Fl_Double_Window _window;
	static GridWindow *_instance;
	stack< Fl_Group *> groupStack;
	GridFormat _grid;
	//create a window.
	GridWindow(int w, int h, const char *title);
	//close the window
	~GridWindow();
public:
	WidgetMaker maker;
	static GridWindow & init(int w, int h,const char *title);
	
	static GridWindow &getInstance();

	Fl_Group & startGroup(ViewZone zone);

	void endGroup();

	//open and return the final window status
	int run();
};


#endif // ! GRIDWINDOW_H
