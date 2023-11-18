#ifndef WIDGETMAKER_H
#define WIDGETMAKER_H

#include <FL/Fl.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Input.H>

#include "gridFormat.h"
#include "imageDisplay.h"

class WidgetMaker {
private:
	//w and h of window.
	int ww;
	int wh;
	//current component, serve as a context when create any widgets.
	GridComponent *compoCtx;

public:
	Fl_Color color_BG;
	Fl_Color color_Text;
	Fl_Color color_Bar;
	Fl_Color color_Press;
	Fl_Color color_Hover;
	Fl_Color color_Block;

	WidgetMaker(int winW, int winH);

	void globalMask();
	
	inline const Division &getGridDivision() {
		if (compoCtx == NULL) throw "try to get Grid division without context!";
		return compoCtx->getViewBox();
	}
	
	inline void setGridContext(GridComponent *gc) {
		compoCtx = gc;
	}
	inline void releaseGridContext() {
		compoCtx = NULL;
	}

	Fl_Group &initGridGroup();

	Fl_Group &makeGroup(double size = 1);

	Fl_Box &makeBox(double size = 1);

	Fl_Button &makeButton(double size = 1);

	Fl_Input &makeInput(double size = 0.5);
	
	ImageDisplay &makeImageDisplay();
	void WidgetMaker::addStyle(Fl_Widget &widget);

};



#endif // !WIDGETMAKER_H
