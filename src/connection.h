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
 *  $Id: connection.h 27 2009-05-14 11:17:28Z imax $
 */
#ifndef CONNECTION_H
#define CONNECTION_H

#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>

using namespace std;

/**
 @author gelraen <gelraen.ua@gmail.com>
*/

class Connection
{

public:
	Connection();

	virtual ~Connection();
	int FdSet( fd_set& fdset ) const;
	virtual bool ReadCmdAsync( string& str ) = 0;
	virtual bool WriteCmdAsync( const string& str ) = 0;
	int Connect(const struct sockaddr *name, socklen_t namelen);
	virtual int Close();
	bool isConnected() const;
    virtual bool ReadCmdSync(string& str);
    virtual bool WriteCmdSync(const string& str);
protected:
	void io();
	void _write();
	void _read();
	
private:
	bool m_bConnected;
protected:
	int m_socket;
	string m_recvbuf;
	string m_sendbuf;
};

#endif
