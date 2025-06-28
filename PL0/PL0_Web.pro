QT       += core gui widgets

# 检查Qt版本并添加WebEngine支持
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
qtHaveModule(webenginewidgets): QT += webenginewidgets webchannel

CONFIG += c++17

# 编译配置
TARGET = PL0_Compiler
TEMPLATE = app

# 定义宏以检测WebEngine支持
qtHaveModule(webenginewidgets) {
    DEFINES += QT_WEBENGINE_LIB
    message("WebEngine support enabled - Modern web interface available")
} else {
    message("WebEngine not available - Classic Qt interface only")
}

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# 源文件
SOURCES += \
    main_web.cpp \
    PL0.cpp \
    mainwindow.cpp

# 头文件
HEADERS += \
    mainwindow.h

# 如果有WebEngine支持，添加Web相关文件
qtHaveModule(webenginewidgets) {
    SOURCES += webmainwindow.cpp
    HEADERS += webmainwindow.h
}

# 资源文件
RESOURCES += \
    resources.qrc

# 其他文件
OTHER_FILES += \
    modern_style.qss \
    web_interface.html \
    美化方案说明.md \
    README_Web界面.md

# 安装配置
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# 部署Web文件
web_files.files = web_interface.html
web_files.path = $$OUT_PWD
INSTALLS += web_files

# 编译后复制Web文件到输出目录
win32 {
    PWD_WIN = $${PWD}
    OUT_PWD_WIN = $${OUT_PWD}
    
    PWD_WIN ~= s,/,\\,g
    OUT_PWD_WIN ~= s,/,\\,g
    
    copydata.commands = $(COPY_FILE) \"$$PWD_WIN\\web_interface.html\" \"$$OUT_PWD_WIN\\web_interface.html\"
    first.depends = $(first) copydata
    export(first.depends)
    export(copydata.commands)
    QMAKE_EXTRA_TARGETS += first copydata
}

# macOS和Linux
unix {
    copydata.commands = cp $$PWD/web_interface.html $$OUT_PWD/
    first.depends = $(first) copydata
    export(first.depends)
    export(copydata.commands)
    QMAKE_EXTRA_TARGETS += first copydata
}
