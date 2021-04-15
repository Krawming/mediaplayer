#include "videowidget.h"

static int flag = true;
videowidget::videowidget(QWidget *parent):QOpenGLWidget(parent)
{
	//string str = "E://tmp_movie//qqw_p5.flv";
	////string str = "E://C//video-h265.mkv";
	////Xffmpeg::Get()->loadvideoFile(str);
	bool result = Xffmpeg::Get()->initVideo("E://tmp_movie//qqw_p5.flv");  //��ʼ��

	if (!result)
	{
		qDebug() << "init video error";
		return;
	}
	startTimer(30);  //�趨��ʱ��
	XVideoThread::Get()->start();   //�����߳� ��ʼ����Ƶ��������Ƶ�����Ʋ����ٶ�
}


void videowidget::paintEvent(QPaintEvent * event)
{
	//���ƻ���
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
		uchar* buf = new uchar[width()*height() * 4];  //������ŵĿռ�
		image = new QImage(buf, width(), height(), QImage::Format_ARGB32);  //��ʼ��image
	}

	//���õ�����Ƶ֡ת��ΪRGB
	//AVPacket pkt = Xffmpeg::Get()->readpkt();  //��ȡ��Ƶ
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
	painter.drawImage(QPoint(0, 0), *image);  //����ffmpeg�������Ƶ
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
	this->update();  //��ʱ����
}

videowidget::~videowidget()
{
}
