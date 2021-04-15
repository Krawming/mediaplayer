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
	void timerEvent(QTimerEvent* event);    //时间设置
	void resizeEvent(QResizeEvent* event);   //窗口大小重置


public slots:
	void open();  //槽函数 响应打开文件
	void play();
	void sliderPressed();
	void sliderReleased();

public:
	

private:
    Ui::mediaplayerClass mainui;

};
