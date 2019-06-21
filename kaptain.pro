CONFIG += qt

QT += qt3support

HEADERS += communication.h global.h grammar.h intermediate.h kaptain.h \
           lexer.h process.h tools.h

SOURCES += communication.cpp grammar.cpp intermediate.cpp kaptain.cpp \
           main.cpp process.cpp tools.cpp 

LEXSOURCES += lexer.lpp
YACCSOURCES += parser.ypp

QMAKE_LEX = flex
QMAKE_YACC = bison
QMAKE_YACCFLAGS = -d -o parser.tab.c
QMAKE_YACC_HEADER = parser.tab.h
QMAKE_YACC_SOURCE = parser.tab.c

unix:!macx { # installation on Unix-ish platforms
 isEmpty(PREFIX):PREFIX = /usr/local
 isEmpty(BIN_DIR):BIN_DIR = $$PREFIX/bin
 isEmpty(DATA_DIR):DATA_DIR = $$PREFIX/share
 isEmpty(DOCS_DIR):DOCS_DIR = $$DATA_DIR/doc/kaptain
 isEmpty(MAN_DIR):MAN_DIR = $$DATA_DIR/man/man1

 target.path = $$BIN_DIR
 documentation.files = COPYING README doc/*
 documentation.path = $$DOCS_DIR
 man.files = man/kaptain.1
 man.path = $$MAN_DIR
 examples.files = examples/*
 examples.path=$$DATA_DIR/kaptain
 INSTALLS = target documentation examples man
}
