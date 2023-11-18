#include <iostream>

#include "gridWindow.h"
#include <FL/Fl.H>
#include "image.h"
#include "readBMP.h"

void makeTop(GridWindow &window,WidgetMaker &maker) {
	window.startGroup(TopTab);
	window.endGroup();
}


void makeLeft(GridWindow &window, WidgetMaker &maker) {
	window.startGroup(LeftTab);
	Fl_Button &but = maker.makeButton();
	//remember the label string must be global, because we need to display it all the time.
	but.label("fdsa");
	window.endGroup();
}
void makeRight(GridWindow &window, WidgetMaker &maker) {
	window.startGroup(RightTab);

	window.endGroup();
}

void makeMain(GridWindow &window, WidgetMaker &maker) {
	Fl_Group &mainG = window.startGroup(MainView);
	mainG.color(maker.color_BG);
	ImageDisplay &imgDis=maker.makeImageDisplay();
	//try to open default image

	FILE *fs=fopen_s("test.bmp", "rb");
	imgDis.setImage(readBMP(fs));
	fclose(fs);
	window.endGroup();
}

int main (int argc, char *argv[]) {
	GridWindow &window= GridWindow::init(800, 600, "Simple Image Process");
	WidgetMaker &maker = window.maker;
	try {
		maker.globalMask();
		makeTop(window, maker);
		makeRight(window, maker);
		makeLeft(window, maker);
		makeMain(window, maker);
	}

	catch (char const *e) {
		
		fprintf(stderr,"catch Exception: %s\n", e);
	}
	catch (string e) {
		fprintf(stderr,"catch Exception with info: %s\n", e.c_str());
	}
	catch (...) {
		fprintf(stderr,"Unexpected error occur!");
	}

	return window.run();
}