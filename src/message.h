///////////////////////////////////////////////////////////////////////////////
//
/// \file       message.h
/// \brief      Printing messages to stderr
//
//  Author:     Lasse Collin
//
//  This file has been put into the public domain.
//  You can do whatever you want with this file.
//
///////////////////////////////////////////////////////////////////////////////

/// Verbosity levels
enum message_verbosity {
	V_SILENT,   ///< No messages
	V_ERROR,    ///< Only error messages
	V_WARNING,  ///< Errors and warnings
	V_VERBOSE,  ///< Errors, warnings, and verbose statistics
	V_DEBUG,    ///< Very verbose
};


/// \brief      Initializes the message functions
///
/// If an error occurs, this function doesn't return.
///
extern void message_init(void);


/// Increase verbosity level by one step unless it was at maximum.
extern void message_verbosity_increase(void);

/// Decrease verbosity level by one step unless it was at minimum.
extern void message_verbosity_decrease(void);

/// Get the current verbosity level.
extern enum message_verbosity message_verbosity_get(void);


/// \brief      Print a message if verbosity level is at least "verbosity"
///
/// This doesn't touch the exit status.
extern void message(enum message_verbosity verbosity, const char *fmt, ...)
		lzma_attribute((__format__(__printf__, 2, 3)));


/// \brief      Prints a warning and possibly sets exit status
///
/// The message is printed only if verbosity level is at least V_WARNING.
/// The exit status is set to WARNING unless it was already at ERROR.
extern void message_warning(const char *fmt, ...)
		lzma_attribute((__format__(__printf__, 1, 2)));


/// \brief      Prints an error message and sets exit status
///
/// The message is printed only if verbosity level is at least V_ERROR.
/// The exit status is set to ERROR.
extern void message_error(const char *fmt, ...)
		lzma_attribute((__format__(__printf__, 1, 2)));


/// \brief      Prints an error message and exits with EXIT_ERROR
///
/// The message is printed only if verbosity level is at least V_ERROR.
extern void message_fatal(const char *fmt, ...)
		lzma_attribute((__format__(__printf__, 1, 2)))
		lzma_attribute((__noreturn__));


/// Print an error message that an internal error occurred and exit with
/// EXIT_ERROR.
extern void message_bug(void) lzma_attribute((__noreturn__));


/// Print a message that establishing signal handlers failed, and exit with
/// exit status ERROR.
extern void message_signal_handler(void) lzma_attribute((__noreturn__));


/// Convert lzma_ret to a string.
extern const char *message_strm(lzma_ret code);


/// Display how much memory was needed and how much the limit was.
extern void message_mem_needed(enum message_verbosity v, uint64_t memusage);


/// Print the filter chain.
extern void message_filters_show(
		enum message_verbosity v, const lzma_filter *filters);
