INCLUDEPATH += $$PWD/ffmpeg/include
LIBS += -L$$PWD/ffmpeg/bin -lavfilter-7 -lavutil-56 -lswresample-3 \
                              -lswscale-5 -lavformat-58 -lavdevice-58 \
                              -lpostproc-55 -lavcodec-58
