#pragma once
#include <qthread.h>
#include "ffmpeg.h"
#include "XAudioPlay.h"
#include <list>

class XVideoThread:public QThread
{
public:
	static XVideoThread *Get()  //创建单例模式
	{
		static XVideoThread xvt;
		return &xvt;
	}
	void run();  //线程运行
	XVideoThread();
	virtual ~XVideoThread();

public:
	AVFrame* yuv;
	bool isexit;
};

