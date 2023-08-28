#ifndef FUNCTIONTOOLS_H
#define FUNCTIONTOOLS_H

#include <sequencebuffer.h>
#include <QObject>
#include <QJsonArray>
#include <QPoint>
#include <QVector2D>

class FunctionTools : public QObject
{
    Q_OBJECT
public:
    enum BlockMatchMethod {
        SAD,
        MAD,
        MSE,
        PSNR,
        SSIM
    };
    explicit FunctionTools(QObject *parent = nullptr);

    Q_INVOKABLE static QJsonArray getHistGray(SequenceBuffer* buffer);

    QList<QVector2D> threeStepsSearch(cv::Mat target, cv::Mat ref,
                                      int blockSize);

    ///
    /// \brief newThreeStepsSearch
    /// \param target       待搜索图像
    /// \param ref          匹配参考图像
    /// \param blockSize
    /// \param blockList
    /// \return
    ///
    static QList<cv::Point> newThreeStepsSearch(cv::Mat& target, cv::Mat& ref,
                                                QList<cv::Rect> blockList,
                                                BlockMatchMethod mode);

    static double getBlockMatchValue(cv::Mat &target, cv::Mat &ref, BlockMatchMethod mode);
    static double getMSE(cv::Mat &target, cv::Mat &ref);
    static double getMAD(cv::Mat &target, cv::Mat &ref);
    static cv::Mat getMatrixExpectation(cv::Mat &mat);
    static double getMatMean(cv::Mat &mat);
    static double getMatStdDev(cv::Mat &mat, double mean);

    static void printMat(cv::Mat &mat);
signals:

};

#endif // FUNCTIONTOOLS_H
