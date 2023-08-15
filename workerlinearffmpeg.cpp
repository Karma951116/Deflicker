#include "workerlinearffmpeg.h"
#include <QtCore/qmath.h>
#include <QDebug>

WorkerLinearFFMPEG::WorkerLinearFFMPEG(SequenceBuffer* srcBuffer, SequenceBuffer* dstBuffer,
                                       FactorType mode,
                                       QList<double>* srcLuminance, QList<double>* dstLuminance)
{
    srcBuffer_ = srcBuffer;
    dstBuffer_ = dstBuffer;
    factorType_ = mode;
    srcLuminance_ = srcLuminance;
    dstLuminance_ = dstLuminance;
}

void WorkerLinearFFMPEG::run()
{
    if (srcBuffer_ == nullptr || dstBuffer_ == nullptr) {
        qDebug() << "deflicker buffer not ready";
        return;
    }
    if (dstBuffer_->nbFrames()) {
        dstBuffer_->clear();
    }
    double factor = 0.0;
    int ref = 0;
    switch (factorType_) {
    case ARITHMETIC_MEAN: {
        for (int i = 0; i < srcLuminance_->size(); i++) {
            factor += srcLuminance_->at(i);
        }
        factor /= srcLuminance_->size();
        factor /= srcLuminance_->at(ref);
        break;
    }
    case GEOMETRIC_MEAN: {
        factor = 1.0;
        for (int i = 0; i < srcLuminance_->size(); i++) {
            factor *= srcLuminance_->at(i);
        }
        factor = qPow(factor, 1.0f / srcLuminance_->size());
        factor /= srcLuminance_->at(ref);
        break;
    }
    case HARMONIC_MEAN: {
        for (int i = 0; i < srcLuminance_->size(); i++) {
            factor += 1.0f / srcLuminance_->at(i);
        }
        factor = srcLuminance_->size() / factor;
        factor /= srcLuminance_->at(ref);
        break;
    }
    case QUADRATIC_MEAN: {
        for (int i = 0; i < srcLuminance_->size(); i++) {
            factor += srcLuminance_->at(i) * srcLuminance_->at(i);
        }
        factor /= srcLuminance_->size();
        factor = sqrtf(factor);
        factor /= srcLuminance_->at(ref);
        break;
    }
    case CUBIC_MEAN: {
        for (int i = 0; i < srcLuminance_->size(); i++) {
            factor += srcLuminance_->at(i) * srcLuminance_->at(i) * srcLuminance_->at(i);
        }
        factor /= srcLuminance_->size();
        factor = cbrtf(factor);
        factor /= srcLuminance_->at(ref);
        break;
    }
    case POWER_MEAN: {
        for (int i = 0; i < srcLuminance_->size(); i++) {
            factor += qPow(srcLuminance_->at(i), srcLuminance_->size());
        }
        factor /= srcLuminance_->size();
        factor = qPow(factor, 1.0f / srcLuminance_->size());
        factor /= srcLuminance_->at(ref);
        break;
    }
    case MEDIAN: {
        break;
    }
    }
    for (int i = 0; i < srcBuffer_->nbFrames(); i++) {
        cv::Mat curImg = srcBuffer_->read(i);
        cv::Mat dstMat = curImg * factor;
        for (int row = 0; row < dstMat.rows; row++) {
            for (int col = 0; col < dstMat.cols; col++) {
                double pix = dstMat.at<cv::Vec3w>(row, col)[0];
                if (pix < 0) pix = 0;
                else if (pix > 65536) pix = 65536;
            }
        }
        dstBuffer_->append(dstMat);
        qDebug() << "process done. index->" + QString::number(i);
    }
    procDone();
    this->deleteLater();
}
