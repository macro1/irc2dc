/*-
 *   Copyright (C) 2008-2009 by Maxim Ignatenko
 *   gelraen.ua@gmail.com
 *
 *   All rights reserved.                                                  *
 *                                                                         *
 *   Redistribution and use in source and binary forms, with or without    *
 *    modification, are permitted provided that the following conditions   *
 *    are met:                                                             *
 *     * Redistributions of source code must retain the above copyright    *
 *       notice, this list of conditions and the following disclaimer.     *
 *     * Redistributions in binary form must reproduce the above copyright *
 *       notice, this list of conditions and the following disclaimer in   *
 *       the documentation and/or other materials provided with the        *
 *       distribution.                                                     *
 *                                                                         *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   *
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     *
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR *
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT  *
 *   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, *
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT      *
 *   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, *
 *   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY *
 *   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT   *
 *   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE *
 *   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  *
 *
 *  $Id: defs.cpp 49 2009-05-22 15:44:21Z imax $
 */

#include "defs.h"
#include <string>
#include <cctype>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <fstream>
#include <signal.h>

using namespace std;

unsigned long LogLevel=0;
ofstream logfile;
string logname;

void reopen_log(int)
{
	logfile.close();
	logfile.open(logname.c_str(),ios::app);
	// what should we do if log not reopened correctly?
	signal(SIGHUP,reopen_log);
}

typedef void (*log_f)(int, const string&);

void log_stderr(int n,const string& str)
{
	tm t;
	time_t tt=time(NULL);
	localtime_r(&tt,&t);
	char buf[256]={0};
	strftime(buf,256,"%F %T: ",&t);
	cerr << buf << str << endl;
}

void log_file(int n, const string& str)
{
	tm t;
	time_t tt=time(NULL);
	localtime_r(&tt,&t);
	char buf[256]={0};
	strftime(buf,256,"%F %T: ",&t);
	logfile << buf << str << endl;
}

void log_syslog(int n, const string& str)
{
	int sysloglevel =
			(n==log::error)   ? LOG_ERR :
			(n==log::warning) ? LOG_WARNING :
			(n==log::notice)  ? LOG_NOTICE :
			(n==log::rawdata) ? LOG_DEBUG :
			(n==log::state || n==log::command) ? LOG_INFO :
			LOG_ERR;
	syslog(LOG_DAEMON | sysloglevel, str.c_str());
}

log_f logger=log_stderr;

string trim(string str, const char ch)
{
	str.erase(0, str.find_first_not_of(ch));
	str.erase(str.find_last_not_of(ch)+1);
	return str;
}

bool initlog(bool usesyslog,const string& filename)
{
	if (usesyslog)
	{
		logger=log_syslog;
	}
	else
	{
		logfile.open(filename.c_str(),ios::app);
		if (!logfile) return false;
		logname=filename;
		logger=log_file;
		signal(SIGHUP,reopen_log);
	}
	return true;
}

void LOG(int n,const string& str, bool explainErrno)
{
	if ((n)&LogLevel)
	{
		string errnoMessage = explainErrno? ": "+string(strerror(errno)) : "";
		logger(n,str+errnoMessage);
	}
}

