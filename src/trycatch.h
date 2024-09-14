#ifndef _TRYCATCH_H
#define _TRYCATCH_H

#include <assert.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "../c-lib-stack/src/stack.h"

// Macros

#define MAX_ERR_MSG_LEN 256
#define BT_BUF_SIZE     256

#define __peek_jmp_buf(__env_stack) (*((jmp_buf *) (__env_stack.top->item)))

/**
 * Start a `try-catch` expression. The `try` block must be
 * followed by one or more `catch(err_type, err_ident)` blocks.
 *
 * Usage: `try { ... } catch(err_type, err_ident) { ... }`
 */
#define try                                      \
    vp_stack_push(&__env_stack, malloc(sizeof(jmp_buf))); \
    if (setjmp(__peek_jmp_buf(__env_stack)) == 0)

/**
 * Catch the error of type `err_type`.
 *
 * Usage: `try { ... } catch(err_type, err_ident) { ... }`
 *
 * @param err_type The type of error to catch
 * @param err_ident The identifier to use for the caught error
 */
#define catch(err_type, err_ident)                                          \
    else if (__error && (strcmp(#err_type, __error->type_name) ==           \
                         0)) for (err_type err_ident = *__error;            \
                                  __error ? (free(__error), __error = NULL, \
                                             vp_stack_pop(&__env_stack), true)       \
                                          : false;)

/**
 * Usage: `throw(new_Error("An error occurred"))`
 *
 * Throws the given `Error`. This macro should only be used inside a `try`
 * block.
 *
 * The `Error` is copied to the heap so that it can be accessed after the
 * `longjmp`, by the `catch` macro.
 *
 * @param err The `Error` to throw.
 */
#define throw(err)                                          \
    {                                                       \
        assert(!__error);                                   \
        __error  = malloc(sizeof(Error));                   \
        *__error = err;                                     \
        assert(__env_stack.top);                            \
        longjmp(*((jmp_buf *) (__env_stack.top->item)), 1); \
    }

// Type definitions

/**
 * The base error struct (associated with `const ErrType BASE_ERR_TYPE`). All
 * other error types should extend or use this struct.
 */
typedef struct error {
    const char *type_name; /**< The name of this type of error */

    /**
     * Initializes the given Error * with the given format string and
     * arguments.
     *
     * @param self    The Error * to initialize
     * @param msg_fmt The format string for the error message (see vsprintf for
     * details)
     * @param args    The va_list of arguments to be formatted into the message
     * string
     */
    void (*__init__)(struct error *self, const char *msg_fmt, va_list args);

    char  msg[MAX_ERR_MSG_LEN]; /**< The error message */
    void *backtrace_buf[BT_BUF_SIZE];
    int   backtrace_size;
} Error;

// Function prototypes

/**
 * Allocates memory for a new Error struct and initializes it's message field
 * with the given format string and arguments.
 *
 * Example usage:
 * new_Error("%s:%d: %s", __FILE__, __LINE__, "An error
 * occurred");`
 *
 * @param msg_fmt The format string for the error message (see printf for
 * details)
 * @param ...     The arguments to be formatted into the message string
 */
extern Error new_Error(const char *msg_fmt, ...);

/**
 * The same as new_Error, but takes a va_list instead of a variable number of
 * arguments.
 *
 * @param msg_fmt The format string for the error message (see printf for
 * details)
 * @param args    The va_list of arguments to be formatted into the message
 * string
 */
extern Error va_new_Error(const char *msg_fmt, va_list args);

/**
 * Initializes the given Error * with the given format string and
 * arguments.
 *
 * @param self    The Error * to initialize
 * @param msg_fmt The format string for the error message (see vsprintf for
 * details)
 * @param args    The va_list of arguments to be formatted into the message
 * string
 */
extern void __va_init_Error(Error *self, const char *msg_fmt, va_list args);

// Global error variable (set when an error is thrown)
extern Error *volatile __error;

// Used in error messages
extern const char *__progname;

extern vp_stack __env_stack;

extern jmp_buf *volatile __try_env;

#endif // _TRYCATCH_H
