#include "../src/trycatch.h"
#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void test_try_catch(void)
{
    try {
        throw(new_Error("Thrown from `%s` %d\n", __func__, __LINE__));
        // This should not be reached
        assert(false);
    } catch (Error, e) {
        printf("Caught `%s` at %s:%d:\n\te.msg: %s", e.type_name, __func__,
               __LINE__, e.msg);
    } catch (Error, err) {
        // This should not be reached
        assert(false);
        printf("%s", err.msg);
    }
    printf("%s: passed\n", __func__);
}

void test_nested_try_catch(void)
{
    bool caught_nested_throw = false;
    try {
        try {
            throw(new_Error("Thrown from line %d\n", __LINE__));
        } catch (Error, e) {
            printf("Caught `%s` at %s:%d:\n\te.msg: %s", e.type_name, __func__,
                   __LINE__, e.msg);
            e = new_Error("Thrown from line %d\n", __LINE__);
            throw(e);
        }
        // This should not be reached
        assert(false);
    } catch (Error, e) {
        caught_nested_throw = true;
        printf("Caught `%s` on line %d:\n\t%s", e.type_name, __LINE__, e.msg);
    }
    assert(caught_nested_throw);
    printf("%s: passed\n", __func__);
}

int main(void)
{
    test_try_catch();
    test_nested_try_catch();
    return EXIT_SUCCESS;
}
