#ifndef IMAGEWIDGET_H
#define IMAGEWIDGET_H
#include <memory>

#include <QWidget>
#include <boost/thread/mutex.hpp>
#include <boost/thread/shared_mutex.hpp>

#include <opencv2/opencv.hpp>
class ImageWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ImageWidget(QWidget *parent = 0);
    void paintEvent(QPaintEvent *);

    void setPicture(std::shared_ptr<cv::Mat> pic);

signals:

public slots:

private :
    std::shared_ptr<cv::Mat> picture;
    boost::mutex             picMutex;
    boost::shared_mutex      picRMutex;

};

#endif // IMAGEWIDGET_H
