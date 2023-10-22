<p align="center"><img src="https://github.com/aqilc/c-mplytest/assets/32044067/c3981ea0-7cff-4333-894d-8385b8098436" width=300></p>
<p align="center">Simplifying C Testing for the first time in decades.</p>

A framework made to get right into testing in C with the main goals being simplicity, syntax (ease-of-typing over prettiness) and utility. This framework leverages the `__COUNTER__` macro provided by many compilers, and is limited to those compilers. Currently only tested with MSVC, MingW and GCC.

![Image of utility](https://github.com/aqilc/c-mplytest/assets/32044067/f07563bc-a726-451f-9454-1293ab28de1b)

### ✨ Features ✨

- No need to copy every test function name into "suites" and/or other BS.
- Clean test output with fully featured timing.
- Properly and easily allows you to name tests and parts of tests.
- Simple and clean to use.
- Catches SIGSEGVs and other signals without stopping the tests.
- Initializers/cleaners per test and before all tests.

### Installation

1. Download `tests.h` and `tests_end.h` into your local machine.
2. Put their directory in your include paths for your projects and compilation setups.

### Usage

1. Put `#include <tests.h>` at the start of your file before all other imports.
2. Put `#include <tests_end.h>` at the very end of your file.

> Note: `tests.h` imports `stdio.h` automatically. You don't need to import it again to use IO.

### API

- `TEST(char name[])`: Defines a test.
- `TEND()`/`TEND`: Put at the end of test definitions.
- `assert(bool passed)`: A check performed per test.
	- `asserteq(any_num_type n1, any_num_type n2)`: Can use any type of number for arguments, including Enums.
- `subtest(bool passed)`: Performs a named check inside of a test.
- `substart(char name[])`: Defines the start of a "subtest". This is a part of the test that is also individually timed
	- This resets the timer so any previous actions between subtests will not be timed.
- `subend(bool passed)`: Ends a subtest. The argument takes whether the subtest passed or failed.
- **Advanced:**
	- `INIT() { /* code */ }`: Defines an initializer function, run before all tests and untimed.
	- `TESTINIT`: Predefined macro (re-define yourself) that is put at the very start of every test. Timed.
	- `TESTCLEAN`: Predefined macro (re-define yourself) that is put at the end of every test. Timed.

### Example

This was used for the graphic above, with the command `gcc example.c -o example.exe && example`.

```c
#include <string.h>
#include "tests.h"

char* str;

// Defines an init function that runs before all tests and doesn't get timed.
INIT() {
	str = malloc(sizeof(char) * 51);
	str[50] = 0;
}

// Tests are started with TEST(char name[]) and ended with TEND(). This starts a new function, and it is advised to INDENT YOUR CODE in the test!
TEST("Asserts")
	// assert(bool passed)
	assert(1 + 1 == 2);
	// asserteq(any_num_type num1, any_num_type num2)
	asserteq(1 + 1, 2.0f);
TEND() // You can also omit the brackets, just used here for aesthetics.

// Subtests help you allocate resources for a test and make mini tests along the way.
TEST("Subtests")
	// subtest(char name[], bool passed)
	subtest("Subtraction", 2 - 1 == 1);

	// substart(char name[])
	substart("Multiplication");
		assert(1 * 2 == 2);
		asserteq(2 * 1, 2);
	// subend(bool passed)
	subend(1);
TEND()

// The test suite is built to catch 
TEST("Signal Catching")
	subtest("Proper access", str[10]);
	subtest("Access Violation", str[60]);
TEND()

// Test constructors and destructors, defined in macros. These, unlike INIT(), run every test and you can put whatever into them.
#undef TESTINIT
#define TESTINIT char* s = 
#undef TESTCLEAN
#define TESTCLEAN assert(s[3] == 0);

TEST("Test Initializers")
	"lol";

	assert(strlen(s), 3);
TEND()

#include "tests_end.h"
```

More examples of real-world use can be found at https://github.com/aqilc/jsc/tree/main/test/api.

> Note: Known issue: Any `assert`s before a subtest count for that subtest. Currently no fix because of the structure of the framework.
