#pragma once
//#ifndef INT64_C
//#define INT64_C
//#define UINT64_C
//#endif

#define TIMEMS      qPrintable(QTime::currentTime().toString("HH:mm:ss zzz"))
#define STRDATETIME qPrintable(QDateTime::currentDateTime().toString("yyyy-MM-dd-HH-mm-ss"))

extern "C"
{
#include "libavutil/opt.h"
#include "libavutil/time.h"
#include "libavutil/frame.h"
#include "libavutil/pixdesc.h"
#include "libavutil/avassert.h"
#include "libavutil/imgutils.h"
#include "libavutil/ffversion.h"
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"
#include "libavformat/avformat.h"
#include "libavfilter/avfilter.h"
#include "libavdevice/avdevice.h"
#include <libswresample/swresample.h>
//#ifdef ffmpegdevice
//#endif

//#pragma comment(lib, "./3thparty/ffmpeg/lib/avformat.lib")
//#pragma comment(lib, "./3thparty/ffmpeg/lib/avutil.lib")
//#pragma comment(lib, "./3thparty/ffmpeg/lib/avcodec.lib")
//#ifndef gcc45
//#include <libavutil/hwcontext.h>
//#include <libavutil/hwcontext_qsv.h>
//#endif
}
