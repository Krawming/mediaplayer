#include "videowidget.h"

static int flag = true;
videowidget::videowidget(QWidget *parent):QOpenGLWidget(parent)
{
	//string str = "E://tmp_movie//qqw_p5.flv";
	////string str = "E://C//video-h265.mkv";
	////Xffmpeg::Get()->loadvideoFile(str);
	bool result = Xffmpeg::Get()->initVideo("E://tmp_movie//qqw_p5.flv");  //初始化

	if (!result)
	{
		qDebug() << "init video error";
		return;
	}
	startTimer(30);  //设定定时器
	XVideoThread::Get()->start();   //启动线程 开始读视频、解码视频、控制播放速度
}


void videowidget::paintEvent(QPaintEvent * event)
{
	//绘制画面
	static QImage *image = NULL;
	static int w = 0;
	static int h = 0;
	if (w != width() || h != height())
	{
		if (image)
		{
			delete image->bits();
			delete image;
			image = NULL;
		}
	}

	if (image == NULL)
	{
		uchar* buf = new uchar[width()*height() * 4];  //解码后存放的空间
		image = new QImage(buf, width(), height(), QImage::Format_ARGB32);  //初始化image
	}

	//将得到的视频帧转换为RGB
	//AVPacket pkt = Xffmpeg::Get()->readpkt();  //读取视频
	//if (pkt.stream_index != Xffmpeg::Get()->videoStream_index)
	//{
	//	av_packet_unref(&pkt);
	//	return;
	//}
	//if (pkt.size == 0)
	//	return;
	//Xffmpeg::Get()->Decode(&pkt);Xffmpeg::Get()->videoFrame
	AVFrame *yuv = XVideoThread::Get()->yuv;
	//if(yuv != NULL)

	//AVFrame* yuv = XVideoThread::Get()->framelist.front();
	Xffmpeg::Get()->ToRGB(yuv, (char*)image->bits(), width(), height());
	//XVideoThread::Get()->framelist.pop_front();

	QPainter painter;
	painter.begin(this);
	painter.drawImage(QPoint(0, 0), *image);  //绘制ffmpeg解码的视频
	//if (image)
	//{
	//	delete image->bits();
	//	delete image;
	//	image = NULL;
	//}
	painter.end();
}

void videowidget::timerEvent(QTimerEvent * event)
{
	this->update();  //定时更新
}

videowidget::~videowidget()
{
}
