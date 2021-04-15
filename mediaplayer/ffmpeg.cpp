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
void Xffmpeg::loadvideoFile(const char* file_path)  //保存文件路径
{
	this->video_path = file_path;
	qDebug() << TIMEMS << " store path" << video_path;
}

bool Xffmpeg::initVideo(const char* file_path)  //video文件加载
{
	Close();
	mutex.lock();

	avFormatContext = avformat_alloc_context();   //格式内容初始化
	if (!file_path)
	{
		qDebug() << TIMEMS << file_path << " loading video path error";
		return false;
	}
	
	ret = avformat_open_input(&avFormatContext, file_path, nullptr, nullptr);   //打开文件
	if (ret != 0)
	{
		qDebug() << TIMEMS << " open video file";
		return false;
	}
	
	videoStream_index = -1;
	audioStream_index = -1;

	ret = avformat_find_stream_info(avFormatContext, nullptr);   //找出文件中的流信息
	if (ret < 0)
	{
		qDebug() << TIMEMS << " cannot find stream info...";
		return false;
	}

	for (int i = 0; i < avFormatContext->nb_streams; i++)
	{
		  //获取视频流解码器
		if (avFormatContext->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)   //找出满足视频流的解码类型
		{
			videoStream_index = i;  //保存视频流序列号codecpar
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
	
	videoCodec = avFormatContext->streams[videoStream_index]->codec;  //获取视频流解码器
	audioCodec = avFormatContext->streams[audioStream_index]->codec;  //获得音频流解码器

	videoDecoder = avcodec_find_decoder(videoCodec->codec_id);  //视频解码器
	audioDecoder = avcodec_find_decoder(audioCodec->codec_id);  //音频解码器

	//设置音频样本等参数
	this->sampleRate = audioCodec->sample_rate;   //采样率
	this->channel = audioCodec->channels;         //通道数
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

//读取packet数据
AVPacket Xffmpeg::readpkt()  
{
	AVPacket pkt;
	memset(&pkt, 0, sizeof(AVPacket));
	mutex.lock();    //加锁
	if (!avFormatContext)
	{
		mutex.unlock();
		return pkt;
	}

	int err = av_read_frame(avFormatContext, &pkt) ;  //读取视频帧
	if (err != 0)//读取失败
	{
		mutex.unlock();
		qDebug() << TIMEMS << "read avpacket error";
		return pkt;
	}
	mutex.unlock();
	return pkt;
}

//解码pkt文件
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

	int p = frame->pts*r2d(avFormatContext->streams[pkt1->stream_index]->time_base);  //当前解码显示的时间
	if (pkt1->stream_index == audioStream_index)
		this->pts = p;   //设置音频流时的pts
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
	swsContext = sws_getCachedContext(   //初始化一个swsContext
		swsContext, videoCtx->width, videoCodec->height,
		videoCodec->pix_fmt, outwidth, outheight,
		AV_PIX_FMT_BGRA,  //输出像素格式
		SWS_BICUBIC,      //转码算法
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
	linesize[0] = outwidth * 4;  //行的宽度， 32位 4字节

	int h = sws_scale(swsContext, yuv->data,   //当前处理区域的每个通道数据指针
				yuv->linesize,  //每个通道行数
				0, videoCodec->height, //原视频帧的高度
				data, linesize);  //输出的每个通道数据指针，  每个通道行字节数

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

void Xffmpeg::free()   //释放相关变量空间
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
	stamp = pos * avFormatContext->duration / (AV_TIME_BASE);  //当前实际位置
	pts = stamp / av_q2d(avFormatContext->streams[videoStream_index]->time_base);  //获得滑动条滑动后的时间戳
	int re = av_seek_frame(avFormatContext, videoStream_index, pts,
			AVSEEK_FLAG_BACKWARD | AVSEEK_FLAG_FRAME);   //将视频移到当前点击滑动条的位置
	avcodec_flush_buffers(avFormatContext->streams[videoStream_index]->codec);  //刷新缓冲
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
