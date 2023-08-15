#ifndef DEFLICKERS_H
#define DEFLICKERS_H

#include <QObject>
#include <QJsonArray>
#include <QJsonObject>
#include "sequencebuffer.h"

namespace Deflickers {

enum Range {
    GLOBAL,
    LOCAL
};

enum LocalMethod {
};

class GlobalDeflicker: public QObject {
    Q_OBJECT

public:
    enum GlobalMethod {
        LINEAR_LZY,
        LINEAR_FFMPEG
    };
    explicit GlobalDeflicker(SequenceBuffer* srcBuffer, SequenceBuffer* dstBuffer);
    Q_INVOKABLE void process();
    Q_INVOKABLE void setType(const GlobalMethod &value);

    Q_INVOKABLE QJsonArray srcLuminance() const;

    Q_INVOKABLE QJsonArray dstLuminance() const;

    Q_INVOKABLE QJsonArray srcHist(int index) const;

    Q_INVOKABLE QJsonArray dstHist(int index) const;

signals:
    void calcDstMeanDone();
private:
    SequenceBuffer* srcBuffer_;
    SequenceBuffer* dstBuffer_;
    QThread* curWork_;
    GlobalMethod type_;
    QList<double> srcLuminance_;
    QList<double> dstLuminance_;
    QJsonArray srcHist_;
    QJsonArray dstHist_;

    QJsonArray getHist(cv::Mat mat);

public slots:
    void onFinished();
    void onSequenceUpdate();
    void onProcDone();
};
class LocalDeflicker: public QObject {

    Q_OBJECT
    void process();
};

}
#endif // DEFLICKERS_H
