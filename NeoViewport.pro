QT += widgets opengl

TEMPLATE = lib
CONFIG += staticlib

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


INCLUDEPATH += $$PWD/3rd/imgui

SOURCES += \
    Neo/Managers/model_manager.cpp \
    Neo/Managers/texture_manager.cpp \
    Neo/OpenGL/camera_data.cpp \
    Neo/OpenGL/mesh_data.cpp \
    Neo/Viewport/neo_renderer.cpp \
    Neo/readers/models/obj_model_reader.cpp

HEADERS += \
    Neo/Managers/model_manager.h \
    Neo/Managers/texture_manager.h \
    Neo/OpenGL/camera_data.h \
    Neo/OpenGL/mesh_data.h \
    Neo/Viewport/neo_renderer.h \
    Neo/readers/models/obj_model_reader.h

# Default rules for deployment.
unix {
    target.path = $$[QT_INSTALL_PLUGINS]/generic
}
!isEmpty(target.path): INSTALLS += target

