#ifndef CUSTOM_ERROR_H
#define CUSTOM_ERROR_H

#include "../src/trycatch.h"

/**
 * @brief The type of error used when a requirement is not met.
 */
typedef Error CustomError;

extern CustomError new_CustomError(const char *msg_fmt, ...);

extern void
__va_init_CustomError(CustomError *self, const char *msg_fmt, va_list args);

#endif // CUSTOM_ERROR_H
