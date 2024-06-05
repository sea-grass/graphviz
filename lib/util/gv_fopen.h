/// @file
/// @brief wrapper around `fopen` for internal library usage

#pragma once

/// hide the symbols this header declares by default
///
/// The expectation is that users of this header (applications, shared
/// libraries, or static libraries) want to call `gv_fopen` but not re-export it
/// to their users. This annotation is only correct while the containing library
/// is built statically. If it were built as a shared library, `gv_fopen` would
/// need to have `default` visibility (and thus be unavoidably re-exported) in
/// order to be callable.
#ifndef UTIL_API
#if !defined(__CYGWIN__) && defined(__GNUC__) && !defined(__MINGW32__)
#define UTIL_API __attribute__((visibility("hidden")))
#else
#define UTIL_API /* nothing */
#endif
#endif

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/// open a file, setting close-on-exec
///
/// Generally, library code should set close-on-exec (`O_CLOEXEC`) on file
/// descriptors it creates to avoid child processes of concurrent `fork`+`exec`
/// operations accidentally inheriting copies of the descriptors. It is tricky
/// to achieve this without races. This function attempts to avoid the common
/// problems when trying to do this with `fopen`.
///
/// @param filename A filename, as you would pass to `fopen`
/// @param mode A mode, as you would pass to `fopen`
/// @return A file handle with close-on-exit set on success or `NULL` on failure
UTIL_API FILE *gv_fopen(const char *filename, const char *mode);

#ifdef __cplusplus
}
#endif
