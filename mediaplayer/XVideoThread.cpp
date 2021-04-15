#include "XVideoThread.h"

static list<AVPacket>  videos;  //存放解码前的视频帧
bool isexit = false;  //线程未退出
static int apts = -1;  //音频pts
float idx = 0;
bool flag = false;
void XVideoThread::run()  //开启线程，读取packet-> 解码pkt -> 得到avframe
{
	char out[10000] = { 0 };
	while (!isexit)  //线程未退出
	{
		//idx += 0.01;
		//qDebug() << " thread is running" << idx;
		if (!Xffmpeg::Get()->isPlay)
		{
			qDebug() << " not is play, sleep 10ms";
			msleep(10);
			continue;
		}

		while (videos.size() > 0)
		{
			//qDebug() << " video size: " << videos.size();
			AVPacket pack = videos.front();
			//int pts = Xffmpeg::Get()->GetPts(&pack);
			//qDebug() << " video pts: " << pts << "  audio pts: " << apts;
			Xffmpeg::Get()->Decode(&pack);
			yuv = Xffmpeg::Get()->videoFrame;
			//framelist.push_back(yuv);
			av_packet_unref(&pack);  //清除packet
			videos.pop_front();
			msleep(20);
		}


		int free = XAudioPlay::Get()->GetFree();  //此时缓冲区的空间大小
		if (free < 10000)
		{
			msleep(1);
			continue;
		}

		AVPacket pkt1 = Xffmpeg::Get()->readpkt();
		if (pkt1.size <= 0)
		{
			msleep(10);
			continue;
		}

		//音频加载
		if (pkt1.stream_index == Xffmpeg::Get()->audioStream_index)
		{
			//先判断视频流序列号是否相同
			apts = Xffmpeg::Get()->Decode(&pkt1);
			av_packet_unref(&pkt1); //释放pkt
			int len = Xffmpeg::Get()->ToPCM(out);  //重采样音频
			XAudioPlay::Get()->Write(out, len);  //写入音频
			continue;
		}
		videos.push_back(pkt1);
	}
}

XVideoThread::XVideoThread()
{
}

XVideoThread::~XVideoThread()
{

}
