#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <opencv2/opencv.hpp>
#include <QWidget>
#include <QPushButton>
#include <QTimer>
#include <QThread>

#include <stdlib.h>
#include <alsa/asoundlib.h>

#include "imagewidget.h"

class GLthread: public QThread
{
    Q_OBJECT
public:
    explicit GLthread(QObject *parent = 0)
    {}
    ~GLthread(){
        stop();
    }
    void stop(){
        this->exit();
    }
private:
    void run(){
        int ret;
        unsigned int val;
        int dir=0;
        char *buffer;
        int size;
        snd_pcm_uframes_t frames;
        snd_pcm_uframes_t periodsize;
        snd_pcm_t *playback_handle;//PCM设备句柄pcm.h
        snd_pcm_hw_params_t *hw_params;//硬件信息和PCM流配置

        FILE *fp = fopen("data/bg.wav", "rb");
        if(fp == NULL)
            exit(-1);
        fseek(fp, 100, SEEK_SET);

        //1. 打开PCM，最后一个参数为0意味着标准配置
        ret = snd_pcm_open(&playback_handle, "default", SND_PCM_STREAM_PLAYBACK, 0);
        if (ret < 0) {
            perror("snd_pcm_open");
            exit(1);
        }

        //2. 分配snd_pcm_hw_params_t结构体
        ret = snd_pcm_hw_params_malloc(&hw_params);
        if (ret < 0) {
            perror("snd_pcm_hw_params_malloc");
            exit(1);
        }
        //3. 初始化hw_params
        ret = snd_pcm_hw_params_any(playback_handle, hw_params);
        if (ret < 0) {
            perror("snd_pcm_hw_params_any");
            exit(1);
        }
        //4. 初始化访问权限
        ret = snd_pcm_hw_params_set_access(playback_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED);
        if (ret < 0) {
            perror("snd_pcm_hw_params_set_access");
            exit(1);
        }
        //5. 初始化采样格式SND_PCM_FORMAT_U8,8位
        ret = snd_pcm_hw_params_set_format(playback_handle, hw_params, SND_PCM_FORMAT_U8);
        if (ret < 0) {
            perror("snd_pcm_hw_params_set_format");
            exit(1);
        }
        //6. 设置采样率，如果硬件不支持我们设置的采样率，将使用最接近的
        //val = 44100,有些录音采样频率固定为8KHz


        val = 8000;
        ret = snd_pcm_hw_params_set_rate_near(playback_handle, hw_params, &val, &dir);
        if (ret < 0) {
            perror("snd_pcm_hw_params_set_rate_near");
            exit(1);
        }
        //7. 设置通道数量
        ret = snd_pcm_hw_params_set_channels(playback_handle, hw_params, 2);
        if (ret < 0) {
            perror("snd_pcm_hw_params_set_channels");
            exit(1);
        }

        /* Set period size to 32 frames. */
        frames = 32;
        periodsize = frames * 2;
        ret = snd_pcm_hw_params_set_buffer_size_near(playback_handle, hw_params, &periodsize);
        if (ret < 0)
        {
            printf("Unable to set buffer size %li : %s\n", frames * 2, snd_strerror(ret));

        }
        periodsize /= 2;

        ret = snd_pcm_hw_params_set_period_size_near(playback_handle, hw_params, &periodsize, 0);
        if (ret < 0)
        {
            printf("Unable to set period size %li : %s\n", periodsize,  snd_strerror(ret));
        }

        //8. 设置hw_params
        ret = snd_pcm_hw_params(playback_handle, hw_params);
        if (ret < 0) {
            perror("snd_pcm_hw_params");
            exit(1);
        }

        /* Use a buffer large enough to hold one period */
        snd_pcm_hw_params_get_period_size(hw_params, &frames, &dir);

        size = frames * 2; /* 2 bytes/sample, 2 channels */
        buffer = (char *) malloc(size);
        fprintf(stderr,
                "size = %d\n",
                size);

        while (1)
        {
            ret = fread(buffer, 1, size, fp);
            if(ret == 0)
            {
                fprintf(stderr, "end of file on input\n");
                break;
            }
            else if (ret != size)
            {
            }
            //9. 写音频数据到PCM设备
            while(ret = snd_pcm_writei(playback_handle, buffer, frames)<0)
            {
                usleep(2000);
                if (ret == -EPIPE)
                {
                    /* EPIPE means underrun */
                    fprintf(stderr, "underrun occurred\n");
                    //完成硬件参数设置，使设备准备好
                    snd_pcm_prepare(playback_handle);
                }
                else if (ret < 0)
                {
                    fprintf(stderr,
                            "error from writei: %s\n",
                            snd_strerror(ret));
                }
            }

        }
        //10. 关闭PCM设备句柄
        snd_pcm_close(playback_handle);
    }
signals:

public slots:
};

class MainWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MainWidget(QWidget *parent = 0);

signals:

public slots:
    void OpenFile1();
    void StartSnow();
    void SnowDown();
    void timerUpDate();

private:
    void OpenFile(QString &fileName, cv::Mat &pic);
    void keyPressEvent( QKeyEvent *e );

private:
    ImageWidget             m_image;
    QPushButton             m_open,m_start,m_animotion;

    QByteArray              ba;
    cv::Mat                 pic1;
    std::vector<cv::Mat>    iron;

    std::vector<size_t>     snow_x;
    std::vector<size_t>     snow_y;
    QTimer                  *timer;
    GLthread                m_thread;
};

#endif // MAINWIDGET_H
