#include "functiontools.h"
#include <QJsonObject>
#include <opencv2/opencv.hpp>

FunctionTools::FunctionTools(QObject *parent) : QObject(parent)
{

}

QJsonArray FunctionTools::getHistGray(SequenceBuffer *buffer)
{
    if (buffer->nbFrames() <= 0) {
        return QJsonArray();
    }
    QVector<unsigned short> hist = QVector<unsigned short>(65536);
    for (int i = 0; i < 65536; i++) {
        hist[i] = 0;
    }
    cv::Mat mat = buffer->read();
    for (int i = 0; i < mat.rows; i++) {
        for (int j = 0; j < mat.cols; j++) {
            unsigned short pix = mat.at<cv::Vec3w>(i, j)[0];
            hist[pix] += 1;
        }
    }
    QJsonArray retHist = QJsonArray();
    for (int i = 0; i < hist.size(); i++) {
        retHist.append(hist[i]);
    }

    return retHist;
}
