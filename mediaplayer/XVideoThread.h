#pragma once
#include <qthread.h>
#include "ffmpeg.h"
#include "XAudioPlay.h"
#include <list>

class XVideoThread:public QThread
{
public:
	static XVideoThread *Get()  //��������ģʽ
	{
		static XVideoThread xvt;
		return &xvt;
	}
	void run();  //�߳�����
	XVideoThread();
	virtual ~XVideoThread();

public:
	AVFrame* yuv;
	bool isexit;
};

