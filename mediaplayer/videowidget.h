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
	void paintEvent(QPaintEvent *event);   //�������»���
	void timerEvent(QTimerEvent* event);  //��ʱ��
	virtual ~videowidget();
};

