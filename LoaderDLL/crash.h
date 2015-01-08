////////////////////////////////////////////////////////////////////////////////
//
// crash.h
//
// Copyright (C) 2001 mamaich
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
////////////////////////////////////////////////////////////////////////////////


#ifndef CRASH_H
#define CRASH_H

#include <stdio.h>
#include <ctype.h>
#include <stdarg.h>
#include <errno.h>
#include <stdlib.h>
#include <assert.h>

LONG __stdcall MyUnhandledExceptionFilter(
  struct _EXCEPTION_POINTERS *ExceptionInfo);

class Logger
{
protected:
    FILE * m_fp;
    bool m_verbose, m_flush;

public:
    // Constructor/Destructor
    Logger();
    ~Logger();

public:
    // dump functions
    void dump(bool verbose, unsigned char * buf, int length);
    void dump(const char *comment, bool verbose, unsigned char * buf, int length);
    void printf(bool verbose, const char * format, ...);
    void vprintf(bool verbose, const char * format, va_list ap);
    void flush();
    bool get_flush() const { return m_flush; }
    void set_flush(bool flush) { m_flush = flush; }
    bool get_verbose() const { return m_verbose; }
    void set_verbose(bool verbose) { m_verbose = verbose; }
};

extern Logger * g_logger;

void error_printf(const char * IN format, ...);
void warning_printf(const char * IN format, ...);
void log_dump(unsigned char * IN buf, int IN length);
void log_flush();

extern HINSTANCE DllBase;

#endif
