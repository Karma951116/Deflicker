#include "deflickers.h"
#include "workerlinearlzy.h"
#include "workerlinearffmpeg.h"


#include <QDebug>

using namespace Deflickers;

GlobalDeflicker::GlobalDeflicker(SequenceBuffer *srcBuffer, SequenceBuffer *dstBuffer)
{
    curWork_ = nullptr;
    type_ = LINEAR_LZY;
    srcBuffer_ = srcBuffer;
    dstBuffer_ = dstBuffer;
    srcLuminance_ = QList<double>();
    dstLuminance_ = QList<double>();
    srcHist_ = QJsonArray();
    dstHist_ = QJsonArray();
}

void GlobalDeflicker::process() {
    if (curWork_ != nullptr) {
        qDebug() << "worker busy now";
        return;
    }
    switch (type_) {
    case LINEAR_LZY: {
        WorkerLinearLzy* worker = new WorkerLinearLzy(srcBuffer_, dstBuffer_,
                                                      WorkerLinearLzy::AVERAGE_GRAYSCALE,
                                                      &srcLuminance_, &dstLuminance_);
        curWork_ = worker;
        worker->start();
        connect(worker, SIGNAL(finished()), this, SLOT(onFinished()));
        connect(worker, SIGNAL(procDone()), this, SLOT(onProcDone()));
        break;
    }
    case LINEAR_FFMPEG:
        WorkerLinearFFMPEG* worker = new WorkerLinearFFMPEG(srcBuffer_, dstBuffer_,
                                                         WorkerLinearFFMPEG::CUBIC_MEAN,
                                                         &srcLuminance_, &dstLuminance_);
        curWork_ = worker;
        worker->start();
        connect(worker, SIGNAL(finished()), this, SLOT(onFinished()));
        connect(worker, SIGNAL(procDone()), this, SLOT(onProcDone()));
        break;
    }
}

void GlobalDeflicker::setType(const GlobalMethod &value) {
    type_ = value;
}

QJsonArray GlobalDeflicker::srcLuminance() const
{
    QJsonArray arr;
    for (int i = 0; i < srcLuminance_.size(); i++) {
        arr.append(srcLuminance_.at(i));
    }
    return arr;
}

QJsonArray GlobalDeflicker::dstLuminance() const
{
    QJsonArray arr;
    for (int i = 0; i < dstLuminance_.size(); i++) {
        arr.append(dstLuminance_.at(i));
    }
    return arr;
}

QJsonArray GlobalDeflicker::srcHist(int index) const
{
    return srcHist_.at(index).toArray();
}

QJsonArray GlobalDeflicker::dstHist(int index) const
{
    return dstHist_.at(index).toArray();
}

void GlobalDeflicker::onFinished() {
    curWork_ = nullptr;
}

void GlobalDeflicker::onSequenceUpdate()
{
    if (srcBuffer_->nbFrames() <= 0) {
        return;
    }
    srcLuminance_.clear();
    srcHist_.empty();
    double pixels = 0;
    for (int idx = 0; idx < srcBuffer_->nbFrames(); idx++) {
        cv::Mat mat = srcBuffer_->read();
        if (pixels == NULL) {
            pixels = mat.rows * mat.cols;
        }
        double luminance = 0;
        for (int i = 0; i < mat.rows; i++) {
            for (int j = 0; j < mat.cols; j++) {
                luminance += mat.at<cv::Vec3w>(i, j)[0];
            }
        }
        double mean = luminance / pixels;
        srcLuminance_.append(mean);
        srcHist_.append(getHist(mat));
        srcBuffer_->next();
    }
    srcBuffer_->setIndex(0);
}

QJsonArray GlobalDeflicker::getHist(cv::Mat mat) {
    QVector<unsigned short> hist = QVector<unsigned short>(65536);
    for (int i = 0; i < 65536; i++) {
        hist[i] = 0;
    }
    for (int i = 0; i < mat.rows; i++) {
        for (int j = 0; j < mat.cols; j++) {
            unsigned short pix = mat.at<cv::Vec3w>(i, j)[0];
            hist[pix] += 1;
        }
    }
    QJsonArray retHist = QJsonArray();
    for (int i = 0; i < hist.size(); i++) {
        if (hist[i] == 0) continue;
        QJsonObject obj;
        obj.insert("index", QString::number(i));
        obj.insert("value", hist[i]);
        retHist.append(obj);
    }

    return retHist;
}

void GlobalDeflicker::onProcDone()
{
    if (dstBuffer_->nbFrames() <= 0) {
        return;
    }
    double pixels = 0;
    dstLuminance_.clear();
    dstHist_.empty();
    for (int idx = 0; idx < dstBuffer_->nbFrames(); idx++) {
        cv::Mat mat = dstBuffer_->read();
        if (pixels == NULL) {
            pixels = mat.rows * mat.cols;
        }
        double luminance = 0;
        for (int i = 0; i < mat.rows; i++) {
            for (int j = 0; j < mat.cols; j++) {
                luminance += mat.at<cv::Vec3w>(i, j)[0];
            }
        }
        double mean = luminance / pixels;
        dstLuminance_.append(mean);
        dstHist_.append(getHist(mat));
        dstBuffer_->next();
    }
    dstBuffer_->setIndex(0);
    emit calcDstMeanDone();
}
