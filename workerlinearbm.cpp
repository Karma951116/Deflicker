#include "workerlinearbm.h"
#include <QDebug>
#include "functiontools.h"
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>

int MIN_BLOCK_SIZE = 16;
int MAX_SEARCH_RANGE = 7;
int FLICKER_LUMA_THREAD = 5;

WorkerLinearBM::WorkerLinearBM(SequenceBuffer *srcBuffer, SequenceBuffer *dstBuffer,
                               WorkerLinearBM::SearchMethod mode)
{
    srcBuffer_ = srcBuffer;
    dstBuffer_ = dstBuffer;
    mode_ = mode;
}

void WorkerLinearBM::run()
{
    if (srcBuffer_ == nullptr || dstBuffer_ == nullptr) {
        qDebug() << "deflicker buffer not ready";
        return;
    }
    if (dstBuffer_->nbFrames()) {
        dstBuffer_->clear();
    }
    cv::Mat firstImg = srcBuffer_->read(0);
    dstBuffer_->append(firstImg);

    int blockSize = 16;
    while (firstImg.cols % blockSize != 0) {
        blockSize++;
        if (blockSize > 64) break;
    }
    while (firstImg.rows % blockSize != 0) {
        blockSize++;
        if (blockSize > 64) break;
    }
    if (blockSize > 64) {
        qDebug() << "no suitable block size, process stop";
        return;
    }
    // 计算分块行列数
    int block_cols = firstImg.cols / blockSize;
    int block_rows = firstImg.rows / blockSize;

    QList<cv::Rect> blockList;
    // 遍历所有块
    for (int x = 0; x < block_cols; x++) {
        for (int y = 0; y < block_rows; y++) {
            cv::Rect rect(x * blockSize, y * blockSize,
                          blockSize, blockSize);
            blockList.append(rect);
        }
    }
//
    for (int i = 1; i < srcBuffer_->nbFrames(); i++) {
        cv::Mat prevImg = srcBuffer_->read(i - 1).clone();
        cv::Mat curImg = srcBuffer_->read(i).clone();

        QList<cv::Point> motionVectors =
                FunctionTools::newThreeStepsSearch(prevImg, curImg,
                                                   blockList,
                                                   FunctionTools::BlockMatchMethod::MSE);

        // 获取前后帧匹配块进行处理
        for (int j = 0; j < blockList.size(); j++) {
            cv::Rect curBlock = blockList[j];
            cv::Rect prevBlock = curBlock + motionVectors[j];
            if (prevBlock.x < 0 || prevBlock.y < 0) {
                qDebug() << j;
            }
            cv::Mat curBlockMat(curImg, curBlock);
            cv::Mat prevBlockMat(prevImg, prevBlock);
            // 计算块亮度均值，排除非闪烁区域
            double curMean = FunctionTools::getMatMean(curBlockMat);
            double curStdDev = FunctionTools::getMatStdDev(curBlockMat, curMean);
            double prevMean = FunctionTools::getMatMean(prevBlockMat);
            double prevStdDev = FunctionTools::getMatStdDev(prevBlockMat, prevMean);
            if (qAbs(curMean - prevMean) < FLICKER_LUMA_THREAD)
                continue;
            cv::Mat curE = FunctionTools::getMatrixExpectation(curBlockMat);
            cv::Mat prevE = FunctionTools::getMatrixExpectation(prevBlockMat);
            double alpha = curStdDev / prevStdDev;
            cv::Mat beta = curE - curStdDev * prevE / prevStdDev;
            curBlockMat = (curBlockMat - beta) / alpha;
            cv::Mat kernal = (cv::Mat_<double>(3,3)
                              << 1, 2, 1,
                                 2, 4, 2,
                                 1, 2, 1);
            kernal = kernal / 16;
            cv::filter2D(curBlockMat, curBlockMat, -1, kernal);
        }
//        cv::imshow("prev", prevImg);
//        cv::imshow("cur", curImg);
//        cv::waitKey(0);
        dstBuffer_->append(curImg);
        qDebug() << "process done. index->" + QString::number(i);
    }
    procDone();
    this->deleteLater();
}




