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
	static Xffmpeg* Get()   //单例，封装一个ffmpeg对象
	{
		static Xffmpeg ff;
		return &ff;
	}

	virtual ~Xffmpeg();
	void loadvideoFile(const char* file_path);
	bool initVideo(const char* file_path);
	void free();   //释放相关申请空间
	//void play();    //播放视频
	AVPacket readpkt();    //读取帧数据
	//AVFrame* Decode(const AVPacket *pkt);   //读取帧数据并进行解码
	int Decode(const AVPacket *pkt1);   //读取帧数据并进行解码
	int GetPts(const AVPacket *pkt);   //获取该包pts
	bool ToRGB(const AVFrame* yuv, char *out, int outwidth, int outheight); //yuv格式转rgb格式
	int ToPCM(char* out);
	static double r2d(AVRational r)
	{
		return r.den == 0 ? 0 : (double)r.num / (double)r.den;
	}
	bool Seek(float pos);  //进度条拖动
	void Close();

	//argv
	int64_t pts;                //播放进度
	AVFrame* videoFrame;    //视频帧
	AVFrame* avFrame2;
	
	//播放、暂停标志
	bool isPlay=true;    //播放视频标志
	//////////////////////////////////////////////////
	//音频
	int sampleRate = 48000;   //样本率
	int sampleSize = 16;	  //样本大小
	int channel = 2;		  //通道数

protected:
	AVFormatContext *avFormatContext;   //格式文本
	int ret;
	AVCodecContext* videoCodec;   //视频解码
	AVCodec* videoDecoder;    //视频解码器
	AVCodecContext* audioCodec;   //音频解码
	AVCodec* audioDecoder;    //音频解码器
	int videoWidth;         //视频宽度
	int videoHeight;        //视频高度
	int oldWidth;          //上一帧宽度
	int oldHeight;         //上一帧高
	AVPacket* avPacket;    //包文件

	AVFrame* avFrame3;
	uint8_t *buffer;       //保存帧数据
	SwsContext *swsContext;  //图像数据RGB
	SwrContext *aCtx;       //音频数据
	int64_t start_time;   //开始时间
	
	
	
	Xffmpeg();
	QMutex mutex;    //互斥量, 用于开多线程，读packet时

public:
	const char* video_path;
	int totalMs;          //总时间
	int videoStream_index;   //视频流索引号
	int audioStream_index;   //音频流索引号

};

