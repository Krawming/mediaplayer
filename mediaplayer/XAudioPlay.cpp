#include "XAudioPlay.h"
#include <qaudiooutput.h>
#include <qmutex.h>

class CXAudioPlay :public XAudioPlay
{
public:
	QAudioOutput *output = NULL;
	QIODevice *io = NULL;
	QMutex mutex;   //������
	void Stop()
	{
		mutex.lock();
		if (output)
		{
			output->stop();
			delete output;
			output = NULL;
			io = NULL;
		}
		mutex.unlock();
	}

	bool Start()
	{
		Stop();
		mutex.lock();
		//QAudioOutput *out;   //������Ƶ
		QAudioFormat fmt;
		fmt.setSampleRate(48000);
		fmt.setSampleSize(16);
		fmt.setChannelCount(2);
		fmt.setCodec("audio/pcm");  //�����ʽ
		fmt.setByteOrder(QAudioFormat::LittleEndian);
		fmt.setSampleType(QAudioFormat::UnSignedInt);   //������Ƶ����
		output = new QAudioOutput(fmt);
		io = output->start();
		mutex.unlock();
		return true;
	}

	void Play(bool isPlay)
	{
		mutex.lock();
		if (!output)
		{
			mutex.unlock();
			return;
		}

		if (isPlay)
		{
			output->resume();   //�ָ�����
		}
		else {
			output->suspend();   //��ͣ����
		}
		mutex.unlock();
	}

	int GetFree()
	{
		mutex.lock();
		if (output == NULL)
		{
			mutex.unlock();
			return 0;
		}

		int free = output->bytesFree();  //ʣ��ռ�
		mutex.unlock();
		return free;
	}

	bool Write(const char* data, int datasize)
	{
		mutex.lock();
		if (io)
		{
			io->write(data, datasize);  //����ȡ����Ƶд�뵽��������
			mutex.unlock();
			return true;
		}
	}
};

XAudioPlay::XAudioPlay()
{
}


XAudioPlay::~XAudioPlay()
{}

XAudioPlay *XAudioPlay::Get()
{
	static CXAudioPlay adp;
	return &adp;
}

