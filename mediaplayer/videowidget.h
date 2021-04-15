#pragma once
#include "ffmpeghead.h"

#include <qopenglwidget.h>
#include <qpainter.h>
#include <string>
#include "ffmpeg.h"
#include "XVideoThread.h"

using namespace std;


class videowidget:public QOpenGLWidget
{
	Q_OBJECT
public:
	videowidget(QWidget *parent = 0);
	void paintEvent(QPaintEvent *event);   //窗口重新绘制
	void timerEvent(QTimerEvent* event);  //定时器
	virtual ~videowidget();
};

