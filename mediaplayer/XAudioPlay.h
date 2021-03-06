#pragma once
class XAudioPlay
{
public:
	static XAudioPlay  *Get();   //单例模式
	virtual bool Start() = 0;
	virtual void Play(bool isPlay) = 0;
	virtual bool Write(const char* data, int datasize) = 0;
	virtual void Stop() = 0;
	virtual int GetFree() = 0;
	virtual ~XAudioPlay();
	int sampleRate = 48000;  //样本率
	int sampleSize = 16;  //样本大小
	int channel = 2;  //通道数

protected:
	XAudioPlay();

};

