INCLUDES = 
METASOURCES = AUTO
bin_PROGRAMS = irc2dc
irc2dc_SOURCES = config.cpp connection.cpp dcclient.cpp dcconfig.cpp \
	dcconnection.cpp defs.cpp ircclient.cpp ircconfig.cpp ircconnection.cpp main.cpp \
	translator.cpp
noinst_HEADERS = config.h connection.h dcclient.h dcconfig.h dcconnection.h \
	defs.h ircclient.h ircconfig.h ircconnection.h translator.h
noinst_PROGRAMS = dctest irctest
dctest_SOURCES = connection.cpp dcclient.cpp dcconfig.cpp dcconnection.cpp \
	dctest.cpp defs.cpp
irctest_SOURCES = connection.cpp defs.cpp ircclient.cpp ircconfig.cpp \
	ircconnection.cpp irctest.cpp
sysconf_DATA = irc2dc.conf.dist
EXTRA_DIST = irc2dc.conf.dist
DEFS+= -DCONFFILE="\"$(sysconfdir)/irc2dc.conf\""
