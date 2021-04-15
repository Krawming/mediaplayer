#pragma once
#include "ffmpeghead.h"
#include <qdebug.h>
#include <qtimer.h>
#include <qdatetime.h>
#include <qimage.h>
//#include <synchapi.h>
#include <qmutex.h>

#include <string>

//#define TIMEMS      qPrintable(QTime::currentTime().toString("HH:mm:ss zzz"))
//#define STRDATETIME qPrintable(QDateTime::currentDateTime().toString("yyyy-MM-dd-HH-mm-ss"))

using namespace std;


class Xffmpeg
{
public:
	static Xffmpeg* Get()   //��������װһ��ffmpeg����
	{
		static Xffmpeg ff;
		return &ff;
	}

	virtual ~Xffmpeg();
	void loadvideoFile(const char* file_path);
	bool initVideo(const char* file_path);
	void free();   //�ͷ��������ռ�
	//void play();    //������Ƶ
	AVPacket readpkt();    //��ȡ֡����
	//AVFrame* Decode(const AVPacket *pkt);   //��ȡ֡���ݲ����н���
	int Decode(const AVPacket *pkt1);   //��ȡ֡���ݲ����н���
	int GetPts(const AVPacket *pkt);   //��ȡ�ð�pts
	bool ToRGB(const AVFrame* yuv, char *out, int outwidth, int outheight); //yuv��ʽתrgb��ʽ
	int ToPCM(char* out);
	static double r2d(AVRational r)
	{
		return r.den == 0 ? 0 : (double)r.num / (double)r.den;
	}
	bool Seek(float pos);  //�������϶�
	void Close();

	//argv
	int64_t pts;                //���Ž���
	AVFrame* videoFrame;    //��Ƶ֡
	AVFrame* avFrame2;
	
	//���š���ͣ��־
	bool isPlay=true;    //������Ƶ��־
	//////////////////////////////////////////////////
	//��Ƶ
	int sampleRate = 48000;   //������
	int sampleSize = 16;	  //������С
	int channel = 2;		  //ͨ����

protected:
	AVFormatContext *avFormatContext;   //��ʽ�ı�
	int ret;
	AVCodecContext* videoCodec;   //��Ƶ����
	AVCodec* videoDecoder;    //��Ƶ������
	AVCodecContext* audioCodec;   //��Ƶ����
	AVCodec* audioDecoder;    //��Ƶ������
	int videoWidth;         //��Ƶ���
	int videoHeight;        //��Ƶ�߶�
	int oldWidth;          //��һ֡���
	int oldHeight;         //��һ֡��
	AVPacket* avPacket;    //���ļ�

	AVFrame* avFrame3;
	uint8_t *buffer;       //����֡����
	SwsContext *swsContext;  //ͼ������RGB
	SwrContext *aCtx;       //��Ƶ����
	int64_t start_time;   //��ʼʱ��
	
	
	
	Xffmpeg();
	QMutex mutex;    //������, ���ڿ����̣߳���packetʱ

public:
	const char* video_path;
	int totalMs;          //��ʱ��
	int videoStream_index;   //��Ƶ��������
	int audioStream_index;   //��Ƶ��������

};

