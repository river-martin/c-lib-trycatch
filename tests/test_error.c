#include "../src/trycatch.h"
#include "custom_error.h"
#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void test_new_err(void)
{
    Error error = new_Error("%s", __func__);
    assert(strcmp(error.type_name, "Error") == 0);
    assert(strcmp(error.msg, __func__) == 0);
    printf("%s: passed\n", __func__);
}

void test_custom_err(void)
{
    CustomError error = new_CustomError("%s", __func__);
    assert(strcmp(error.type_name, "CustomError") == 0);
    assert(strcmp(error.msg, __func__) == 0);
    printf("%s: passed\n", __func__);
}

int main(void)
{
    test_new_err();
    test_custom_err();
    return EXIT_SUCCESS;
}
