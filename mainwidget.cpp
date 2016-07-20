#include <qgridlayout.h>
#include <QFileDialog>
#include <QDebug>
#include <QKeyEvent>
#include <unistd.h>

#include "mainwidget.h"

MainWidget::MainWidget(QWidget *parent) :
    QWidget(parent)
{
    QGridLayout *mainLayout = new QGridLayout;

    setFixedSize(954, 718);

    m_open.setText("open image");
    m_start.setText("snow show");
    m_animotion.setText("snow down");
    m_start.setDisabled(true);
    m_animotion.setDisabled(true);

    connect(&m_open, SIGNAL(clicked()), this, SLOT(OpenFile1()));
    connect(&m_start, SIGNAL(clicked()), this, SLOT(StartSnow()));
    connect(&m_animotion, SIGNAL(clicked()), this, SLOT(timerUpDate()));


    mainLayout->addWidget(&m_image, 0, 0, 4, 4);
    mainLayout->addWidget(&m_open,  4, 0, 1, 1);
    mainLayout->addWidget(&m_start, 4, 2, 1, 1);
    mainLayout->addWidget(&m_animotion, 4, 3, 1, 1);


    this->setLayout(mainLayout);

    QString SnowName = "/home/zhenweizhang/my_progs/qt_cv/data/bg.bmp";
    OpenFile(SnowName,pic1);
    m_image.setPicture(std::shared_ptr<cv::Mat>(&pic1));

    m_thread.start();

    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(SnowDown()));
}

void MainWidget::OpenFile1()
{
    QString filter = "pic(*.jpg |*.png | *.bmp | *.eps)";
    QDir dir;
    QString fileName = QFileDialog::getOpenFileName(this, "please choose left picture",
                                                    dir.absolutePath(), filter);

    OpenFile(fileName, pic1);

    update();
}

void MainWidget::StartSnow()
{
    QString SnowName = "/home/zhenweizhang/my_progs/qt_cv/data/snow.bmp";
    QByteArray ba = SnowName.toLocal8Bit();
    iron = cv::imread(ba.data());
    cv::resize(iron, iron, cv::Size(iron.cols*0.5, iron.rows*0.5));

    snow_x.clear(); snow_y.clear();
    srand((unsigned)time(0));
    int count = rand()%6 + 4;
    for (int var = 0; var < count; ++var) {
        size_t tmp_x = 62*(rand()%15);
        size_t tmp_y = 24*(rand()%10);
        snow_x.push_back(tmp_x);
        snow_y.push_back(tmp_y);
        cv::Mat imageROI = pic1(cv::Rect(tmp_x,tmp_y,iron.cols,iron.rows));
        cv::addWeighted(imageROI, 1.0, iron, 0.8, 0.1, imageROI);
    }

    update();
    m_start.setDisabled(true);
    m_animotion.setEnabled(true);
}

void MainWidget::SnowDown()
{
    QString SnowName = "/home/zhenweizhang/my_progs/qt_cv/data/bg.bmp";
    OpenFile(SnowName,pic1);

    int snowCount = snow_x.size();
    if(snowCount < 4)
    {
        srand((unsigned)time(0));
        int count = rand()%5;
        for (int var = 0; var < count; ++var) {
            size_t tmp_x = 62*(rand()%15);
            size_t tmp_y = 24*(rand()%10);
            snow_x.push_back(tmp_x);
            snow_y.push_back(tmp_y);
        }

    }
    std::vector<size_t>::iterator iter_x = snow_x.begin();
    std::vector<size_t>::iterator iter_y = snow_y.begin();
    for (; iter_x != snow_x.end() ; ++iter_x,++iter_y) {
        srand((unsigned)time(0));
        int speed = (rand()%8)+8;
        *iter_y +=speed;
        if(*iter_y+iron.rows > pic1.rows){
            snow_x.erase(iter_x);
            snow_y.erase(iter_y);
            break;
        }
    }

    for (iter_x = snow_x.begin(),iter_y = snow_y.begin(); iter_x != snow_x.end() ; ++iter_x,++iter_y) {
        cv::Mat imageROI = pic1(cv::Rect(*iter_x,*iter_y,iron.cols,iron.rows));
        cv::addWeighted(imageROI, 1.0, iron, 0.8, 0.1, imageROI);
    }
    update();

}

void MainWidget::timerUpDate()
{
    timer->start(160);
}



void MainWidget::OpenFile(QString &fileName, cv::Mat &pic)
{
    if(!pic.empty())
        pic.release();
    QByteArray ba = fileName.toLocal8Bit();
    pic = cv::imread(ba.data());
    m_start.setEnabled(true);
}

void MainWidget::keyPressEvent(QKeyEvent *e)
{
    switch ( e->key() )
    {
    case Qt::Key_Escape:
        close();
        break;

   default:
        break;
    }
}
