#include "ffmpeg.h"

Xffmpeg::Xffmpeg()
{
	avcodec_register_all();
}


Xffmpeg::~Xffmpeg()
{
	free();
	Close();
}
void Xffmpeg::loadvideoFile(const char* file_path)  //�����ļ�·��
{
	this->video_path = file_path;
	qDebug() << TIMEMS << " store path" << video_path;
}

bool Xffmpeg::initVideo(const char* file_path)  //video�ļ�����
{
	Close();
	mutex.lock();

	avFormatContext = avformat_alloc_context();   //��ʽ���ݳ�ʼ��
	if (!file_path)
	{
		qDebug() << TIMEMS << file_path << " loading video path error";
		return false;
	}
	
	ret = avformat_open_input(&avFormatContext, file_path, nullptr, nullptr);   //���ļ�
	if (ret != 0)
	{
		qDebug() << TIMEMS << " open video file";
		return false;
	}
	
	videoStream_index = -1;
	audioStream_index = -1;

	ret = avformat_find_stream_info(avFormatContext, nullptr);   //�ҳ��ļ��е�����Ϣ
	if (ret < 0)
	{
		qDebug() << TIMEMS << " cannot find stream info...";
		return false;
	}

	for (int i = 0; i < avFormatContext->nb_streams; i++)
	{
		  //��ȡ��Ƶ��������
		if (avFormatContext->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)   //�ҳ�������Ƶ���Ľ�������
		{
			videoStream_index = i;  //������Ƶ�����к�codecpar
			pts = r2d(avFormatContext->streams[i]->avg_frame_rate);
			break;
		}
	}

	for (int i = 0; i < avFormatContext->nb_streams; i++)
	{
		if (avFormatContext->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO)
		{
			audioStream_index = i;
			break;
		}
	}


	if (videoStream_index == -1 || audioStream_index == -1)
	{
		qDebug() << TIMEMS << "find video/audio stream index error";
		return false;
	}
	
	videoCodec = avFormatContext->streams[videoStream_index]->codec;  //��ȡ��Ƶ��������
	audioCodec = avFormatContext->streams[audioStream_index]->codec;  //�����Ƶ��������

	videoDecoder = avcodec_find_decoder(videoCodec->codec_id);  //��Ƶ������
	audioDecoder = avcodec_find_decoder(audioCodec->codec_id);  //��Ƶ������

	//������Ƶ�����Ȳ���
	this->sampleRate = audioCodec->sample_rate;   //������
	this->channel = audioCodec->channels;         //ͨ����
	if(audioCodec)
	{
		switch (audioCodec->sample_fmt)
		{
		case AV_SAMPLE_FMT_S16:
			this->sampleSize = 16;
			break;
		case AV_SAMPLE_FMT_S32:
			this->sampleSize = 32;
		default:
			break;
		}
		qDebug() << TIMEMS << " audio sample rate: " << this->sampleRate \
			<< " sample size: " << this->sampleSize << " channel: " << this->channel;
	}

	if (avcodec_open2(videoCodec, videoDecoder, nullptr) < 0)
	{
		qDebug()<<TIMEMS << "can not decode video decoder...";
		return false;
	}

	if (avcodec_open2(audioCodec, audioDecoder, nullptr) < 0)
	{
		qDebug() << TIMEMS << "can not decode audio decoder...";
		return false;
	}

	totalMs = avFormatContext->duration / (AV_TIME_BASE);
	qDebug() << TIMEMS << " file totalSec is " << totalMs / 60 << " : " << totalMs % 60;
	swsContext = NULL;
	mutex.unlock();
	return true;
}

//��ȡpacket����
AVPacket Xffmpeg::readpkt()  
{
	AVPacket pkt;
	memset(&pkt, 0, sizeof(AVPacket));
	mutex.lock();    //����
	if (!avFormatContext)
	{
		mutex.unlock();
		return pkt;
	}

	int err = av_read_frame(avFormatContext, &pkt) ;  //��ȡ��Ƶ֡
	if (err != 0)//��ȡʧ��
	{
		mutex.unlock();
		qDebug() << TIMEMS << "read avpacket error";
		return pkt;
	}
	mutex.unlock();
	return pkt;
}

//����pkt�ļ�
int Xffmpeg::Decode(const AVPacket* pkt1)
{
	mutex.lock();
	if (!avFormatContext)
	{
		mutex.unlock();
		return 0;
	}

	if (videoFrame == NULL)
		videoFrame = av_frame_alloc();

	if (avFrame2 == NULL)
		avFrame2 = av_frame_alloc();

	AVFrame *frame = videoFrame;

	if (pkt1->stream_index == audioStream_index)
	{
		frame = avFrame2;
	}

/*	int re = -3;
	while (re < 0)
	{
		re = avcodec_send_packet(avFormatContext->streams[pkt1->stream_index]->codec, pkt1);
		re = avcodec_receive_frame(avFormatContext->streams[pkt1->stream_index]->codec, videoFrame);
		if (re == 0)
			break;
	}
	if (re < 0)
	{
		mutex.unlock();
		qDebug() << TIMEMS << " can not get yuv";
		return 0;
	}
	*/

	int re = avcodec_send_packet(avFormatContext->streams[pkt1->stream_index]->codec, pkt1);
	if (re != 0)
	{
		mutex.unlock();
		return 0;
	}

	re = avcodec_receive_frame(avFormatContext->streams[pkt1->stream_index]->codec, frame);
	if (re != 0)
	{
		mutex.unlock();
		return 0;
	}
	mutex.unlock();

	int p = frame->pts*r2d(avFormatContext->streams[pkt1->stream_index]->time_base);  //��ǰ������ʾ��ʱ��
	if (pkt1->stream_index == audioStream_index)
		this->pts = p;   //������Ƶ��ʱ��pts
	//qDebug() << TIMEMS << " pts=" << p;
	return p;
}

int Xffmpeg::GetPts(const AVPacket * pkt)
{
	return pts;
}

bool Xffmpeg::ToRGB(const AVFrame * yuv, char *out, int outwidth, int outheight)
{
	mutex.lock();
	if (!avFormatContext)
	{
		mutex.unlock();
		return false;
	}

	AVCodecContext *videoCtx = avFormatContext->streams[this->videoStream_index]->codec;
	swsContext = sws_getCachedContext(   //��ʼ��һ��swsContext
		swsContext, videoCtx->width, videoCodec->height,
		videoCodec->pix_fmt, outwidth, outheight,
		AV_PIX_FMT_BGRA,  //������ظ�ʽ
		SWS_BICUBIC,      //ת���㷨
		NULL, NULL, NULL);

	if (!swsContext)
	{
		mutex.unlock();
		qDebug() << TIMEMS << "sws_getCachedContext failed";
		return false;
	}

	uint8_t *data[AV_NUM_DATA_POINTERS] = { 0 };
	data[0] = (uint8_t *)out;
	int linesize[AV_NUM_DATA_POINTERS] = { 0 };
	linesize[0] = outwidth * 4;  //�еĿ�ȣ� 32λ 4�ֽ�

	int h = sws_scale(swsContext, yuv->data,   //��ǰ���������ÿ��ͨ������ָ��
				yuv->linesize,  //ÿ��ͨ������
				0, videoCodec->height, //ԭ��Ƶ֡�ĸ߶�
				data, linesize);  //�����ÿ��ͨ������ָ�룬  ÿ��ͨ�����ֽ���

	mutex.unlock();
	return true;
}

int Xffmpeg::ToPCM(char * out)
{
	mutex.lock();
	if (!avFormatContext || !out || !videoFrame)
	{
		mutex.unlock();
		return 0;
	}

	if (aCtx == NULL)
	{
		aCtx = swr_alloc();
		swr_alloc_set_opts(aCtx, audioCodec->channel_layout,
			AV_SAMPLE_FMT_S16,
			audioCodec->sample_rate,
			audioCodec->channels,
			audioCodec->sample_fmt,
			audioCodec->sample_rate,
			0, 0);
		swr_init(aCtx);
	}
	uint8_t *data[1];
	data[0] = (uint8_t *)out;

	int len = swr_convert(aCtx, data, 10000,
		(const uint8_t **)avFrame2->data,
		avFrame2->nb_samples);

	if (len <= 0)
	{
		mutex.unlock();
		return 0;
	}

	int outsize = av_samples_get_buffer_size(NULL, audioCodec->channels,
		avFrame2->nb_samples, AV_SAMPLE_FMT_S16, 0);

	mutex.unlock();
	return outsize;
}

void Xffmpeg::free()   //�ͷ���ر����ռ�
{
	if (swsContext != NULL)
	{
		sws_freeContext(swsContext);
		swsContext = NULL;
	}

	if (avPacket != NULL)
	{
		av_packet_unref(avPacket);
		avPacket = NULL;
	}

	if (videoFrame != NULL)
	{
		av_frame_free(&videoFrame);
		videoFrame = NULL;
	}

	if (avFrame2 != NULL)
	{
		av_frame_free(&avFrame2);
		avFrame2 = NULL;
	}

	if (avFrame3 != NULL)
	{
		av_frame_free(&avFrame3);
		avFrame3 = NULL;
	}

	if (videoCodec != NULL)
	{
		avcodec_close(videoCodec);
		videoCodec = NULL;
	}

	if (audioCodec != NULL)
	{
		avcodec_close(audioCodec);
		audioCodec = NULL;
	}

	if (avFormatContext != NULL)
	{
		avformat_close_input(&avFormatContext);
		avFormatContext = NULL;
	}
}

bool Xffmpeg::Seek(float pos)
{
	mutex.lock();
	if (!avFormatContext)
	{
		mutex.unlock();
		return false;
	}

	int64_t stamp = 0;
	stamp = pos * avFormatContext->duration / (AV_TIME_BASE);  //��ǰʵ��λ��
	pts = stamp / av_q2d(avFormatContext->streams[videoStream_index]->time_base);  //��û������������ʱ���
	int re = av_seek_frame(avFormatContext, videoStream_index, pts,
			AVSEEK_FLAG_BACKWARD | AVSEEK_FLAG_FRAME);   //����Ƶ�Ƶ���ǰ�����������λ��
	avcodec_flush_buffers(avFormatContext->streams[videoStream_index]->codec);  //ˢ�»���
	mutex.unlock();
	if (re > 0)
		return true;
	return false;
}

void Xffmpeg::Close()
{
	mutex.lock();
	if (avFormatContext)
		avformat_close_input(&avFormatContext);
	mutex.unlock();
}
