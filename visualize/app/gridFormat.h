#ifndef GRIDFORMAT_H
#define GRIDFORMAT_H

#include <map>
using namespace std;

//unit: pixel.

typedef struct {
	double startX;
	double startY;
	double endX;
	double endY;
} Division;

enum Display{
	ROW,COLUMN,WRAPPED
};


//when zone is traditional 2*3, there is shortcut to specify parts of windows.

class GridComponent{
protected:
	Display _display;
	double _pads[2];

	//current available percentage
	double _curX;
	double _curY;

	//relative to mother spacing, where _view cut the padding.
	Division _view;
	double _spacing;

	bool isReversed;
public:

	GridComponent(Division &zone,
		Display dis = COLUMN,
		double padX = 0.008,
		double padY = 0.008,
		double spacing = 0.02);

	inline double vw() {
		return _view.endX - _view.startX;
	}

	inline double vh() {
		return _view.endY - _view.startY;
	}

	inline const double *getPadding() {
		return _pads;
	}
	const Division& getViewBox();
	//better set it when no space has been taken.
	void setPadding(double padX, double padY);

	//copy a division and return.
	Division getSpace(double stretch = 1);
	int checkSquareRemain();

	bool isFull();

};

enum ViewZone
{
	TopTab, MainView, RightTab, LeftTab
};

class GridFormat{
private:
	//only have (0 to _xDiv-1) * (0 to _yDiv-1) parts
	int _rows;
	int _cols;
	//a division-> GridComponent
	map<int,GridComponent *> _grid;
public:
	//initialize a traditional 2*3 grid 
	GridFormat(double r = 0.06, double c1 = 0.08, double c2 = 0.8);

	GridComponent& getZone(ViewZone shortCut);
	
	~GridFormat();
};

#endif