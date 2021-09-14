#include "video.h"
#include "ui_video.h"
#include <QPainter>

Video::Video(QWidget *parent) :
	QMainWindow(parent),
    ui(new Ui::Video)
{
	ui->setupUi(this);
	ffmpeg=NULL;
}

Video::~Video()
{
	delete ui;
}
void Video::setFFmpeg(FFmpeg *ff)
{
	ffmpeg=ff;
}

#include <QElapsedTimer>
#include <unistd.h>
void Video::paintEvent(QPaintEvent *)
{
	static QElapsedTimer t;
	if(ffmpeg->picture.data != NULL)
	{
		QPainter painter(this);
		if(ffmpeg->mutex.tryLock(1000))
		{
			if (!ffmpeg->ready) {
				ffmpeg->mutex.unlock();
				usleep(20000);
			}
			ffmpeg->ready = false;
            QImage image=QImage(ffmpeg->picture.data[0],ffmpeg->width,ffmpeg->height,QImage::Format_RGB888);
            QPixmap  pix =  QPixmap::fromImage(image);
            painter.drawPixmap(0, 0, ui->centralwidget->width(), ui->centralwidget->height(), pix);
            this->setWindowTitle("Video Flow");
			if (t.elapsed())
				painter.drawText(10, 10, QString("FPS: %1").arg(1000/t.restart()));
			update();
			ffmpeg->mutex.unlock();
            usleep(20000);
		}
	}
}
