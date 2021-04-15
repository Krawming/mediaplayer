#include "XSlider.h"


XSlider::XSlider(QWidget* p):QSlider(p)
{
}

XSlider::~XSlider()
{
}

void XSlider::mousePressEvent(QMouseEvent* event)
{
	double pos = (double)event->pos().x() / (double)width();
	setValue(pos*this->maximum());
	QSlider::mousePressEvent(event);
}
