#pragma once
#include <qslider.h>
#include <qobject.h>
#include <QMouseEvent>

class XSlider: public QSlider
{
	Q_OBJECT
public:
	XSlider(QWidget* p);
	~XSlider();
	void mousePressEvent(QMouseEvent* event);
};

