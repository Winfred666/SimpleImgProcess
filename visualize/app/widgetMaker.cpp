
#include "widgetMaker.h"
#include <FL/fl_draw.H>
#include <FL/Enumerations.H>
#include <FL/Fl.H>

#define WIDGET_PLACE_SETTER ww*div.startX,wh*div.startY,ww*(div.endX-div.startX),wh*(div.endY-div.startY)
#define PROVIDE_COMPOCTX if(compoCtx==NULL) throw "cannot find gridcomponent context, fail to create widget!";\
	const Division &div = compoCtx->getSpace(size)



void myBox_draw(int x, int y, int w, int h, Fl_Color c) {
	fl_color(fl_rgb_color(20, 20, 20));
	fl_rectf(x, y, w, h);
}

WidgetMaker::WidgetMaker(int winW, int winH) :ww(winW), wh(winH), compoCtx(NULL) {
	color_BG = fl_rgb_color(25,25,25);
	color_Text = fl_rgb_color(186, 186, 186);
	color_Bar = fl_rgb_color(48, 48, 48);
	color_Block = fl_rgb_color(65, 65, 70);
	color_Press = fl_rgb_color(74,74,74);
	color_Hover = fl_rgb_color(85, 80, 85);
	//here is a display of change box style. draw freely on it.
	Fl::set_boxtype(FL_FREE_BOXTYPE, myBox_draw, 0, 0, 0, 0);
}


void WidgetMaker::globalMask() {
	Fl_Box *box = new Fl_Box(0, 0, ww, wh);
	box->color(color_BG);
	box->box(FL_FLAT_BOX);
}

Fl_Group &WidgetMaker::initGridGroup() {
	if (compoCtx == NULL) throw "initGridGroup: init grid group without context!!";
	Division div=compoCtx->getViewBox();
	const double *pad = compoCtx->getPadding();
	div.startX -= pad[0] / 2;
	div.endX += pad[0] / 2;
	div.startY -= pad[1] / 2;
	div.endY += pad[1] / 2;
	Fl_Group* group = new Fl_Group(WIDGET_PLACE_SETTER);
	addStyle(*group);
	group->color(color_Bar);
	return *group;
}

Fl_Group &WidgetMaker::makeGroup(double size) {
	PROVIDE_COMPOCTX;
	Fl_Group* group = new Fl_Group(WIDGET_PLACE_SETTER);
	addStyle(*group);
	return *group;
}

Fl_Box &WidgetMaker::makeBox(double size) {
	PROVIDE_COMPOCTX;
	Fl_Box* box = new Fl_Box(WIDGET_PLACE_SETTER);
	addStyle(*box);
	return *box;
}

Fl_Button &WidgetMaker::makeButton(double size) {
	PROVIDE_COMPOCTX;
	Fl_Button* button = new Fl_Button(WIDGET_PLACE_SETTER);
	addStyle(*button);
	button->down_color(color_Press);
	button->selection_color(color_Hover);
	button->box(FL_BORDER_BOX);
	button->down_box(FL_THIN_DOWN_BOX);
	return *button;
}

Fl_Input &WidgetMaker::makeInput(double size) {
	PROVIDE_COMPOCTX;
	Fl_Input* input = new Fl_Input(WIDGET_PLACE_SETTER);
	addStyle(*input);
	return *input;
}


ImageDisplay &WidgetMaker::makeImageDisplay() {
	if (compoCtx == NULL) throw "makeImageDisplay: init ImageDisplay without context!!";
	Division div = compoCtx->getViewBox();
	ImageDisplay* disp = new ImageDisplay(WIDGET_PLACE_SETTER);
	addStyle(*disp);
	return *disp;
}


void WidgetMaker::addStyle(Fl_Widget &box) {
	box.labelsize(20);
	box.labelcolor(color_Text);
	box.color(color_Block);
	box.box(FL_FLAT_BOX);
}