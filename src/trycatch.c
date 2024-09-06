#include "trycatch.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

jmp_buf *volatile __try_env = NULL;
Error *volatile __error     = NULL;

Error new_Error(const char *msg_fmt, ...)
{
    va_list args;
    va_start(args, msg_fmt);
    Error self = va_new_Error(msg_fmt, args);
    va_end(args);
    return self;
}

Error va_new_Error(const char *msg_fmt, va_list args)
{
    Error self;
    __va_init_Error(&self, msg_fmt, args);
    return self;
}

void __va_init_Error(Error *self, const char *msg_fmt, va_list args)
{
    self->type_name      = "Error";
    self->backtrace_size = 0;
    self->__init__       = &__va_init_Error;
    int count = vsnprintf((char *) self->msg, MAX_ERR_MSG_LEN, msg_fmt, args);
    assert(count < MAX_ERR_MSG_LEN);
    self->backtrace_size = 0;
}
