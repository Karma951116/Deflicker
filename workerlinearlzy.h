#ifndef WORKERLINEARLZY_H
#define WORKERLINEARLZY_H

#include <QObject>
#include <QThread>

#include "sequencebuffer.h"

class WorkerLinearLzy : public QThread
{
    Q_OBJECT
public:
    enum Eigenvalue {
        AVERAGE_GRAYSCALE,
        STANDARD_DEVIATION
    };
    WorkerLinearLzy(SequenceBuffer* srcBuffer, SequenceBuffer* dstBuffer,
                    Eigenvalue mode,
                    QList<double>* srcLuminance, QList<double>* dstLuminance);
    void run() override;
signals:
    void procDone();
private:
    SequenceBuffer* srcBuffer_;
    SequenceBuffer* dstBuffer_;
    Eigenvalue mode_;
    QList<double>* srcLuminance_;
    QList<double>* dstLuminance_;
};

#endif // WORKERLINEARLZY_H
