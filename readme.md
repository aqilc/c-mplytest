<div align="center">
	<h1>c-mplytest</h1>
	<p>Testing in C, made easy, fast and pretty.</p>
</div>


```c
#include "tests.h"

TEST("Test Name") {
	assert(1 == 1);
	asserteq(1 + 1, 2);
	assertmemeq(&(int) { 0x2 }, { 0x0, 0x0, 0x0, 0x2 });
	assertstreq("hi", "hi");
}

TEST("Subtests") {
	SUB("1 == 1") {
		assert(1 == 1);
	}

	SUB("Should you use this library?") {
		assert(!!"yes!");
	}
}

#include "tests_end.h"
```

## ✨ Features

- [x] Works in MSVC, MinGW, GCC, and any other compiler that supports `__COUNTER__`.
- [x] ↳ No need to copy every test function name into "suites" and/or other BS.
- [x] Clean test output with fully featured timing, down to the nanosecond.
- [x] **Microbenchmarks** alongside tests!!!
- [x] Properly and easily allows you to name tests and parts of tests with spaces and any other character you can dream of.
- [x] **Catches SIGSEGVs** and other signals without stopping the tests, properly failing the appropriate test and subtest.
- [x] Personally tested over thousands of times, been used in very versatile situations and adapted for most things.

## 🧰 Usage

1. Put `#include <tests.h>` at the start of your file before all other imports.
2. Put `#include <tests_end.h>` at the very end of your file.

> **Note**
> `tests.h` imports `stdio.h` automatically. You don't need to import it again to use IO.

## 📖 API

- `TEST("Test name") {}`: Defines a test.
- `assert(bool passed)`: A check performed per test.
	- `asserteq(any_num_type n1, any_num_type n2)`: Can use any type of number for arguments, including Enums.
	- `assertmemeq(ptr, byte array or string)`: Tests to see if memory location matches the specific bytes specified.
	- `assertstreq("str1", "str2")`: Checks if two strings are exactly the same.
- `SUB("Subtest name") {}`: Defines a "sub" test which allows you to test individual functionality with setup.
- `BENCH("Benchmark Name") {}`: Lets you microbenchmark a small piece of code alongside tests.
	- `SUBBENCH() {}`: Lets you benchmark code associated with a subtest.
- **Advanced:**
	- `INIT() { /* code */ }`: Defines an initializer function, run before all tests and untimed.
	- `benchiters(int x)`: Sets the amount of iterations the proceeding benchmarks will go through.

## Demo

![Image of utility](https://github.com/aqilc/c-mplytest/assets/32044067/a32c1679-fe10-42cd-9ed1-9cd903b3add6)

This was used for the graphic above, with the command `gcc example.c -o example.exe && example`.

```c
#include <string.h>
#include "tests.h"

char* str;

// Defines an init function that runs before all tests and doesn't get timed.
INIT() {
	str = malloc(sizeof(char) * 5001);
	str[5001] = 0;
}

// Tests are started with TEST("Test Name") {}
TEST("Asserts") {
	assert(1 + 1 == 2);
	asserteq(1 + 1, 2.0f);
}

// Tests with subtests are different from normal tests with just asserts - Only the contents of the subtest are timed.
TEST("Subtests") {
	char* setup = malloc(2000); // This is not timed.
	
	SUB("Subtraction") { assert(2 - 1 == 1); }
	SUB("Multiplication") {
		assert(1 * 2 == 2);
		asserteq(2 * 1, 2);
	}
	free(setup); // Not timed.
}

// This test is built to catch SIGSEGVs
TEST("Signal Catching") {
	SUB("Proper access") { assert(str[10]); }
	SUB("Access Violation") { assert(* (char*) NULL); }
}

TEST("Benchmarks") {
	BENCH("memset") {
		memset(str, 'h', 5000);
	}

	benchiters(200000); // You can adjust the benchmark iterations live.

	SUB("Subtest Benchmark") { asserteq(strlen(str), 5000); }
	SUBBENCH() { // Note: No arguments
		strlen(str);
	}
}

#include "tests_end.h"
```

```cpp
> gcc -o demo.exe demo.c && demo
> clang -o demo.exe demo.c && demo
> cl /Fedemo.exe demo.c && demo
> clang-cl -o demo.exe demo.c && demo
```

## Notes

- Any `assert`s before a subtest count for that subtest.
- Down to rename "subtests", I just couldn't come up with something better :(
