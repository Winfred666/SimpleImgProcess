#include "gridWindow.h"

#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Group.H>
#include <stack>


GridWindow* GridWindow::_instance = NULL;

GridWindow::GridWindow(int w, int h, const char *title) :_grid(),
	_window(w, h,title),
	maker(w, h) {}

//close the window
GridWindow::~GridWindow() {
GridWindow::_instance = NULL;
}

GridWindow &GridWindow::init(int w, int h, const char *title) {
	if (_instance != NULL) return *_instance;
	_instance = new GridWindow(w, h, title);
	return *_instance;
}

GridWindow &GridWindow::getInstance() {
	if (GridWindow::_instance == NULL) throw "GridWindow::getInstance: haven't inititialize GridWindow instance yet!";
	return *GridWindow::_instance;
}

Fl_Group &GridWindow::startGroup(ViewZone zone) {
	GridComponent &ret = _grid.getZone(zone);
	maker.setGridContext(&ret);
	Fl_Group &g = maker.initGridGroup();
	groupStack.push(&g);
	return g;
}

void GridWindow::endGroup() {
	if (groupStack.empty()) return;
	Fl_Group *group = groupStack.top();
	groupStack.pop();
	group->end();
}

//open and return the final window status
int GridWindow::run() {
	_window.end();
	_window.show();
	return Fl::run();
}
