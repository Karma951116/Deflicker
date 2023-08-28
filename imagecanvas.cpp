#include "imagecanvas.h"
#include <QPainter>
#include <opencv2/imgproc.hpp>

ImageCanvas::ImageCanvas()
{

}

void ImageCanvas::paint(QPainter *painter)
{
    painter->setRenderHint(QPainter::Antialiasing, true);

    int width = this->width();
    int height = this->height();

    painter->setBrush(Qt::black);
    painter->drawRect(0, 0, width, height);

    if (!image_.isNull()) {
        QImage img = image_.scaled(QSize(width, height), Qt::KeepAspectRatio);
        /* calculate display position */
        int x = (this->width() - img.width()) / 2;
        int y = (this->height() - img.height()) / 2;
        painter->drawImage(QPoint(x, y), img);
    }
}

void ImageCanvas::updateFrame()
{
    cv::Mat mat = buffer_->read();
//    cv::Mat matDisp;
//    mat.convertTo(matDisp, CV_8UC3);
    image_ = QImage(mat.data, mat.cols, mat.rows, QImage::Format_BGR888).copy();
    update();
}

void ImageCanvas::setBuffer(SequenceBuffer *buffer)
{
    buffer_ = buffer;
}
