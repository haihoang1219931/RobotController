TEMPLATE = app
CONFIG += c++11 no_keywords console

QT += qml quick qml
CONFIG += c++11

#unix:!macx: INCLUDEPATH += /usr/local/include/opencv4
#unix:!macx: DEPENDPATH += /usr/local/include/opencv4
#unix:!macx: LIBS += -L/usr/local/lib/  \
#    -lopencv_objdetect \
#    -lopencv_shape -lopencv_stitching -lopencv_superres -lopencv_features2d -lopencv_calib3d \
#    -lopencv_videostab \
#    -lopencv_video \
#    -lopencv_core \
#    -lopencv_highgui \
#    -lopencv_imgcodecs \
#    -lopencv_imgproc \
#    -lopencv_videoio

#win32: LIBS += -lpthread

HEADERS += \
    SAL/Robot.h \
    SAL/Button.h \
    SAL/ApplicationController.h \
    SAL/CommandReader.h \
    RobotSim/ApplicationSim.h \
    RobotSim/MainProcess.h \
    RobotSim/VideoDisplay/VideoRender.h \
    RobotSim/VideoDisplay/I420Render.h

SOURCES += \
    main.cpp \
    SAL/Robot.cpp \
    SAL/Button.cpp \
    SAL/ApplicationController.cpp \
    SAL/CommandReader.cpp \
    RobotSim/ApplicationSim.cpp \
    RobotSim/MainProcess.cpp \
    RobotSim/VideoDisplay/VideoRender.cpp \
    RobotSim/VideoDisplay/I420Render.cpp

RESOURCES += \
    RobotSim/qml.qrc
QML_DESIGNER_IMPORT_PATH =
