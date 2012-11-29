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
 *  $Id: dcconnection.cpp 41 2009-05-21 23:29:05Z ulidtko $
 */
#include "dcconnection.h"
#include "defs.h"

DCConnection::DCConnection()
 : Connection()
{
}


DCConnection::~DCConnection()
{
}


/*!
    \fn DCConnection::WriteCmdAsync(const string& str)
 */
bool DCConnection::WriteCmdAsync(const string& s)
{
	// not sure is it needed. We may place data in buffer even if not connected
	if (!isConnected()) return false;
	
	// 1) replace "|" with "&#124;"
	//		this provided for guarantee that it will be single command
	//		all other possible replacements should be done somwhere else
	// 2) place command to m_sendbuf, followed by "|"
	string str=s;
	string::size_type pos=0;
	while((pos=str.find('|',pos))!=string::npos)
	{
		str.replace(pos,1,"&#124;");
	}
	LOG(log::command,string("to ")+int2str(m_socket)+string(" > ")+str);
	m_sendbuf+=str;
	m_sendbuf+=string("|");
	
	_write(); // send data to server
	
	return true;
}


/*!
    \fn DCConnection::ReadCmdAsync(string& str)
 */
bool DCConnection::ReadCmdAsync(string& str)
{
	if (!isConnected()) return false;
	
	_read(); // check for new data from server
	
	if (m_recvbuf.empty()) return false;
	
	string::size_type pos=0;
	pos=m_recvbuf.find("|",0);
	if (pos==string::npos) return false; // still no full command
	str=m_recvbuf.substr(0,pos);
	LOG(log::command,string("from ")+int2str(m_socket)+string(" > ")+str);
	m_recvbuf.erase(0,pos+1);
	
	// now replace encoded | and & with normal representation
	pos=0;
	while((pos=str.find("&#124;"))!=string::npos)
	{
		str.replace(pos,6,"|");
	}
	pos=0;
	while((pos=str.find("&#36;"))!=string::npos)
	{
		str.replace(pos,5,"$");
	}
	
	return true;
}
