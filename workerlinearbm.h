#ifndef WORKERLINEARBM_H
#define WORKERLINEARBM_H

#include <QObject>
#include <QThread>
#include <sequencebuffer.h>

class WorkerLinearBM : public QThread
{
    Q_OBJECT
public:
    enum SearchMethod {
        TSS,        // Three Steps Search
        NTSS        // New Three Steps Search
    };

    WorkerLinearBM(SequenceBuffer* srcBuffer, SequenceBuffer* dstBuffer,
                   SearchMethod mode);
    void run() override;
signals:
    void procDone();
private:
    SequenceBuffer* srcBuffer_;
    SequenceBuffer* dstBuffer_;
    SearchMethod mode_;
};

#endif // WORKERLINEARBM_H
