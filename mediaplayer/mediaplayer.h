#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_mediaplayer.h"
#include "qfiledialog.h"
#include "qmessagebox.h"
#include <qlabel.h>
#include "XAudioPlay.h"

class mediaplayer : public QMainWindow
{
    Q_OBJECT

public:
    mediaplayer(QWidget *parent = Q_NULLPTR);
	void timerEvent(QTimerEvent* event);    //ʱ������
	void resizeEvent(QResizeEvent* event);   //���ڴ�С����


public slots:
	void open();  //�ۺ��� ��Ӧ���ļ�
	void play();
	void sliderPressed();
	void sliderReleased();

public:
	

private:
    Ui::mediaplayerClass mainui;

};
