#include <qgridlayout.h>
#include <QFileDialog>
#include <QDebug>
#include <QKeyEvent>
#include <unistd.h>
#include <QTime>

#include <sys/time.h>
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

    QString SnowName = "data/bg.bmp";
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
    QString SnowName = "data/snow.bmp";
    ba = SnowName.toLocal8Bit();

    snow_x.clear(); snow_y.clear(),iron.clear();
    srand((unsigned)time(0));
    int count = rand()%6 + 4;
    for (int var = 0; var < count; ++var) {
        cv::Mat temp = cv::imread(ba.data());
        cv::resize(temp, temp, cv::Size(temp.cols*0.3, temp.rows*0.3));
        iron.push_back(temp);

        size_t tmp_x = 62*(rand()%15);
        size_t tmp_y = 24*(rand()%10);
        snow_x.push_back(tmp_x);
        snow_y.push_back(tmp_y);
        cv::Mat imageROI = pic1(cv::Rect(tmp_x,tmp_y,temp.cols,temp.rows));
        cv::addWeighted(imageROI, 1.0, temp, 0.8, 0.1, imageROI);
    }

    update();
    m_start.setDisabled(true);
    m_animotion.setEnabled(true);
}

void MainWidget::SnowDown()
{
    QString SnowName = "data/bg.bmp";
    OpenFile(SnowName,pic1);

    int snowCount = snow_x.size();
    if(snowCount < 4)
    {
        srand((unsigned)time(0));
        int count = rand()%6;
        for (int var = 0; var < count; ++var) {
            size_t tmp_x = 62*(rand()%15);
            size_t tmp_y = 24*(rand()%10);

            float m_size = 0.1*(float)(rand()%5)+0.1;
            cv::Mat temp = cv::imread(ba.data());
            cv::resize(temp, temp, cv::Size(temp.cols*m_size, temp.rows*m_size));

            iron.push_back(temp);
            snow_x.push_back(tmp_x);
            snow_y.push_back(tmp_y);
        }
    }

    std::vector<size_t>::iterator  iter_x = snow_x.begin();
    std::vector<size_t>::iterator  iter_y = snow_y.begin();
    std::vector<cv::Mat>::iterator iter_iron = iron.begin();

    for (; iter_x != snow_x.end() ; ++iter_iron,++iter_x,++iter_y) {
        struct timeval start;
        gettimeofday(&start,0);

        qsrand(uint(start.tv_usec));
        int speed = (qrand()%8)+8;
        *iter_y += speed;
        speed = (qrand()%8)-4;
        *iter_x += speed;
    }

    bool checkDone = false;
    while (!checkDone) {
        for (iter_iron =iron.begin(),iter_x = snow_x.begin(),iter_y = snow_y.begin();
             iter_x != snow_x.end() ; ++iter_iron,++iter_x,++iter_y) {

            if(*iter_y+ iter_iron->rows > pic1.rows || *iter_x+ iter_iron->cols > pic1.cols){
                snow_x.erase(iter_x);
                snow_y.erase(iter_y);
                iron.erase(iter_iron);
                break;
            }
        }
        if(iter_iron == iron.end()) checkDone = true;
    }


    for (iter_iron = iron.begin(),iter_x = snow_x.begin(),iter_y = snow_y.begin();
         iter_iron != iron.end(),iter_x != snow_x.end(), iter_y != snow_y.end();
         iter_x++,iter_y++,iter_iron++) {

        if((*iter_x < 0)||(*iter_x) > pic1.cols) {
//            std::cout<<"error!\n";
            continue;
        }
//        std::cout<<"*iter_x = "<<*iter_x<<", *iter_y = "<<*iter_y<<", iter_iron->cols = "<<iter_iron->cols
//                <<", iter_iron->rows = "<<iter_iron->rows<<"\n";
//        std::cout.flush();
        cv::Mat imageROI = pic1(cv::Rect(*iter_x,*iter_y,iter_iron->cols,iter_iron->rows));
        cv::addWeighted(imageROI, 1.0, *iter_iron, 0.8, 0.1, imageROI);
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
