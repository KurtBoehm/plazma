///////////////////////////////////////////////////////////////////////////////
//
/// \file       args.c
/// \brief      Argument parsing
///
/// \note       Filter-specific options parsing is in options.c.
//
//  Author:     Lasse Collin
//
//  This file has been put into the public domain.
//  You can do whatever you want with this file.
//
///////////////////////////////////////////////////////////////////////////////

#include "private.h"

#include "getopt.h"
#include <ctype.h>

bool opt_stdout = false;
bool opt_force = false;
bool opt_keep_original = false;

// We don't modify or free() this, but we need to assign it in some
// non-const pointers.
const char stdin_filename[] = "(stdin)";
