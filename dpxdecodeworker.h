#ifndef DPXDECODEWORKER_H
#define DPXDECODEWORKER_H

#include <QObject>
#include <QThread>

#include "opencv2/core/mat.hpp"
#include "sequencebuffer.h"

extern "C"
{
#include <libavformat/avformat.h>
#include "libswscale/swscale.h"
#include "libavutil/pixfmt.h"
#include "libavutil/imgutils.h"
}

class DpxDecodeWorker : public QThread
{
    Q_OBJECT
public:
    DpxDecodeWorker(QString filePath, int index);
    void run() override;
private:
    QString path_;
    int index_;
signals:
    void decodeFinished(int index, cv::Mat mat);
};

#endif // DPXDECODEWORKER_H
