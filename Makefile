#############################################################################
# Makefile for building: kaptain
# Generated by qmake (2.01a) (Qt 4.6.2) on: Mon Feb 28 10:51:28 2011
# Project:  kaptain.pro
# Template: app
# Command: /usr/bin/qmake -unix -o Makefile kaptain.pro
#############################################################################

####### Compiler, tools and options

CC            = gcc
CXX           = g++
DEFINES       = -DQT_NO_DEBUG -DQT_QT3SUPPORT_LIB -DQT3_SUPPORT -DQT_GUI_LIB -DQT_CORE_LIB -DQT_SHARED
CFLAGS        = -pipe -O2 -D_REENTRANT -Wall -W $(DEFINES)
CXXFLAGS      = -pipe -O2 -D_REENTRANT -Wall -W $(DEFINES)
INCPATH       = -I/usr/share/qt4/mkspecs/linux-g++ -I. -I/usr/include/qt4/QtCore -I/usr/include/qt4/QtGui -I/usr/include/qt4/Qt3Support -I/usr/include/qt4 -I.
LINK          = g++
LFLAGS        = -Wl,-O1
LIBS          = $(SUBLIBS)  -L/usr/lib -lQt3Support -lQtGui -lQtCore -lpthread 
AR            = ar cqs
RANLIB        = 
QMAKE         = /usr/bin/qmake
TAR           = tar -cf
COMPRESS      = gzip -9f
COPY          = cp -f
SED           = sed
COPY_FILE     = $(COPY)
COPY_DIR      = $(COPY) -r
STRIP         = strip
INSTALL_FILE  = install -m 644 -p
INSTALL_DIR   = $(COPY_DIR)
INSTALL_PROGRAM = install -m 755 -p
DEL_FILE      = rm -f
SYMLINK       = ln -f -s
DEL_DIR       = rmdir
MOVE          = mv -f
CHK_DIR_EXISTS= test -d
MKDIR         = mkdir -p

####### Output directory

OBJECTS_DIR   = ./

####### Files

SOURCES       = communication.cpp \
		grammar.cpp \
		intermediate.cpp \
		kaptain.cpp \
		main.cpp \
		process.cpp \
		tools.cpp moc_communication.cpp \
		moc_kaptain.cpp \
		parser_yacc.cpp \
		lexer_lex.cpp
OBJECTS       = communication.o \
		grammar.o \
		intermediate.o \
		kaptain.o \
		main.o \
		process.o \
		tools.o \
		moc_communication.o \
		moc_kaptain.o \
		parser_yacc.o \
		lexer_lex.o
DIST          = /usr/share/qt4/mkspecs/common/g++.conf \
		/usr/share/qt4/mkspecs/common/unix.conf \
		/usr/share/qt4/mkspecs/common/linux.conf \
		/usr/share/qt4/mkspecs/qconfig.pri \
		/usr/share/qt4/mkspecs/features/qt_functions.prf \
		/usr/share/qt4/mkspecs/features/qt_config.prf \
		/usr/share/qt4/mkspecs/features/exclusive_builds.prf \
		/usr/share/qt4/mkspecs/features/default_pre.prf \
		/usr/share/qt4/mkspecs/features/release.prf \
		/usr/share/qt4/mkspecs/features/default_post.prf \
		/usr/share/qt4/mkspecs/features/qt.prf \
		/usr/share/qt4/mkspecs/features/unix/thread.prf \
		/usr/share/qt4/mkspecs/features/moc.prf \
		/usr/share/qt4/mkspecs/features/warn_on.prf \
		/usr/share/qt4/mkspecs/features/resources.prf \
		/usr/share/qt4/mkspecs/features/uic.prf \
		/usr/share/qt4/mkspecs/features/yacc.prf \
		/usr/share/qt4/mkspecs/features/lex.prf \
		/usr/share/qt4/mkspecs/features/include_source_dir.prf \
		kaptain.pro
QMAKE_TARGET  = kaptain
DESTDIR       = 
TARGET        = kaptain

first: all
####### Implicit rules

.SUFFIXES: .o .c .cpp .cc .cxx .C

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cc.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cxx.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.C.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.c.o:
	$(CC) -c $(CFLAGS) $(INCPATH) -o "$@" "$<"

####### Build rules

all: Makefile $(TARGET)

$(TARGET):  $(OBJECTS)  
	$(LINK) $(LFLAGS) -o $(TARGET) $(OBJECTS) $(OBJCOMP) $(LIBS)

Makefile: kaptain.pro  /usr/share/qt4/mkspecs/linux-g++/qmake.conf /usr/share/qt4/mkspecs/common/g++.conf \
		/usr/share/qt4/mkspecs/common/unix.conf \
		/usr/share/qt4/mkspecs/common/linux.conf \
		/usr/share/qt4/mkspecs/qconfig.pri \
		/usr/share/qt4/mkspecs/features/qt_functions.prf \
		/usr/share/qt4/mkspecs/features/qt_config.prf \
		/usr/share/qt4/mkspecs/features/exclusive_builds.prf \
		/usr/share/qt4/mkspecs/features/default_pre.prf \
		/usr/share/qt4/mkspecs/features/release.prf \
		/usr/share/qt4/mkspecs/features/default_post.prf \
		/usr/share/qt4/mkspecs/features/qt.prf \
		/usr/share/qt4/mkspecs/features/unix/thread.prf \
		/usr/share/qt4/mkspecs/features/moc.prf \
		/usr/share/qt4/mkspecs/features/warn_on.prf \
		/usr/share/qt4/mkspecs/features/resources.prf \
		/usr/share/qt4/mkspecs/features/uic.prf \
		/usr/share/qt4/mkspecs/features/yacc.prf \
		/usr/share/qt4/mkspecs/features/lex.prf \
		/usr/share/qt4/mkspecs/features/include_source_dir.prf \
		/usr/lib/libQt3Support.prl \
		/usr/lib/libQtGui.prl \
		/usr/lib/libQtCore.prl
	$(QMAKE) -unix -o Makefile kaptain.pro
/usr/share/qt4/mkspecs/common/g++.conf:
/usr/share/qt4/mkspecs/common/unix.conf:
/usr/share/qt4/mkspecs/common/linux.conf:
/usr/share/qt4/mkspecs/qconfig.pri:
/usr/share/qt4/mkspecs/features/qt_functions.prf:
/usr/share/qt4/mkspecs/features/qt_config.prf:
/usr/share/qt4/mkspecs/features/exclusive_builds.prf:
/usr/share/qt4/mkspecs/features/default_pre.prf:
/usr/share/qt4/mkspecs/features/release.prf:
/usr/share/qt4/mkspecs/features/default_post.prf:
/usr/share/qt4/mkspecs/features/qt.prf:
/usr/share/qt4/mkspecs/features/unix/thread.prf:
/usr/share/qt4/mkspecs/features/moc.prf:
/usr/share/qt4/mkspecs/features/warn_on.prf:
/usr/share/qt4/mkspecs/features/resources.prf:
/usr/share/qt4/mkspecs/features/uic.prf:
/usr/share/qt4/mkspecs/features/yacc.prf:
/usr/share/qt4/mkspecs/features/lex.prf:
/usr/share/qt4/mkspecs/features/include_source_dir.prf:
/usr/lib/libQt3Support.prl:
/usr/lib/libQtGui.prl:
/usr/lib/libQtCore.prl:
qmake:  FORCE
	@$(QMAKE) -unix -o Makefile kaptain.pro

dist: 
	@$(CHK_DIR_EXISTS) .tmp/kaptain1.0.0 || $(MKDIR) .tmp/kaptain1.0.0 
	$(COPY_FILE) --parents $(SOURCES) $(DIST) .tmp/kaptain1.0.0/ && $(COPY_FILE) --parents communication.h global.h grammar.h intermediate.h kaptain.h lexer.h process.h tools.h .tmp/kaptain1.0.0/ && $(COPY_FILE) --parents communication.cpp grammar.cpp intermediate.cpp kaptain.cpp main.cpp process.cpp tools.cpp .tmp/kaptain1.0.0/ && $(COPY_FILE) --parents parser.ypp .tmp/kaptain1.0.0/ && $(COPY_FILE) --parents parser.ypp .tmp/kaptain1.0.0/ && $(COPY_FILE) --parents lexer.lpp .tmp/kaptain1.0.0/ && (cd `dirname .tmp/kaptain1.0.0` && $(TAR) kaptain1.0.0.tar kaptain1.0.0 && $(COMPRESS) kaptain1.0.0.tar) && $(MOVE) `dirname .tmp/kaptain1.0.0`/kaptain1.0.0.tar.gz . && $(DEL_FILE) -r .tmp/kaptain1.0.0


clean:compiler_clean 
	-$(DEL_FILE) $(OBJECTS)
	-$(DEL_FILE) *~ core *.core


####### Sub-libraries

distclean: clean
	-$(DEL_FILE) $(TARGET) 
	-$(DEL_FILE) Makefile


mocclean: compiler_moc_header_clean compiler_moc_source_clean

mocables: compiler_moc_header_make_all compiler_moc_source_make_all

compiler_moc_header_make_all: moc_communication.cpp moc_kaptain.cpp
compiler_moc_header_clean:
	-$(DEL_FILE) moc_communication.cpp moc_kaptain.cpp
moc_communication.cpp: communication.h
	/usr/bin/moc-qt4 $(DEFINES) $(INCPATH) communication.h -o moc_communication.cpp

moc_kaptain.cpp: kaptain.h
	/usr/bin/moc-qt4 $(DEFINES) $(INCPATH) kaptain.h -o moc_kaptain.cpp

compiler_rcc_make_all:
compiler_rcc_clean:
compiler_image_collection_make_all: qmake_image_collection.cpp
compiler_image_collection_clean:
	-$(DEL_FILE) qmake_image_collection.cpp
compiler_moc_source_make_all:
compiler_moc_source_clean:
compiler_uic_make_all:
compiler_uic_clean:
compiler_yacc_decl_make_all: parser_yacc.h
compiler_yacc_decl_clean:
	-$(DEL_FILE) parser_yacc.h
parser_yacc.h: parser.ypp
	bison -d -o parser.tab.c -p parser -b parser parser.ypp
	 rm -f parser_yacc.h parser_yacc.cpp
	 mv -f parser.tab.h parser_yacc.h
	 mv -f parser.tab.c parser_yacc.cpp
	

compiler_yacc_impl_make_all: parser_yacc.cpp
compiler_yacc_impl_clean:
	-$(DEL_FILE) parser_yacc.cpp
parser_yacc.cpp: parser.ypp \
		parser_yacc.h
	


compiler_lex_make_all: lexer_lex.cpp
compiler_lex_clean:
	-$(DEL_FILE) lexer_lex.cpp
lexer_lex.cpp: lexer.lpp
	flex -Plexer lexer.lpp
	 rm -f lexer_lex.cpp
	 mv -f lex.lexer.c lexer_lex.cpp
	

compiler_clean: compiler_moc_header_clean compiler_yacc_decl_clean compiler_yacc_impl_clean compiler_lex_clean 

####### Compile

communication.o: communication.cpp global.h \
		tools.h \
		communication.h \
		grammar.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o communication.o communication.cpp

grammar.o: grammar.cpp global.h \
		grammar.h \
		communication.h \
		tools.h \
		intermediate.h \
		kaptain.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o grammar.o grammar.cpp

intermediate.o: intermediate.cpp intermediate.h \
		grammar.h \
		kaptain.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o intermediate.o intermediate.cpp

kaptain.o: kaptain.cpp tools.h \
		process.h \
		kaptain.h \
		intermediate.h \
		grammar.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o kaptain.o kaptain.cpp

main.o: main.cpp global.h \
		tools.h \
		communication.h \
		process.h \
		grammar.h \
		intermediate.h \
		kaptain.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o main.o main.cpp

process.o: process.cpp process.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o process.o process.cpp

tools.o: tools.cpp tools.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o tools.o tools.cpp

moc_communication.o: moc_communication.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_communication.o moc_communication.cpp

moc_kaptain.o: moc_kaptain.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o moc_kaptain.o moc_kaptain.cpp

parser_yacc.o: parser_yacc.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o parser_yacc.o parser_yacc.cpp

lexer_lex.o: lexer_lex.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o lexer_lex.o lexer_lex.cpp

####### Install

install_target: first FORCE
	@$(CHK_DIR_EXISTS) $(INSTALL_ROOT)/usr/local/bin/ || $(MKDIR) $(INSTALL_ROOT)/usr/local/bin/ 
	-$(INSTALL_PROGRAM) "$(QMAKE_TARGET)" "$(INSTALL_ROOT)/usr/local/bin/$(QMAKE_TARGET)"
	-$(STRIP) "$(INSTALL_ROOT)/usr/local/bin/$(QMAKE_TARGET)"

uninstall_target:  FORCE
	-$(DEL_FILE) "$(INSTALL_ROOT)/usr/local/bin/$(QMAKE_TARGET)"
	-$(DEL_DIR) $(INSTALL_ROOT)/usr/local/bin/ 


install_documentation: first FORCE
	@$(CHK_DIR_EXISTS) $(INSTALL_ROOT)/usr/local/share/doc/kaptain/ || $(MKDIR) $(INSTALL_ROOT)/usr/local/share/doc/kaptain/ 
	-$(INSTALL_FILE) /home/zsolt/kaptain/kaptain-0.73/COPYING $(INSTALL_ROOT)/usr/local/share/doc/kaptain/
	-$(INSTALL_FILE) /home/zsolt/kaptain/kaptain-0.73/README $(INSTALL_ROOT)/usr/local/share/doc/kaptain/
	-$(INSTALL_FILE) /home/zsolt/kaptain/kaptain-0.73/doc/crypt.gif $(INSTALL_ROOT)/usr/local/share/doc/kaptain/
	-$(INSTALL_FILE) /home/zsolt/kaptain/kaptain-0.73/doc/database.tgz $(INSTALL_ROOT)/usr/local/share/doc/kaptain/
	-$(INSTALL_FILE) /home/zsolt/kaptain/kaptain-0.73/doc/fig1.png $(INSTALL_ROOT)/usr/local/share/doc/kaptain/
	-$(INSTALL_FILE) /home/zsolt/kaptain/kaptain-0.73/doc/kaptain.html $(INSTALL_ROOT)/usr/local/share/doc/kaptain/
	-$(INSTALL_FILE) /home/zsolt/kaptain/kaptain-0.73/doc/kaptain.info $(INSTALL_ROOT)/usr/local/share/doc/kaptain/
	-$(INSTALL_FILE) /home/zsolt/kaptain/kaptain-0.73/doc/kaptain.texi $(INSTALL_ROOT)/usr/local/share/doc/kaptain/
	-$(INSTALL_FILE) /home/zsolt/kaptain/kaptain-0.73/doc/record.gif $(INSTALL_ROOT)/usr/local/share/doc/kaptain/
	-$(INSTALL_FILE) /home/zsolt/kaptain/kaptain-0.73/doc/tar.gif $(INSTALL_ROOT)/usr/local/share/doc/kaptain/
	-$(INSTALL_FILE) /home/zsolt/kaptain/kaptain-0.73/doc/tutorial.tgz $(INSTALL_ROOT)/usr/local/share/doc/kaptain/


uninstall_documentation:  FORCE
	-$(DEL_FILE) -r $(INSTALL_ROOT)/usr/local/share/doc/kaptain/COPYING 
	 -$(DEL_FILE) -r $(INSTALL_ROOT)/usr/local/share/doc/kaptain/README 
	 -$(DEL_FILE) -r $(INSTALL_ROOT)/usr/local/share/doc/kaptain/crypt.gif 
	 -$(DEL_FILE) -r $(INSTALL_ROOT)/usr/local/share/doc/kaptain/database.tgz 
	 -$(DEL_FILE) -r $(INSTALL_ROOT)/usr/local/share/doc/kaptain/fig1.png 
	 -$(DEL_FILE) -r $(INSTALL_ROOT)/usr/local/share/doc/kaptain/kaptain.html 
	 -$(DEL_FILE) -r $(INSTALL_ROOT)/usr/local/share/doc/kaptain/kaptain.info 
	 -$(DEL_FILE) -r $(INSTALL_ROOT)/usr/local/share/doc/kaptain/kaptain.texi 
	 -$(DEL_FILE) -r $(INSTALL_ROOT)/usr/local/share/doc/kaptain/record.gif 
	 -$(DEL_FILE) -r $(INSTALL_ROOT)/usr/local/share/doc/kaptain/tar.gif 
	 -$(DEL_FILE) -r $(INSTALL_ROOT)/usr/local/share/doc/kaptain/tutorial.tgz
	-$(DEL_DIR) $(INSTALL_ROOT)/usr/local/share/doc/kaptain/ 


install_examples: first FORCE
	@$(CHK_DIR_EXISTS) $(INSTALL_ROOT)/usr/local/share/kaptain/ || $(MKDIR) $(INSTALL_ROOT)/usr/local/share/kaptain/ 
	-$(INSTALL_FILE) /home/zsolt/kaptain/kaptain-0.73/examples/arping.kaptn $(INSTALL_ROOT)/usr/local/share/kaptain/
	-$(INSTALL_FILE) /home/zsolt/kaptain/kaptain-0.73/examples/budget.kaptn $(INSTALL_ROOT)/usr/local/share/kaptain/
	-$(INSTALL_FILE) /home/zsolt/kaptain/kaptain-0.73/examples/crypt.kaptn $(INSTALL_ROOT)/usr/local/share/kaptain/
	-$(INSTALL_FILE) /home/zsolt/kaptain/kaptain-0.73/examples/curl.kaptn $(INSTALL_ROOT)/usr/local/share/kaptain/
	-$(INSTALL_FILE) /home/zsolt/kaptain/kaptain-0.73/examples/dia2code.kaptn $(INSTALL_ROOT)/usr/local/share/kaptain/
	-$(INSTALL_FILE) /home/zsolt/kaptain/kaptain-0.73/examples/emboss.tgz $(INSTALL_ROOT)/usr/local/share/kaptain/
	-$(INSTALL_FILE) /home/zsolt/kaptain/kaptain-0.73/examples/enscript.kaptn $(INSTALL_ROOT)/usr/local/share/kaptain/
	-$(INSTALL_FILE) /home/zsolt/kaptain/kaptain-0.73/examples/find.kaptn $(INSTALL_ROOT)/usr/local/share/kaptain/
	-$(INSTALL_FILE) /home/zsolt/kaptain/kaptain-0.73/examples/finger.kaptn $(INSTALL_ROOT)/usr/local/share/kaptain/
	-$(INSTALL_FILE) /home/zsolt/kaptain/kaptain-0.73/examples/grep.kaptn $(INSTALL_ROOT)/usr/local/share/kaptain/
	-$(INSTALL_FILE) /home/zsolt/kaptain/kaptain-0.73/examples/indent.kaptn $(INSTALL_ROOT)/usr/local/share/kaptain/
	-$(INSTALL_FILE) /home/zsolt/kaptain/kaptain-0.73/examples/lameGUI.kaptn $(INSTALL_ROOT)/usr/local/share/kaptain/
	-$(INSTALL_FILE) /home/zsolt/kaptain/kaptain-0.73/examples/ls.kaptn $(INSTALL_ROOT)/usr/local/share/kaptain/
	-$(INSTALL_FILE) /home/zsolt/kaptain/kaptain-0.73/examples/mpage.kaptn $(INSTALL_ROOT)/usr/local/share/kaptain/
	-$(INSTALL_FILE) /home/zsolt/kaptain/kaptain-0.73/examples/nslookup.kaptn $(INSTALL_ROOT)/usr/local/share/kaptain/
	-$(INSTALL_FILE) /home/zsolt/kaptain/kaptain-0.73/examples/open.kaptn $(INSTALL_ROOT)/usr/local/share/kaptain/
	-$(INSTALL_FILE) /home/zsolt/kaptain/kaptain-0.73/examples/ping.kaptn $(INSTALL_ROOT)/usr/local/share/kaptain/
	-$(INSTALL_FILE) /home/zsolt/kaptain/kaptain-0.73/examples/povray.kaptn $(INSTALL_ROOT)/usr/local/share/kaptain/
	-$(INSTALL_FILE) /home/zsolt/kaptain/kaptain-0.73/examples/procmail.kaptn $(INSTALL_ROOT)/usr/local/share/kaptain/
	-$(INSTALL_FILE) /home/zsolt/kaptain/kaptain-0.73/examples/puf.tgz $(INSTALL_ROOT)/usr/local/share/kaptain/
	-$(INSTALL_FILE) /home/zsolt/kaptain/kaptain-0.73/examples/search.kaptn $(INSTALL_ROOT)/usr/local/share/kaptain/
	-$(INSTALL_FILE) /home/zsolt/kaptain/kaptain-0.73/examples/tar.kaptn $(INSTALL_ROOT)/usr/local/share/kaptain/
	-$(INSTALL_FILE) /home/zsolt/kaptain/kaptain-0.73/examples/weblint.kaptn $(INSTALL_ROOT)/usr/local/share/kaptain/
	-$(INSTALL_FILE) /home/zsolt/kaptain/kaptain-0.73/examples/wget.kaptn $(INSTALL_ROOT)/usr/local/share/kaptain/
	-$(INSTALL_FILE) /home/zsolt/kaptain/kaptain-0.73/examples/whois.kaptn $(INSTALL_ROOT)/usr/local/share/kaptain/
	-$(INSTALL_FILE) /home/zsolt/kaptain/kaptain-0.73/examples/zangband.kaptn $(INSTALL_ROOT)/usr/local/share/kaptain/


uninstall_examples:  FORCE
	-$(DEL_FILE) -r $(INSTALL_ROOT)/usr/local/share/kaptain/arping.kaptn 
	 -$(DEL_FILE) -r $(INSTALL_ROOT)/usr/local/share/kaptain/budget.kaptn 
	 -$(DEL_FILE) -r $(INSTALL_ROOT)/usr/local/share/kaptain/crypt.kaptn 
	 -$(DEL_FILE) -r $(INSTALL_ROOT)/usr/local/share/kaptain/curl.kaptn 
	 -$(DEL_FILE) -r $(INSTALL_ROOT)/usr/local/share/kaptain/dia2code.kaptn 
	 -$(DEL_FILE) -r $(INSTALL_ROOT)/usr/local/share/kaptain/emboss.tgz 
	 -$(DEL_FILE) -r $(INSTALL_ROOT)/usr/local/share/kaptain/enscript.kaptn 
	 -$(DEL_FILE) -r $(INSTALL_ROOT)/usr/local/share/kaptain/find.kaptn 
	 -$(DEL_FILE) -r $(INSTALL_ROOT)/usr/local/share/kaptain/finger.kaptn 
	 -$(DEL_FILE) -r $(INSTALL_ROOT)/usr/local/share/kaptain/grep.kaptn 
	 -$(DEL_FILE) -r $(INSTALL_ROOT)/usr/local/share/kaptain/indent.kaptn 
	 -$(DEL_FILE) -r $(INSTALL_ROOT)/usr/local/share/kaptain/lameGUI.kaptn 
	 -$(DEL_FILE) -r $(INSTALL_ROOT)/usr/local/share/kaptain/ls.kaptn 
	 -$(DEL_FILE) -r $(INSTALL_ROOT)/usr/local/share/kaptain/mpage.kaptn 
	 -$(DEL_FILE) -r $(INSTALL_ROOT)/usr/local/share/kaptain/nslookup.kaptn 
	 -$(DEL_FILE) -r $(INSTALL_ROOT)/usr/local/share/kaptain/open.kaptn 
	 -$(DEL_FILE) -r $(INSTALL_ROOT)/usr/local/share/kaptain/ping.kaptn 
	 -$(DEL_FILE) -r $(INSTALL_ROOT)/usr/local/share/kaptain/povray.kaptn 
	 -$(DEL_FILE) -r $(INSTALL_ROOT)/usr/local/share/kaptain/procmail.kaptn 
	 -$(DEL_FILE) -r $(INSTALL_ROOT)/usr/local/share/kaptain/puf.tgz 
	 -$(DEL_FILE) -r $(INSTALL_ROOT)/usr/local/share/kaptain/search.kaptn 
	 -$(DEL_FILE) -r $(INSTALL_ROOT)/usr/local/share/kaptain/tar.kaptn 
	 -$(DEL_FILE) -r $(INSTALL_ROOT)/usr/local/share/kaptain/weblint.kaptn 
	 -$(DEL_FILE) -r $(INSTALL_ROOT)/usr/local/share/kaptain/wget.kaptn 
	 -$(DEL_FILE) -r $(INSTALL_ROOT)/usr/local/share/kaptain/whois.kaptn 
	 -$(DEL_FILE) -r $(INSTALL_ROOT)/usr/local/share/kaptain/zangband.kaptn
	-$(DEL_DIR) $(INSTALL_ROOT)/usr/local/share/kaptain/ 


install_man: first FORCE
	@$(CHK_DIR_EXISTS) $(INSTALL_ROOT)/usr/local/share/man/man1/ || $(MKDIR) $(INSTALL_ROOT)/usr/local/share/man/man1/ 
	-$(INSTALL_FILE) /home/zsolt/kaptain/kaptain-0.73/man/kaptain.1 $(INSTALL_ROOT)/usr/local/share/man/man1/


uninstall_man:  FORCE
	-$(DEL_FILE) -r $(INSTALL_ROOT)/usr/local/share/man/man1/kaptain.1
	-$(DEL_DIR) $(INSTALL_ROOT)/usr/local/share/man/man1/ 


install:  install_target install_documentation install_examples install_man  FORCE

uninstall: uninstall_target uninstall_documentation uninstall_examples uninstall_man   FORCE

FORCE:

