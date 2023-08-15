#ifndef SEQUENCEBUFFER_H
#define SEQUENCEBUFFER_H

#include <QObject>
#include <opencv2/core/mat.hpp>

class SequenceBuffer : public QObject
{
    Q_OBJECT
public:
    Q_PROPERTY(int index READ getIndex NOTIFY indexChanged)
    Q_PROPERTY(int pixels READ framePixels NOTIFY pixelsChanged)

    explicit SequenceBuffer(QObject *parent = nullptr);
    ~SequenceBuffer();
    void append(cv::Mat mat);
    Q_INVOKABLE void clear();
    cv::Mat read();
    cv::Mat read(int getIndex);
    Q_INVOKABLE void next();
    Q_INVOKABLE void prev();
    Q_INVOKABLE int nbFrames();
    Q_INVOKABLE int getIndex();
    Q_INVOKABLE void insert(int getIndex, cv::Mat mat);
    Q_INVOKABLE int framePixels();
    void setIndex(int getIndex);

private:
    QList<cv::Mat>* sequence_;
    int index_;

signals:
    void indexChanged();
    void pixelsChanged();

};

#endif // SEQUENCEBUFFER_H
