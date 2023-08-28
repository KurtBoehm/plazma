///////////////////////////////////////////////////////////////////////////////
//
/// \file       message.c
/// \brief      Printing messages
//
//  Author:     Lasse Collin
//
//  This file has been put into the public domain.
//  You can do whatever you want with this file.
//
///////////////////////////////////////////////////////////////////////////////

#include "private.h"

#include <stdarg.h>


/// Verbosity level
static enum message_verbosity verbosity = V_WARNING;


extern void
message_init(void)
{}


extern void
message_verbosity_increase(void)
{
	if (verbosity < V_DEBUG)
		++verbosity;

	return;
}


extern void
message_verbosity_decrease(void)
{
	if (verbosity > V_SILENT)
		--verbosity;

	return;
}


extern enum message_verbosity
message_verbosity_get(void)
{
	return verbosity;
}


static void
vmessage(enum message_verbosity v, const char *fmt, va_list ap)
{
	if (v <= verbosity) {
#ifdef __clang__
#	pragma GCC diagnostic push
#	pragma GCC diagnostic ignored "-Wformat-nonliteral"
#endif
		vfprintf(stderr, fmt, ap);
#ifdef __clang__
#	pragma GCC diagnostic pop
#endif

		fputc('\n', stderr);
	}
}


extern void
message(enum message_verbosity v, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vmessage(v, fmt, ap);
	va_end(ap);
}


extern void
message_warning(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vmessage(V_WARNING, fmt, ap);
	va_end(ap);

	set_exit_status(E_WARNING);
	return;
}


extern void
message_error(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vmessage(V_ERROR, fmt, ap);
	va_end(ap);

	set_exit_status(E_ERROR);
	return;
}


extern void
message_fatal(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vmessage(V_ERROR, fmt, ap);
	va_end(ap);

	tuklib_exit(E_ERROR, E_ERROR, false);
}


extern void
message_bug(void)
{
	message_fatal(_("Internal error (bug)"));
}


extern void
message_signal_handler(void)
{
	message_fatal(_("Cannot establish signal handlers"));
}


extern const char *
message_strm(lzma_ret code)
{
	switch (code) {
	case LZMA_NO_CHECK:
		return _("No integrity check; not verifying file integrity");

	case LZMA_UNSUPPORTED_CHECK:
		return _("Unsupported type of integrity check; "
				"not verifying file integrity");

	case LZMA_MEM_ERROR:
		return strerror(ENOMEM);

	case LZMA_MEMLIMIT_ERROR:
		return _("Memory usage limit reached");

	case LZMA_FORMAT_ERROR:
		return _("File format not recognized");

	case LZMA_OPTIONS_ERROR:
		return _("Unsupported options");

	case LZMA_DATA_ERROR:
		return _("Compressed data is corrupt");

	case LZMA_BUF_ERROR:
		return _("Unexpected end of input");

	case LZMA_OK:
	case LZMA_STREAM_END:
	case LZMA_GET_CHECK:
	case LZMA_PROG_ERROR:
	case LZMA_SEEK_NEEDED:
	case LZMA_RET_INTERNAL1:
	case LZMA_RET_INTERNAL2:
	case LZMA_RET_INTERNAL3:
	case LZMA_RET_INTERNAL4:
	case LZMA_RET_INTERNAL5:
	case LZMA_RET_INTERNAL6:
	case LZMA_RET_INTERNAL7:
	case LZMA_RET_INTERNAL8:
		// Without "default", compiler will warn if new constants
		// are added to lzma_ret, it is not too easy to forget to
		// add the new constants to this function.
		break;
	}

	return _("Internal error (bug)");
}


extern void
message_mem_needed(enum message_verbosity v, uint64_t memusage)
{
	if (v > verbosity)
		return;

	// Convert memusage to MiB, rounding up to the next full MiB.
	// This way the user can always use the displayed usage as
	// the new memory usage limit. (If we rounded to the nearest,
	// the user might need to +1 MiB to get high enough limit.)
	memusage = round_up_to_mib(memusage);

	uint64_t memlimit = hardware_memlimit_get(opt_mode);

	// Handle the case when there is no memory usage limit.
	// This way we don't print a weird message with a huge number.
	if (memlimit == UINT64_MAX) {
		message(v, _("%s MiB of memory is required. "
				"The limiter is disabled."),
				uint64_to_str(memusage, 0));
		return;
	}

	// With US-ASCII:
	// 2^64 with thousand separators + " MiB" suffix + '\0' = 26 + 4 + 1
	// But there may be multibyte chars so reserve enough space.
	char memlimitstr[128];

	// Show the memory usage limit as MiB unless it is less than 1 MiB.
	// This way it's easy to notice errors where one has typed
	// --memory=123 instead of --memory=123MiB.
	if (memlimit < (UINT32_C(1) << 20)) {
		snprintf(memlimitstr, sizeof(memlimitstr), "%s B",
				uint64_to_str(memlimit, 1));
	} else {
		// Round up just like with memusage. If this function is
		// called for informational purposes (to just show the
		// current usage and limit), we should never show that
		// the usage is higher than the limit, which would give
		// a false impression that the memory usage limit isn't
		// properly enforced.
		snprintf(memlimitstr, sizeof(memlimitstr), "%s MiB",
				uint64_to_str(round_up_to_mib(memlimit), 1));
	}

	message(v, _("%s MiB of memory is required. The limit is %s."),
			uint64_to_str(memusage, 0), memlimitstr);

	return;
}


extern void
message_filters_show(enum message_verbosity v, const lzma_filter *filters)
{
	if (v > verbosity)
		return;

	char *buf;
	const lzma_ret ret = lzma_str_from_filters(&buf, filters,
			LZMA_STR_ENCODER | LZMA_STR_GETOPT_LONG, NULL);
	if (ret != LZMA_OK)
		message_fatal("%s", message_strm(ret));

	fprintf(stderr, _("Filter chain: %s\n"), buf);
	free(buf);
	return;
}
