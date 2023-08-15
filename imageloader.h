#ifndef IMAGELOADER_H
#define IMAGELOADER_H

#include <QObject>
#include "dpxdecodeworker.h"

extern "C"
{
#include <libavformat/avformat.h>
#include "libswscale/swscale.h"
#include "libavutil/pixfmt.h"
#include "libavutil/imgutils.h"
}

class ImageLoader : public QObject
{
    Q_OBJECT
public:
    explicit ImageLoader(QObject *parent = nullptr);

    void setSrcBuffer(SequenceBuffer *newSrcBuffer);
    Q_INVOKABLE void load(QString path);
private:
    SequenceBuffer* srcBuffer_;
    int nbWorkers;

signals:
    void sequenceUpdate();

public slots:
    void onDecodeFinished(int index, cv::Mat mat);
};

#endif // IMAGELOADER_H
