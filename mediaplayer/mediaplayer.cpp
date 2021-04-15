#include "mediaplayer.h"

static bool isplay = false;  //���ű�ǩ
static bool isPressSlider;   //�Ƿ����������

mediaplayer::mediaplayer(QWidget *parent)
    : QMainWindow(parent)
{
    mainui.setupUi(this);
	startTimer(40);   //������ʱ��
}


void mediaplayer::open()
{
	QString name = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("ѡ����Ƶ�ļ�"));
	if (name.isEmpty())
		return;
	this->setWindowTitle(name);
	Xffmpeg::Get()->initVideo(name.toLocal8Bit());
	int totalMs = Xffmpeg::Get()->totalMs;
	if (totalMs <= 0)
	{
		QMessageBox::information(this, "err", "file open failed!!");
		return;
	}
	//mainui.time_Slider->singleStep = float(1000 / totalMs);
	XAudioPlay::Get()->sampleRate = Xffmpeg::Get()->sampleRate;
	XAudioPlay::Get()->channel = Xffmpeg::Get()->channel;
	XAudioPlay::Get()->sampleSize = 16;
	XAudioPlay::Get()->Start();  //������Ƶ

	char buf[1024] = { 0 };    //����ܵĲ���ʱ��
	int min = (totalMs) / 60;
	int sec = (totalMs) % 60;
	sprintf(buf, "%03d:%02d", min, sec);
	mainui.total_time->setText(buf);   //���ò�����ʱ��
	play();
}

void mediaplayer::timerEvent(QTimerEvent * event)
{
	int min = (Xffmpeg::Get()->pts) / 60;
	int sec = (Xffmpeg::Get()->pts) % 60; 
	char buf[1024] = { 0 };
	sprintf(buf, "%03d:%02d /", min, sec);
	mainui.playtime->setText(buf);

	if (Xffmpeg::Get()->totalMs > 0)
	{
		float rate = (float)Xffmpeg::Get()->pts / ((float)Xffmpeg::Get()->totalMs);  //��ǰʱ��ռ��ʱ��ı���
		if (!isPressSlider)
		{
			mainui.time_Slider->setValue(rate*1000);   //���õ�ǰ������λ��
		}
	}
}

void mediaplayer::resizeEvent(QResizeEvent * event)
{
	mainui.openGLWidget->resize(size());
	//mainui.playbtn->move(this->width() / 2 + 50, this->height() - 80);
}

void mediaplayer::play()
{
	isplay = !isplay;
	Xffmpeg::Get()->isPlay = isplay;   //������״̬���ݵ�Xffmepg�е�isPlay
	if (isplay)
	{
		//XVideoThread::Get()->isexit = false;
		qDebug() << " play the movie";
	}
	else {
		//XVideoThread::Get()->isexit = true;
		qDebug() << " ---------------- not play";
	}
}

void mediaplayer::sliderPressed()
{
	isPressSlider = true;
}

void mediaplayer::sliderReleased()
{
	isPressSlider = false;
	float pos = 0;
	pos = (float)mainui.time_Slider->value() / (float)(mainui.time_Slider->maximum() + 1);
	Xffmpeg::Get()->Seek(pos);

}


