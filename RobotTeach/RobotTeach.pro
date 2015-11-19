QT += core
QT -= gui

TARGET = RobotTeach
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app


unix:!macx: LIBS += -L$$PWD/../lib/ -ltld

INCLUDEPATH += $$PWD/../include
DEPENDPATH += $$PWD/../include

unix:!macx: PRE_TARGETDEPS += $$PWD/../lib/libtld.a




unix:!macx: LIBS += -L$$PWD/../lib/ -lferNN

INCLUDEPATH += $$PWD/../include
DEPENDPATH += $$PWD/../include

unix:!macx: PRE_TARGETDEPS += $$PWD/../lib/libferNN.a





unix:!macx: LIBS += -L$$PWD/../lib/ -lLKTracker

INCLUDEPATH += $$PWD/../include
DEPENDPATH += $$PWD/../include

unix:!macx: PRE_TARGETDEPS += $$PWD/../lib/libLKTracker.a





unix:!macx: LIBS += -L$$PWD/../lib/ -ltld_utils

INCLUDEPATH += $$PWD/../include
DEPENDPATH += $$PWD/../include

unix:!macx: PRE_TARGETDEPS += $$PWD/../lib/libtld_utils.a







SOURCES += \
    gaussian.cpp \
    serial.cpp \
    intelisu.cpp \
    maptwod.cpp

DISTFILES += \
    parameters.yml \
    RobotTeach.pro.user \
    ../build-RobotTeach-Desktop_Qt_5_5_1_GCC_32bit-Release/Makefile \
    CMakeLists.txt

HEADERS += \
    gaussian.h \
    serial.h \
    TLD.h \
    FerNNClassifier.h \
    LKTracker.h \
    tld_utils.h \
    maptwod.h

INCLUDEPATH +=/usr/local/include/opencv\
/usr/local/include

LIBS +=/usr/local/lib/libopencv_calib3d.so\
/usr/local/lib/libopencv_contrib.so\
/usr/local/lib/libopencv_core.so\
/usr/local/lib/libopencv_features2d.so\
/usr/local/lib/libopencv_flann.so\
/usr/local/lib/libopencv_gpu.so\
/usr/local/lib/libopencv_highgui.so\
/usr/local/lib/libopencv_imgproc.so\
/usr/local/lib/libopencv_legacy.so\
#/usr/local/lib/libopencv_ml.so\
#/usr/local/lib/libopencv_nonfree.so\
#/usr/local/lib/libopencv_objdetect.so\
#/usr/local/lib/libopencv_ocl.so\
#/usr/local/lib/libopencv_photo.so\
#/usr/local/lib/libopencv_stitching.so\
#/usr/local/lib/libopencv_superres.so\
#/usr/local/lib/libopencv_ts.a\
/usr/local/lib/libopencv_video.so\
#    libferNN.a \
#    libLKTracker.a \
#    libtld.a \
#    libtld_utils.a
#/usr/local/lib/libopencv_videostab.so\

