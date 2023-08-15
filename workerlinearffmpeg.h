#ifndef WORKERLINEARFFMPEG_H
#define WORKERLINEARFFMPEG_H

#include <QObject>
#include <QThread>


#include "sequencebuffer.h"

class WorkerLinearFFMPEG : public QThread
{
    Q_OBJECT
public:
    enum FactorType {
        ARITHMETIC_MEAN,        //算数平均数
        GEOMETRIC_MEAN,         //几何平均数
        HARMONIC_MEAN,          //调和平均数
        QUADRATIC_MEAN,         //均方根
        CUBIC_MEAN,             //立方平均数
        POWER_MEAN,             //幂平均
        MEDIAN                  //中位数
    };
    WorkerLinearFFMPEG(SequenceBuffer* srcBuffer, SequenceBuffer* dstBuffer,
                       FactorType mode,
                       QList<double>* srcLuminance, QList<double>* dstLuminance);
    void run() override;

signals:
    void procDone();
private:
    SequenceBuffer* srcBuffer_;
    SequenceBuffer* dstBuffer_;
    FactorType factorType_;
    QList<double>* srcLuminance_;
    QList<double>* dstLuminance_;
};

#endif // WORKERLINEARFFMPEG_H
