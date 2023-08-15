#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "deflickers.h"
#include "imageloader.h"
#include "sequencebuffer.h"
#include "imagecanvas.h"
#include "functiontools.h"

extern "C"
{
#include <libavformat/avformat.h>
//#include "libavfilter/avfiltergraph.h"
#include "libavfilter/buffersink.h"
#include "libavfilter/buffersrc.h"
#include "libswscale/swscale.h"
#include "libavdevice/avdevice.h"
#include "libavutil/pixfmt.h"
#include "libavutil/opt.h"
#include "libavcodec/avfft.h"
#include "libavutil/imgutils.h"
#include "libswresample/swresample.h"
}

using namespace Deflickers;

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);


    QApplication app(argc, argv);

    qmlRegisterType<ImageCanvas>("ImageCanvas",1,0,"ImageCanvas");

    SequenceBuffer* srcBuffer = new SequenceBuffer();
    SequenceBuffer* dstBuffer = new SequenceBuffer();
    ImageLoader* imageLoader = new ImageLoader();
    imageLoader->setSrcBuffer(srcBuffer);
    GlobalDeflicker* globalDeflicker = new GlobalDeflicker(srcBuffer, dstBuffer);
    QObject::connect(imageLoader, SIGNAL(sequenceUpdate()),
                     globalDeflicker, SLOT(onSequenceUpdate()));
    FunctionTools* funcTools = new FunctionTools();


    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("srcBuffer", srcBuffer);
    engine.rootContext()->setContextProperty("dstBuffer", dstBuffer);
    engine.rootContext()->setContextProperty("imageLoader", imageLoader);
    engine.rootContext()->setContextProperty("globalDeflicker", globalDeflicker);
    engine.rootContext()->setContextProperty("funcTools", funcTools);

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
