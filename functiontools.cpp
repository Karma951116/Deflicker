#include "functiontools.h"
#include <QJsonObject>
#include <opencv2/opencv.hpp>
#include <qmath.h>
#include <QDebug>

FunctionTools::FunctionTools(QObject *parent) : QObject(parent)
{

}

QJsonArray FunctionTools::getHistGray(SequenceBuffer *buffer)
{
    if (buffer->nbFrames() <= 0) {
        return QJsonArray();
    }
    QVector<unsigned short> hist = QVector<unsigned short>(65535);
    for (int i = 0; i < 65535; i++) {
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


QList<cv::Point> FunctionTools::newThreeStepsSearch(cv::Mat& target, cv::Mat& ref,
                                                    QList<cv::Rect> blockList,
                                                    BlockMatchMethod mode)
{
    QList<cv::Point> motionVectors;
    for (cv::Rect rect: blockList) {
        // 原始块
        cv::Rect refRect(rect);
        cv::Mat refBlockImg(ref, refRect);
        // 寻找MBD(minimum block distortion)
        double minMatchValue = -1;
        cv::Point mVector;
        // 计算原点及步长为1的9个块
        for (int offsetX = -1; offsetX < 1; offsetX++) {
            for (int offsetY = -1; offsetY < 1; offsetY++) {
                cv::Rect tgtRect = refRect + cv::Point(offsetX, offsetY);
                // 检查矩形区域是否有效
                if (tgtRect.x < 0 || tgtRect.y < 0) {
                    continue;
                }
                if (tgtRect.br().x > ref.cols - 1 ||
                        tgtRect.br().y > ref.rows - 1) {
                    continue;
                }
                cv::Mat prevBlockImg(target, tgtRect);

                // 计算匹配值
                double curValue = getBlockMatchValue(prevBlockImg, refBlockImg, mode);
                if (minMatchValue == -1 || curValue < minMatchValue) {
                    minMatchValue = curValue;
                    mVector = cv::Point(offsetX, offsetY);
                }
                else {
                    continue;
                }
            }
        }

        // 计算步长为4的8个块
        for (int offsetX = -4; offsetX < 4; offsetX += 4) {
            for (int offsetY = -4; offsetY < 4; offsetY += 4) {
                if (offsetX == 0 && offsetY == 0) continue;
                cv::Rect tgtRect = refRect + cv::Point(offsetX, offsetY);
                // 检查矩形区域是否有效
                if (tgtRect.x < 0 || tgtRect.y < 0) {
                    continue;
                }
                if (tgtRect.br().x > ref.cols - 1 ||
                        tgtRect.br().y > ref.rows - 1) {
                    continue;
                }
                cv::Mat prevBlockImg(target, tgtRect);

                // 计算匹配值
                double curValue = getBlockMatchValue(prevBlockImg, refBlockImg, mode);
                if (minMatchValue == -1 || curValue < minMatchValue) {
                    minMatchValue = curValue;
                    mVector = cv::Point(offsetX, offsetY);
                }
                else {
                    continue;
                }
            }
        }
        // 若MBD为原点
        if (mVector.x == 0 && mVector.y == 0) {
            motionVectors.append(mVector);
        }
        // 若为步长1的8个点
        else if (qAbs(mVector.x) == 1 ||
                 qAbs(mVector.y) == 1) {
            // 以匹配点为中心搜索
            cv::Rect tgtRect = refRect + mVector;
            for (int offsetX = -1; offsetX < 1; offsetX++) {
                for (int offsetY = -1; offsetY < 1; offsetY++) {
                    if (offsetX == 0 && offsetY == 0) continue;
                    cv::Rect subTgtRect = tgtRect + cv::Point(offsetX, offsetY);
                    // 检查矩形区域是否有效
                    if (subTgtRect.x < 0 || subTgtRect.y < 0) {
                        continue;
                    }
                    if (subTgtRect.br().x > ref.cols - 1 ||
                            subTgtRect.br().y > ref.rows - 1) {
                        continue;
                    }
                    cv::Mat prevBlockImg(target, subTgtRect);

                    // 计算匹配值
                    double curValue = getBlockMatchValue(prevBlockImg, refBlockImg, mode);
                    if (minMatchValue == -1 || curValue < minMatchValue) {
                        minMatchValue = curValue;
                        mVector += cv::Point(offsetX, offsetY);
                    }
                    else {
                        continue;
                    }
                }
            }
            motionVectors.append(mVector);
        }
        // 按三步搜索法继续
        else {
            cv::Rect tgtRect = refRect + mVector;
            // 以步长为2搜索
            for (int offsetX = -2; offsetX < 2; offsetX += 2) {
                for (int offsetY = -2; offsetY < 2; offsetY += 2) {
                    if (offsetX == 0 && offsetY == 0) continue;
                    cv::Rect subTgtRect = tgtRect + cv::Point(offsetX, offsetY);
                    // 检查矩形区域是否有效
                    if (subTgtRect.x < 0 || subTgtRect.y < 0) {
                        continue;
                    }
                    if (subTgtRect.br().x > ref.cols - 1 ||
                            subTgtRect.br().y > ref.rows - 1) {
                        continue;
                    }
                    cv::Mat prevBlockImg(target, subTgtRect);

                    // 计算匹配值
                    double curValue = getBlockMatchValue(prevBlockImg, refBlockImg, mode);
                    if (minMatchValue == -1 || curValue < minMatchValue) {
                        minMatchValue = curValue;
                        mVector += cv::Point(offsetX, offsetY);
                    }
                    else {
                        continue;
                    }
                }
            }
            //以步长为1搜索
            tgtRect = tgtRect + mVector;
            for (int offsetX = -1; offsetX < 1; offsetX++) {
                for (int offsetY = -1; offsetY < 1; offsetY++) {
                    if (offsetX == 0 && offsetY == 0) continue;
                    cv::Rect subTgtRect = tgtRect + cv::Point(offsetX, offsetY);
                    // 检查矩形区域是否有效
                    if (subTgtRect.x < 0 || subTgtRect.y < 0) {
                        continue;
                    }
                    if (subTgtRect.br().x > ref.cols - 1 ||
                            subTgtRect.br().y > ref.rows - 1) {
                        continue;
                    }
                    cv::Mat prevBlockImg(target, subTgtRect);

                    // 计算匹配值
                    double curValue = getBlockMatchValue(prevBlockImg, refBlockImg, mode);
                    if (minMatchValue == -1 || curValue < minMatchValue) {
                        minMatchValue = curValue;
                        mVector += cv::Point(offsetX, offsetY);
                    }
                    else {
                        continue;
                    }
                }
            }
            motionVectors.append(mVector);
        }
    }
    return motionVectors;
}

double FunctionTools::getBlockMatchValue(cv::Mat &target, cv::Mat &ref, FunctionTools::BlockMatchMethod mode)
{
    double value = 0;
    switch (mode) {
    case MSE:
        value = getMSE(target, ref);
        break;
    case MAD:
        value = getMAD(target, ref);
        break;
    case SAD:
    case PSNR:
    case SSIM:
        break;
    }
    return value;
}

///
/// \brief FunctionTools::getMSE
/// \param src 搜索图
/// \param dst 模板图
/// \return
///
/// 按单通道计算
double FunctionTools::getMSE(cv::Mat &target, cv::Mat &ref)
{
    double mse = 0.0;
    for (int i = 0; i < target.rows; i++)
    {
        for (int j = 0; j < target.cols; j++)
        {
            unsigned short srcValue = target.at<cv::Vec3w>(i, j)[0];
            unsigned short dstValue = ref.at<cv::Vec3w>(i, j)[0];
            mse += qPow(qAbs(srcValue - dstValue), 2);
        }
    }
    mse=mse / (target.rows * target.cols);
    return mse;
}

double FunctionTools::getMAD(cv::Mat &target, cv::Mat &ref)
{
    double mad = 0.0;
    for (int i = 0; i < target.rows; i++)
    {
        for (int j = 0; j < target.cols; j++)
        {
            cv::Vec3w srcValue = target.at<cv::Vec3w>(i, j);
            cv::Vec3w dstValue = ref.at<cv::Vec3w>(i, j);
            mad += qAbs(srcValue[0] - dstValue[0]);
        }
    }
    mad = mad / (target.rows * target.cols);
    return mad;
}

cv::Mat FunctionTools::getMatrixExpectation(cv::Mat &mat)
{
    int elements = mat.rows * mat.cols;
    cv::Mat ret = mat / elements;
    return ret;
}

double FunctionTools::getMatMean(cv::Mat &mat)
{
    double sum = 0;
    for (int row = 0; row < mat.rows; row++) {
        for(int col = 0; col < mat.cols; col++) {
            sum += mat.at<cv::Vec3w>(row, col)[0];
        }
    }
    sum /= mat.rows * mat.cols;
    return sum;
}

double FunctionTools::getMatStdDev(cv::Mat &mat, double mean)
{
    double stdDev = 0;
    for (int row = 0; row < mat.rows; row++) {
        for(int col = 0; col < mat.cols; col++) {
            double luma = mat.at<cv::Vec3w>(row, col)[0];
            stdDev += qPow(luma - mean, 2);
        }
    }
    stdDev /= mat.rows * mat.cols;
    stdDev = qSqrt(stdDev);
    return stdDev;
}

void FunctionTools::printMat(cv::Mat &mat)
{
    qDebug() << "\n";
    for (int row = 0; row < mat.rows; row++) {
        QList<unsigned short> rowData;
        for(int col = 0; col < mat.cols; col++) {
            rowData.append(mat.at<cv::Vec3w>(row, col)[0]);
        }
        qDebug() << rowData;
    }
    qDebug() << "\n";
}
