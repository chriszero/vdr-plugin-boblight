/*
 * common.c: EPG2VDR plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#include <sys/time.h>
#include <stdarg.h>
#include <string.h>
#include <syslog.h>

#include <vdr/thread.h>

#include "common.h"
#include "config.h"

cMutex logMutex;

//***************************************************************************
// Tell
//***************************************************************************

void tell(int eloquence, const char* format, ...)
{
   if (cfg.loglevel < eloquence)
      return ;

   const int sizeBuffer = 100000;
   char t[sizeBuffer+100]; *t = 0;
   va_list ap;

   cMutexLock lock(&logMutex);
   
   va_start(ap, format);

   snprintf(t, sizeBuffer, "BOBLIGHT: ");
   vsnprintf(t+strlen(t), sizeBuffer-strlen(t), format, ap);
   
   syslog(LOG_ERR, "%s", t);

   va_end(ap);
}

//***************************************************************************
// Error
//***************************************************************************

int error(const char* format, ...)
{
   const int sizeBuffer = 100000;
   char t[sizeBuffer+100]; *t = 0;
   va_list ap;

   cMutexLock lock(&logMutex);
   
   va_start(ap, format);

   snprintf(t, sizeBuffer, "BOBLIGHT: ");
   vsnprintf(t+strlen(t), sizeBuffer-strlen(t), format, ap);
   
   syslog(LOG_ERR, "%s", t);

   va_end(ap);

   return fail;
}

//***************************************************************************
// msNow
//***************************************************************************

MsTime msNow()
{
   timeval tv;

   gettimeofday(&tv, 0); 

   return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}
