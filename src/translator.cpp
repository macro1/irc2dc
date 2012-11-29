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
 *  $Id: translator.cpp 50 2009-05-22 16:21:05Z imax $
 */
#include "translator.h"
#include <iostream>
#include "defs.h"

using namespace std;

Translator::Translator()
{
	m_bConfigured=false;
}


Translator::~Translator()
{
	if (m_bConfigured)
	{
		regfree(&m_IRCRe);
		regfree(&m_DCRe);
	}
}

/*!
    \fn Translator::acceptableIRCMessage(const string& str)
 */
bool Translator::acceptableIRCMessage(const string& str)
{
	if (!m_bConfigured) return false;
	int t;
	regmatch_t match;
	if ((t=regexec(&m_IRCRe,str.c_str(),1,&match,0))!=0)
	{
		if (t!=REG_NOMATCH)
		{
			char buf[256]={0};
			sprintf(buf,"%d",t);
			LOG(log::warning,
				string("Translator::acceptableIRCMessage(): regexec(3) returned ")+buf);
		}
		return false;
	}
	// chech that regex matched full string
	if (match.rm_so!=0||match.rm_eo!=str.length()) return false;
	
	return true;
}


/*!
    \fn Translator::acceptableDCMessage(const string& str)
 */
bool Translator::acceptableDCMessage(const string& str)
{
	if (!m_bConfigured) return false;
	int t;
	regmatch_t match;
	if ((t=regexec(&m_DCRe,str.c_str(),1,&match,0))!=0)
	{
		if (t!=REG_NOMATCH)
		{
			char buf[256]={0};
			sprintf(buf,"%d",t);
			LOG(log::warning,
				string("Translator::acceptableDCMessage(): regexec(3) returned ")+buf);
		}
		return false;
	}
	// chech that regex matched full string
	if (match.rm_so!=0||match.rm_eo!=str.length()) return false;
	
	
	// reject our own messages
	if (str.find(string("<")+m_config.m_dc_nick+string("> "))==0) return false;
	return true;
}


/*!
    \fn Translator::getConfig()
 */
const Config& Translator::getConfig()
{
	return m_config;
}


/*!
    \fn Translator::setConfig(const Config& conf)
 */
bool Translator::setConfig(const Config& conf)
{
	// check config
	if (conf.m_dc_nick.empty()) return false;
	if (conf.m_irc_channel.empty()) return false;
	
	m_config=conf;
	
	/* form REs
	 * REs will be used just for matching messages,
	 * needed fields will be extracted in some other way
	*/

	if (m_bConfigured)
	{
		regfree(&m_IRCRe);
		regfree(&m_DCRe);
	}
	
	// these characters needs to be escaped: "^.[]$()|*+?{\"
	string str=m_config.m_irc_channel;
	string::size_type pos=0;
	while ((pos=str.find_first_of("^.[]$()|*+?{\\",pos))!=string::npos)
	{
		str.insert(pos,1,'\\');
		pos+=2;
	}
	
	int t;
	char buf[256]={0};
	if ((t=regcomp(&m_IRCRe,
		 (string(":[^:!@ ]+((![^:!@ ]+)?@[^:!@ ]+)? PRIVMSG ")+
			str + string(" :.*")).c_str(), REG_EXTENDED))!=0)
	{
		sprintf(buf,"%d",t);
		LOG(log::warning,
			string("Translator::setConfig(): Can not compile regex for IRC. ")+
			string("regcomp(3) returned ")+buf);
		return false;
	}
	// maybe here we should use REG_PEND, cause '\0' can be in DC message
	if ((t=regcomp(&m_DCRe,"<[^$\\|<> ]+> .*",REG_EXTENDED))!=0)
	{
		sprintf(buf,"%d",t);
		LOG(log::warning,
			string("Translator::setConfig(): Can not compile regex for DC. ")+
			string("regcomp(3) returned ")+buf);
		regfree(&m_IRCRe);
		return false;
	}
	
	m_bConfigured=true;	
	return true;
}


/*!
    \fn Translator::IRCtoDC(const string& src,string& dst)
 */
bool Translator::IRCtoDC(const string& src,string& dst)
{
	if (!m_bConfigured||!acceptableIRCMessage(src)) return false;
	
	string nick,text;
	string::size_type pos;
	
	pos=src.find_first_of("!@ ");
	// as src matched by regex, pos never shall be eq to string::npos
	if (pos==string::npos)
	{
		LOG(log::error,"Translator::IRCtoDC(): Something strange going on here...");
		return false;
	}
	nick=src.substr(1,pos-1); // skip leading ":"
	
	// ":" should appear only twice
	pos=src.find(':',1);
	if (pos==string::npos)
	{
		LOG(log::error,"Translator::IRCtoDC(): Something strange going on here... wtf?");
		return false;
	}
	text=src.substr(pos+1); // rest of the src is a message
	
	dst=string("<")+m_config.m_dc_nick+string("> <")+nick+string("> ")+text;
	
	return true;
}


/*!
    \fn Translator::DCtoIRC(const string& src,string& dst)
 */
bool Translator::DCtoIRC(const string& src,string& dst)
{
	if (!m_bConfigured||!acceptableDCMessage(src)) return false;
	
	/* here we don't need to parse something,
	 * and acceptableDCMessage should reject our own messages
	*/
	
	dst=string("PRIVMSG ")+m_config.m_irc_channel+string(" :")+src;
	
	return true;
}
