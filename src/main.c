///////////////////////////////////////////////////////////////////////////////
//
/// \file       main.c
/// \brief      main()
//
//  Author:     Lasse Collin
//
//  This file has been put into the public domain.
//  You can do whatever you want with this file.
//
///////////////////////////////////////////////////////////////////////////////

#include "private.h"
#include <ctype.h>

/// Exit status to use. This can be changed with set_exit_status().
static enum exit_status_type exit_status = E_SUCCESS;

#if defined(_WIN32) && !defined(__CYGWIN__)
/// exit_status has to be protected with a critical section due to
/// how "signal handling" is done on Windows. See signals.c for details.
static CRITICAL_SECTION exit_status_cs;
#endif

extern void set_exit_status(enum exit_status_type new_status) {
  assert(new_status == E_WARNING || new_status == E_ERROR);

#if defined(_WIN32) && !defined(__CYGWIN__)
  EnterCriticalSection(&exit_status_cs);
#endif

  if (exit_status != E_ERROR) {
    exit_status = new_status;
  }

#if defined(_WIN32) && !defined(__CYGWIN__)
  LeaveCriticalSection(&exit_status_cs);
#endif

  return;
}
