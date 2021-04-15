#include "XVideoThread.h"

static list<AVPacket>  videos;  //��Ž���ǰ����Ƶ֡
bool isexit = false;  //�߳�δ�˳�
static int apts = -1;  //��Ƶpts
float idx = 0;
bool flag = false;
void XVideoThread::run()  //�����̣߳���ȡpacket-> ����pkt -> �õ�avframe
{
	char out[10000] = { 0 };
	while (!isexit)  //�߳�δ�˳�
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
			av_packet_unref(&pack);  //���packet
			videos.pop_front();
			msleep(20);
		}


		int free = XAudioPlay::Get()->GetFree();  //��ʱ�������Ŀռ��С
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

		//��Ƶ����
		if (pkt1.stream_index == Xffmpeg::Get()->audioStream_index)
		{
			//���ж���Ƶ�����к��Ƿ���ͬ
			apts = Xffmpeg::Get()->Decode(&pkt1);
			av_packet_unref(&pkt1); //�ͷ�pkt
			int len = Xffmpeg::Get()->ToPCM(out);  //�ز�����Ƶ
			XAudioPlay::Get()->Write(out, len);  //д����Ƶ
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
