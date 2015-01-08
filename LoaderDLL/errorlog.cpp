#include "stdafx.h"
#include "crash.h"

static char error_buf[500];
Logger *g_logger = 0;

////////////////////////////////////////////////////////////////////////////////
//
//  Constructor/Destructor
//
////////////////////////////////////////////////////////////////////////////////

Logger::Logger()
: m_verbose(true), m_flush(true)
{
	char Buff[MAX_PATH];
	GetModuleFileName(DllBase,Buff,MAX_PATH);
	char *Tmp=strrchr(Buff,'\\');
	if(Tmp)
		*Tmp=0;
	strcat(Buff,"\\crash.txt");
    m_fp = fopen(Buff, "wt");
    if(m_fp == NULL)
    {
        OutputDebugString("ERROR: failed to create log file\n");
        OutputDebugString(strerror(errno));
    }
    else
        g_logger = this;
}

Logger::~Logger()
{
    g_logger = 0;
    if(m_fp != 0)
    {
        printf(true, "Log closed.\n");
        fclose(m_fp);
    }
}

void CreateLogger()
{
	if(g_logger==0)
		g_logger = new Logger();
}

////////////////////////////////////////////////////////////////////////////////
//
//  Dump functions
//
////////////////////////////////////////////////////////////////////////////////

void Logger::dump(const char *Comment,bool verbose, unsigned char * buf, int length)
{
    if(verbose && !m_verbose)
        return;
	fprintf(m_fp,"------------------ %s ------------------\n",Comment);
	dump(verbose, buf, length);
}

void Logger::dump(bool verbose, unsigned char * buf, int length)
{
    if(verbose && !m_verbose)
        return;
    int num_lines = length / 16;
    if(length % 16 != 0)
        num_lines++;

    // Dump the buffer 16 bytes per line
    for(int line = 0; line < num_lines; line++)
    {
        int row;
        fprintf(m_fp, "%04x: ", line * 16);

        // Print the bytes of the line as hex
        for(row = 0; row < 16; row++)
        {
            if(line * 16 + row < length)
                fprintf(m_fp, "%02x ", buf[line * 16 + row]);
            else
                fprintf(m_fp, "-- ");
        }
        fprintf(m_fp, ": ");

        // Print the bytes as characters (if printable)
        for(row = 0; row < 16; row++)
        {
            if(line * 16 + row < length)
                fputc(isprint(buf[line * 16 + row])
                    ? buf[line * 16 + row] : '.', m_fp);
        }
        fputc('\n', m_fp);
    }
    if(m_flush)
        fflush(m_fp);
}

void Logger::printf(bool verbose, const char * format, ...)
{
    if(verbose && !m_verbose)
        return;
    va_list arg;
    va_start(arg, format);
    vfprintf(m_fp, format, arg);
    va_end(arg);

    if(m_flush)
        fflush(m_fp);
}

void Logger::vprintf(bool verbose, const char * format, va_list ap)
{
	if(verbose && !m_verbose)
        return;
    vfprintf(m_fp, format, ap);
    if(m_flush)
        fflush(m_fp);
}

void Logger::flush()
{
    fflush(m_fp);
}

void error_printf(const char * format, ...)
{
	CreateLogger();
    if(g_logger != 0)
    {
        va_list arg;
        va_start(arg, format);
        g_logger->printf(false, "***Error: ");
        g_logger->vprintf(false, format, arg);
        va_end(arg);
    }
    else
    {
        OutputDebugString("error_printf(): logger doesn't exist!\n");
        va_list arg;
        va_start(arg, format);
        vsprintf(error_buf, format, arg);
        va_end(arg);
        OutputDebugString(error_buf);
    }
}

void log_dump(unsigned char * buf, int length)
{
	CreateLogger();
    if(g_logger != 0)
        g_logger->dump(false, buf, length);
    else
        OutputDebugString("log_dump(): logger doesn't exist!\n");
}

void log_flush()
{
	CreateLogger();
    if(g_logger != 0)
        g_logger->flush();
    else
        OutputDebugString("log_flush(): logger doesn't exist!");
}