QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++14

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    LoginDialog.cpp \
    aboutdialog.cpp \
    main.cpp \
    mainwindow.cpp \
    registerdialog.cpp \
    updatebookdialog.cpp \
    updateuserdialog.cpp \
    userinfodialog.cpp \
    veritydialog.cpp

HEADERS += \
    BasicClass/Book.h \
    BasicClass/BookTable.h \
    BasicClass/BorrowRecord.h \
    BasicClass/BorrowTable.h \
    BasicClass/Manage.h \
    BasicClass/Password.h \
    BasicClass/Telephone.h \
    BasicClass/User.h \
    BasicClass/UserTable.h \
    LoginDialog.h \
    aboutdialog.h \
    common.h \
    mainwindow.h \
    registerdialog.h \
    updatebookdialog.h \
    updateuserdialog.h \
    userinfodialog.h \
    veritydialog.h

FORMS += \
    LoginDialog.ui \
    aboutdialog.ui \
    mainwindow.ui \
    registerdialog.ui \
    updatebookdialog.ui \
    updateuserdialog.ui \
    userinfodialog.ui \
    veritydialog.ui

TRANSLATIONS += \
    BookInfoSystem_yue_CN.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    image.qrc
