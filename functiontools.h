#ifndef FUNCTIONTOOLS_H
#define FUNCTIONTOOLS_H

#include <QObject>
#include <QJsonArray>
#include <sequencebuffer.h>

class FunctionTools : public QObject
{
    Q_OBJECT
public:
    explicit FunctionTools(QObject *parent = nullptr);

    Q_INVOKABLE static QJsonArray getHistGray(SequenceBuffer* buffer);
signals:

};

#endif // FUNCTIONTOOLS_H
