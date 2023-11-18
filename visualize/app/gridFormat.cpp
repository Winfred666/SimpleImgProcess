#include "gridFormat.h"


GridComponent::GridComponent(Division &zone,
	Display dis,
	double padX,
	double padY,
	double spacing) :_display(dis), _spacing(spacing), _pads{padX,padY} {

	_view.startX = zone.startX + padX;
	_view.endX = zone.endX - padX;
	_view.startY = zone.startY + padY;
	_view.endY = zone.endY - padY;

	isReversed = false;
	_curX = _curY = 0;
}

const Division& GridComponent::getViewBox() {
	return _view;
}

void GridComponent::setPadding(double padX, double padY) {
	_view.startX -= padX - _pads[0];
	_view.endX += padX - _pads[0];
	_view.startY -= padY - _pads[1];
	_view.endY += padY - _pads[1];
	_pads[0] = padX;
	_pads[1] = padY;
}
//copy a division and return.
Division GridComponent::getSpace(double stretch) {
	if (isFull()) throw "GridComponent::getSpace : the space is Full!!";

	Division ret;
	ret.startX = _view.startX + vw()*_curX;
	ret.startY = _view.startY + vh()*_curY;
	if (_display == COLUMN) {
		ret.endX = _view.endX;
		double addH = vw()*stretch;
		ret.endY = ret.startY + addH;
		_curY += addH + _spacing;
	}
	else if (_display == ROW) {
		ret.endY = _view.endY;
		double addW = vh()*stretch;
		ret.endX = ret.startX + addW;
		_curX += addW + _spacing;
	}
	return ret;
}

int GridComponent::checkSquareRemain() {
	if (_display == COLUMN) {
		return (int)(vh()*(1 - _curY) / vw());
	}
	else if (_display == ROW) {
		return (int)(vw()*(1 - _curX) / vh());
	}
	else return 0;
}

bool GridComponent::isFull() {
	return checkSquareRemain() < 1;
}


//initialize a traditional 2*3 grid 
GridFormat::GridFormat(double r, double c1, double c2) {
	_rows = 3;
	_cols = 2;
	
	Division top = { 0,0,1,r }, left = { 0,r,c1,1 }, main = { c1,r,c2,1 }, right = { c2,r,1,1 };
	_grid[TopTab] = new GridComponent(top,ROW);
	_grid[MainView] = new GridComponent(main);
	_grid[LeftTab] = new GridComponent(left);
	_grid[RightTab] = new GridComponent(right);
}

GridComponent& GridFormat::getZone(ViewZone shortCut) {
	if (_grid.find(shortCut) == _grid.end()) throw "Can't find Grid Component!";
	return *_grid[shortCut];
}

GridFormat::~GridFormat() {
	for (auto it : _grid) {
		delete it.second;
	}
}