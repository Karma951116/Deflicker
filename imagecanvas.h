#ifndef IMAGECANVAS_H
#define IMAGECANVAS_H

#include <QQuickPaintedItem>
#include <QObject>
#include <QImage>

#include "sequencebuffer.h"

class ImageCanvas : public QQuickPaintedItem
{
    Q_OBJECT
public:
    ImageCanvas();

    Q_INVOKABLE void setBuffer(SequenceBuffer *buffer);
    Q_INVOKABLE void updateFrame();
protected:
    virtual void paint(QPainter* painter);

private:
    QImage image_;
    SequenceBuffer* buffer_;

};

#endif // IMAGECANVAS_H
