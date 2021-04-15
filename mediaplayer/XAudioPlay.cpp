#include "XAudioPlay.h"
#include <qaudiooutput.h>
#include <qmutex.h>

class CXAudioPlay :public XAudioPlay
{
public:
	QAudioOutput *output = NULL;
	QIODevice *io = NULL;
	QMutex mutex;   //互斥量
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
		//QAudioOutput *out;   //播放音频
		QAudioFormat fmt;
		fmt.setSampleRate(48000);
		fmt.setSampleSize(16);
		fmt.setChannelCount(2);
		fmt.setCodec("audio/pcm");  //解码格式
		fmt.setByteOrder(QAudioFormat::LittleEndian);
		fmt.setSampleType(QAudioFormat::UnSignedInt);   //设置音频类型
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
			output->resume();   //恢复播放
		}
		else {
			output->suspend();   //暂停播放
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

		int free = output->bytesFree();  //剩余空间
		mutex.unlock();
		return free;
	}

	bool Write(const char* data, int datasize)
	{
		mutex.lock();
		if (io)
		{
			io->write(data, datasize);  //将获取的音频写入到缓冲区中
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

