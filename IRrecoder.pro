TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.c

#TIFF
#INCLUDEPATH += ../lib/tiff-4.0.6/libtiff  ../lib\tiff-4.0.6-build\libtiff
#LIBS += -LD:\work\lib\tiff-4.0.6-build\libtiff -ltiffd

#SDK
INCLUDEPATH += Include
LIBS += -LD:\work\IRrecoder\Lib -lxeneth64 -lShlwapi


