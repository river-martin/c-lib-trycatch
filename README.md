# c-lib-trycatch

This library defines the macros `try` and `catch(err_type, err_ident)`, and the type `Error`.

## Installation

```Bash
make
sudo make install
```

To uninstall the library, run `sudo make uninstall`.

## Testing

```Bash
make tests
./test_error
./test_try_catch
```

## Usage

Include the header file in your C code, and use the `try`, `catch(err_type, err_ident)`, and `throw(err)` macros. For example:

```C
#include <trycatch.h>
#include <stdio.h>

int main(void) {
  try {
    throw(new_Error("Error at %s:%d\n", __func__, __LINE__));
  } catch(Error, error) {
    printf("%s", error.msg);
  }
  return 0;
}
```

Link against the library when you compile your code.

```Bash
clang -o your_code your_code.c -ltrycatch
```
