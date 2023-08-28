#include <QDebug>
#include <QtCore/qmath.h>

#include "workerlinearlzy.h"
#include "opencv2/core/matx.hpp"
//#include "opencv2/highgui.hpp"

using namespace cv;

WorkerLinearLzy::WorkerLinearLzy(SequenceBuffer* srcBuffer, SequenceBuffer* dstBuffer,
                                 Eigenvalue mode,
                                 QList<double>* srcLuminance, QList<double>* dstLuminance)
{
    srcBuffer_ = srcBuffer;
    dstBuffer_ = dstBuffer;
    mode_ = mode;
    srcLuminance_ = srcLuminance;
    dstLuminance_ = dstLuminance;
}

void WorkerLinearLzy::run()
{
    if (srcBuffer_ == nullptr || dstBuffer_ == nullptr) {
        qDebug() << "deflicker buffer not ready";
        return;
    }
    if (dstBuffer_->nbFrames()) {
        dstBuffer_->clear();
    }
    dstBuffer_->append(srcBuffer_->read(0));
    // 以像素亮度均值作为特征值
    switch (mode_) {
    case AVERAGE_GRAYSCALE: {
        // 帧循环
        for (int i = 1; i < srcBuffer_->nbFrames(); i++) {
            double alpha = 0.0, beta = 0.0;
            cv::Mat prevImg = dstBuffer_->read(i - 1);
            cv::Mat curImg = srcBuffer_->read(i);
            int pixs = curImg.rows * curImg.cols;
            // 像素循环
            double crossSum1, crossSum2, preLumSum, curLumSum, prevE, curE;
            crossSum1 = crossSum2 = preLumSum = curLumSum = prevE = curE = 0;
            // 对于黑白图像取任意通道值
            for (int row = 0; row < curImg.rows; row++) {
                for (int col = 0; col < curImg.cols; col++) {
                    double prevPix = prevImg.at<cv::Vec3w>(row, col)[0];
                    double curPix = curImg.at<cv::Vec3w>(row, col)[0];
                    crossSum1 += (prevPix * curPix);
                    crossSum2 += (prevPix * prevPix);
                    preLumSum += prevPix;
                    curLumSum += curPix;
                }
            }
            prevE = preLumSum / pixs;
            curE = curLumSum / pixs;
            alpha = (crossSum1 - curE * prevE) / (crossSum2 - prevE);
            beta = curE - alpha * prevE;
            cv::Mat dstMat = (1 / alpha) * curImg  - beta / alpha;
            dstBuffer_->append(dstMat);
            qDebug() << "process done. index->" + QString::number(i);
        }
        break;
    }
    case STANDARD_DEVIATION: {
        float srcMean = 0;
        for (int i = 0; i < srcLuminance_->size(); i++) {
            srcMean += srcLuminance_->at(i);
        }
        srcMean /= srcLuminance_->size();
        for (int i = 1; i < srcBuffer_->nbFrames(); i++) {
            float alpha = 0.0, beta = 0.0;
            cv::Mat prevImg = dstBuffer_->read(i - 1);
            cv::Mat curImg = srcBuffer_->read(i);
            int pixs = curImg.rows * curImg.cols;
            // 像素循环
            double crossSum1, crossSum2, preLumSum, curLumSum, prevE, curE;
            crossSum1 = crossSum2 = preLumSum = curLumSum = prevE = curE = 0;
            // 对于黑白图像取任意通道值
            for (int row = 0; row < curImg.rows; row++) {
                for (int col = 0; col < curImg.cols; col++) {
                    double prevPix = prevImg.at<cv::Vec3w>(row, col)[0] / 65535.0;
                    double curPix = curImg.at<cv::Vec3w>(row, col)[0] / 65535.0;
                    crossSum1 += (prevPix * curPix);
                    crossSum2 += (prevPix * prevPix);
                    preLumSum += prevPix;
                    curLumSum += curPix;
                }
            }
            prevE = sqrt(qPow(preLumSum / pixs - srcMean, 2));
            curE = sqrt(qPow(curLumSum / pixs - srcMean, 2));
            alpha = (crossSum1 - curE * prevE) / (crossSum2 - prevE);
            beta = curE - alpha * prevE;
            cv::Mat dstMat = curImg / alpha - beta / alpha;
            dstBuffer_->append(dstMat);
            qDebug() << "process done. index->" + QString::number(i);
        }
    }
    }
    emit procDone();
    this->deleteLater();
}
