#include <QPainter>
#include <QEvent>
#include <QPixmap>

#include "imagewidget.h"

ImageWidget::ImageWidget(QWidget *parent) :
    QWidget(parent)
{
}




void ImageWidget::paintEvent(QPaintEvent *)
{
    QSize size = this->size();
    QPixmap pixmap(size);
    pixmap.fill(Qt::white);

    QPainter p(&pixmap);
    boost::shared_lock<boost::shared_mutex> lock(picRMutex);

    if(picture.get() != nullptr && !picture->empty())
    {
        cv::Mat mat;
        cv::resize(*picture, mat, cv::Size(size.width(), size.height()));
        QImage img(size.width(), size.height(), QImage::Format_RGB888);

        for(int i = 0; i < size.width(); ++i)
        {
            for(int j = 0; j < size.height(); ++j)
            {
                uchar b = mat.at<cv::Vec3b>(j, i)[0];
                uchar g = mat.at<cv::Vec3b>(j, i)[1];
                uchar r = mat.at<cv::Vec3b>(j, i)[2];

                img.setPixel(i, j, qRgb(r, g, b));
            }
        }

        p.drawImage(0,0, img);
    }

    lock.unlock();
    p.end();

    p.begin(this);
    p.drawPixmap(0, 0, pixmap);
    p.end();

}

void ImageWidget::setPicture(std::shared_ptr<cv::Mat> pic)
{
    if(pic.get() != nullptr)
    {
        picMutex.lock();
        picture.swap(pic);
        picMutex.unlock();
    }

    update();
}
