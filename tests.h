/*
 * tests.h v3.2.4 - Aqil Contractor @AqilC 2024
 * Licenced under Attribution-NonCommercial-ShareAlike 3.0
 * 
 * This file is the beginning file of the 'tests.h' testing framework made by Aqil Contractor. To use this framework,
 * include this file at the top of your testing file, and 'tests_end.h' at the end of your testing file.
 *
 * Example:
 * #include "tests.h"
 *
 * TEST("Test name") {
 * 	assert(1 == 1);
 * }
 *
 * TEST("Another test") {
 * 	assert(1 == 1);
 * 	assert(1 == 2);
 * }
 *
 * #include "tests_end.h"
 *
 * This will output:
 * 1) Test name (test.c:3)                          PASS   0001 ns
 * 2) Another test (test.c:7)
 *   (test.c:9) Error: Assertion '1 == 2' failed.
 * 1 / 2 tests passed. Took 0.00 ms
 *
 *
 * Version History:
 * - Other versions only include minor changes, bug fixes, or small features.
 * - 3.2.0 - Remove some macros used internally.
 * - 3.1.0 - Change test output colors.
 * - 3.0.0 - Change test syntax completely, allowing SUB("subtest name") {} instead of subtest("subtest name"); ... subend();
 * - 2.3.0 - Add a way to run a test without printing output, just to time or debug it.
 * - 2.0.0 - Add benchmarks.
 * - 2.0.0 beta2 - Move around includes.
 * - 1.0.0 - Add assertmemeq, assertstreq, and asserteq.
 */



#ifndef NO_PRINT
#include <stdio.h>
#else
#define printf(...) 0
#define puts(...) 0
#define snprintf(...) 0
#endif


#include <setjmp.h>

jmp_buf owo;
unsigned int sigthing = 0;
void* sigaddr = 0;

double tests_starttime;
double tests_totaltime;
_Bool subtest_done = 0;
int tests_benchprogress;
int tests_benchiters = 1000;


char testtempbuf[1000];

int subtests_run = 0;
int subtests_passed = 0;
int asserts = 0;
_Bool assert_aborted = 0;
int testoutputwidth = 70;
char* tests_cursubtest = NULL;


// https://stackoverflow.com/a/53388334/10013227
#if defined(_MSC_VER) && !defined(__clang__)
void init__(void) {}
void init(void);
#pragma comment(linker, "/alternatename:init=init__")
#else
void init(void) __attribute__((weak)); 
#endif
#define INIT() void init(void)





// https://stackoverflow.com/a/33206814/10013227
// Terminal colors defines
#define TERMREDBGBLACKBOLD "\033[1;41;30m"
#define TERMGREENBGBLACK   "\033[42;30m"
#define TERMWHITEBGBLACK   "\033[47;30m"
#define TERMBLACKBGWHITE   "\033[40;37m"
#define TERMREDBGBLACK     "\033[41;30m"
#define TERMBLUEBG         "\033[48;5;27;30m"
#define TERMLIGHTBLUEBG    "\033[48;5;69;30m"
#define TERMGREENBOLD      "\033[1;32m"
#define TERMREDBOLD        "\033[1;31m"
#define TERMBLUEBOLD       "\033[1;34m"
#define TERMGRAYBOLD       "\033[1;90m"
#define TERMYELLOW         "\033[33m"
#define TERMBLUE           "\033[38;5;69m"
#define TERMGRAY           "\033[90m"
#define TERMPINK           "\033[38;5;201m"
#define TERMCYANBOLD       "\033[1;33m"
#define TERMRESET          "\033[0m"


// used to be 70
#define TESTNAMELIMIT testoutputwidth





#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

// https://stackoverflow.com/a/2349941/10013227
#ifdef _WIN32
#include <windows.h>

static inline long double get_precise_time() {
	LARGE_INTEGER t;
	static LARGE_INTEGER f;
	if(!f.QuadPart) QueryPerformanceFrequency(&f);
	QueryPerformanceCounter(&t);
	return (long double)t.QuadPart/(long double)f.QuadPart;
}

LONG WINAPI plswork(PEXCEPTION_POINTERS pinfo) {
	sigthing = pinfo->ExceptionRecord->ExceptionCode;
	longjmp(owo, 1);
}
#else
#include <sys/time.h>
#include <sys/resource.h>
#include <signal.h>

static inline long double get_precise_time() {
	struct timeval t;
	struct timezone tzp;
	gettimeofday(&t, &tzp);
	return (long double)t.tv_sec + (long double)t.tv_usec*(long double)1e-6;
}

static void handler (int sig, siginfo_t* bruh, void* idc) {
	sigthing = sig;
	sigaddr = bruh->si_addr;
	longjmp(owo, 1);
}
#endif



// PURELY FOR DEBUGGING WITH GDB OR ANOTHER COMMAND LINE DEBUGGER
#if defined(__linux__)
// https://linux.die.net/man/3/malloc_usable_size
#include <malloc.h>
static inline size_t msizeof(const void *p) {
    return malloc_usable_size((void*)p);
}
#elif defined(__APPLE__)
// https://www.unix.com/man-page/osx/3/malloc_size/
#include <malloc/malloc.h>
static inline size_t msizeof(const void *p) {
    return malloc_size(p);
}
#elif defined(_WIN32)
// https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/msize
#include <malloc.h>
static inline size_t msizeof(const void *p) {
    return _msize((void *)p);
}
#endif


char* tests_print_mem(void* mem, int size) {
	int place = 0;
	while(size --) place += snprintf(testtempbuf + place, 1000, "0x%02X ", *(unsigned char*) mem++);
	testtempbuf[place - 1] = 0;
	return testtempbuf;
}



// ------------------------------------------------------------------- SUBTESTS -------------------------------------------------------------------
#define SUBTESTINDENT "  "

// Custom assert, requires something to be true to continue with the test.
#define assert(x) do { if(x) { asserts ++; break; } puts(""); if(subtests_run) printf(SUBTESTINDENT); puts("("__FILE__":"TOSTRING(__LINE__)") "TERMREDBOLD"Error:"TERMRESET" Assertion '"#x"' failed."); /*subtests_run = 0; subtests_passed = 0;*/ tests_starttime = get_precise_time(); assert_aborted = 1; } while (0)
#define asserteq(x, y) do { if((x) == (y)) { asserts ++; break; } puts(""); if(subtests_run) printf(SUBTESTINDENT); printf("("__FILE__":"TOSTRING(__LINE__)") "TERMREDBOLD"Error:"TERMRESET" '"#x"'(%d) != '"#y"'(%d).", (int) (x), (int) (y)); /*subtests_run = 0; subtests_passed = 0;*/ tests_starttime = get_precise_time(); assert_aborted = 1; } while (0)
#define assertmemeq(x, ...) do { if(!memcmp(x, (char[]) __VA_ARGS__, sizeof((char[]) __VA_ARGS__))) { asserts ++; break; } puts(""); if(subtests_run) printf(SUBTESTINDENT); printf("("__FILE__":"TOSTRING(__LINE__)") "TERMREDBOLD"Error:"TERMRESET" '"#x"'(%s) != '"#__VA_ARGS__"'.", tests_print_mem((x), sizeof((char[]) __VA_ARGS__))); /*subtests_run = 0; subtests_passed = 0;*/ tests_starttime = get_precise_time(); assert_aborted = 1; } while (0)
#define assertstreq(x, ...) do { if(!strcmp(x, __VA_ARGS__)) { asserts ++; break; } puts(""); if(subtests_run) printf(SUBTESTINDENT); printf("("__FILE__":"TOSTRING(__LINE__)") "TERMREDBOLD"Error:"TERMRESET" '"#x"'(\"%s\") != '"#__VA_ARGS__"'(\"%s\").", x, __VA_ARGS__); /*subtests_run = 0; subtests_passed = 0;*/ tests_starttime = get_precise_time(); assert_aborted = 1; } while (0)

// A sub test, which checks if something is going according to plan but if it's not, it can still continue
// #define subtest(x, y) do {\
// 	SUBTESTINIT(x);\
// 	if((y) && !assert_aborted) SUBTESTPASSOUTPUT(y)\
// 	printf("\n"SUBTESTINDENT TERMREDBOLD"Subtest '" x "' (#%d) failed."TERMRESET, subtests_run);\
// 	tests_starttime = get_precise_time(); asserts = 0;\
// } while (0)

// For subtests with multiple checks
#define SUB(x) subtests_run++;\
	printf("\n" SUBTESTINDENT x TERMRESET " %-*s", /*subtests_run,*/(int) (TESTNAMELIMIT - sizeof(SUBTESTINDENT) + 1 - sizeof(""x"") + 1), "");\
	tests_cursubtest = (x); tests_starttime = get_precise_time(); while((subtest_done = !subtest_done) || subend_())
static inline _Bool subend_(void) {
	// printf("asserts: %d, subtest_done: %d, assert_aborted: %d\n", asserts, subtest_done, assert_aborted);
	if(!assert_aborted) {
		long double passed_time = (get_precise_time() - tests_starttime);
		tests_totaltime += passed_time;
		double tmul = 1000.0;
		char* timeunit = "ms";
		if(passed_time < 9.9e-6) tmul = 1000000000.0, timeunit = "ns";
		else if(passed_time < 9.9e-3) tmul = 1000000.0, timeunit = "\u03BCs";
		if(asserts < 12)
			printf("%.*s" TERMGREENBOLD "%.*s" TERMRESET TERMGREENBGBLACK " PASS "TERMRESET" "TERMBLUEBG" %04.0Lf %s " TERMRESET,
				asserts * 2,
				"\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b",
				asserts * 4,
				"\u2713 \u2713 \u2713 \u2713 \u2713 \u2713 \u2713 \u2713 \u2713 \u2713 \u2713 \u2713 \u2713 \u2713 \u2713 \u2713 \u2713 \u2713 \u2713 \u2713 \u2713 \u2713 \u2713 \u2713 \u2713 \u2713 \u2713 \u2713 \u2713 \u2713 \u2713 \u2713 \u2713 \u2713 \u2713 \u2713 \u2713 \u2713 \u2713 ",
				passed_time * tmul, timeunit);
		else
			printf("%.*s\b\b\b\b" TERMYELLOW "%dx " TERMGREENBOLD "\u2713 "TERMRESET TERMGREENBGBLACK" PASS "TERMRESET" "TERMBLUEBG" %04.0Lf %s " TERMRESET,
			/*Length of the number in `assert`*/ asserts < 10 ? 1 : asserts < 100 ? 2 : asserts < 1000 ? 3 : asserts < 10000 ? 4 : asserts < 100000 ? 5 : asserts < 1000000 ? 6 : asserts < 10000000 ? 7 : asserts < 100000000 ? 8 : asserts < 1000000000 ? 9 : 10,
			"\b\b\b\b\b\b\b\b\b\b\b\b", asserts, passed_time * tmul, timeunit);
		subtests_passed++;
		asserts = 0;
		subtest_done = 0;
		tests_starttime = get_precise_time();
		return 0;
	}
	subtest_done = 0;
	assert_aborted = 0;
	printf("\n"SUBTESTINDENT TERMREDBOLD"Subtest '%s' (#%d) failed."TERMRESET, tests_cursubtest, subtests_run);
	tests_starttime = get_precise_time(); asserts = 0;
	return 0;
}


#define benchiters(x) tests_benchiters = (x)
#define BENCH(x) printf("\n" SUBTESTINDENT TERMGRAY "Bench: " x TERMRESET " %-*s", (int) (TESTNAMELIMIT - 7 - sizeof(SUBTESTINDENT) + 1 - sizeof(""x"") + 1), ""); tests_starttime = get_precise_time(); tests_benchprogress = 0; while ((tests_benchprogress++) < tests_benchiters || benchend_())
#define SUBBENCH() printf("\n" SUBTESTINDENT SUBTESTINDENT TERMGRAY "Bench for %s" TERMRESET " %-*s", tests_cursubtest, (int) (TESTNAMELIMIT - 10 - sizeof(SUBTESTINDENT SUBTESTINDENT) + 1 - strlen(tests_cursubtest)), ""); tests_starttime = get_precise_time(); tests_benchprogress = 0; while ((tests_benchprogress++) < tests_benchiters || benchend_())

static inline _Bool benchend_() {
	long double passed_time = (get_precise_time() - tests_starttime);
	long double time_per_iter = passed_time / (long double) tests_benchiters;
	tests_totaltime += passed_time;
	
	double ttmul = 1000.0;
	char* ttimeunit = "ms";
	if(passed_time < 9.9e-6) ttmul = 1000000000.0, ttimeunit = "ns";
	else if(passed_time < 9.9e-3) ttmul = 1000000.0, ttimeunit = "\u03BCs";
	
	double tmul = 1000.0;
	char* timeunit = "ms";
	if(time_per_iter < 9.9e-6) tmul = 1000000000.0, timeunit = "ns";
	else if(time_per_iter < 9.9e-3) tmul = 1000000.0, timeunit = "\u03BCs";

	printf("\b\b\b\b\b\b\b\b%.*s" TERMYELLOW "%dx " TERMBLUE "%04.0Lf %s/iter" TERMRESET " " TERMBLUEBG " %04.0Lf %s " TERMRESET, tests_benchiters < 10 ? 1 : tests_benchiters < 100 ? 2 : tests_benchiters < 1000 ? 3 : tests_benchiters < 10000 ? 4 : tests_benchiters < 100000 ? 5 : tests_benchiters < 1000000 ? 6 : tests_benchiters < 10000000 ? 7 : tests_benchiters < 100000000 ? 8 : tests_benchiters < 1000000000 ? 9 : 10, "\b\b\b\b\b\b\b\b\b\b\b\b", tests_benchiters, time_per_iter * tmul, timeunit, passed_time * ttmul, ttimeunit);
	return 0;
}

// ---------------------------------------------------- Macro based testing framework starts here ----------------------------------------------------

#define TESTCONCAT(a, b) a##b
#define TESTFUNCRET int
#define TESTFUNCARGS (void)

#define TEST(name) TEST_(name, __COUNTER__)

#define TEST_(name, N) \
static inline void TESTCONCAT(test_internal_, N)(void);\
int TESTCONCAT(test_, N)(void) {\
	asserts = 0;\
	printf(TERMCYANBOLD "%d)" TERMRESET " " name " " TERMGRAY "(" __FILE__ ":" TOSTRING(__LINE__) ")" TERMRESET " %-*s" TERMRESET, N + 1, (int) (TESTNAMELIMIT - sizeof(name " (" __FILE__ ":" TOSTRING(__LINE__) ")") + 1 - 3), "");\
	tests_starttime = get_precise_time();\
	TESTCONCAT(test_internal_, N)();\
	if(assert_aborted == 1) { puts(""); assert_aborted = 0; return 1; }\
	long double passed_time = (get_precise_time() - tests_starttime); /* Measures the amount of clocks the test took*/\
	tests_totaltime += passed_time;\
	double tmul = 1000.0;\
	char* timeunit = "ms";\
	if(passed_time < 9.9e-6) tmul = 1000000000.0, timeunit = "ns";\
	else if(passed_time < 9.9e-3) tmul = 1000000.0, timeunit = "\u03BCs";\
	if(!subtests_run)\
		if(asserts < 12)\
			printf("%.*s"TERMGREENBOLD"%.*s"TERMRESET TERMGREENBGBLACK" PASS "TERMRESET" "TERMBLUEBG" %04.0Lf %s "TERMRESET"\n", asserts * 2,\
				"\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b", asserts * 4,\
				"\u2713 \u2713 \u2713 \u2713 \u2713 \u2713 \u2713 \u2713 \u2713 \u2713 \u2713 \u2713 \u2713 \u2713 \u2713 \u2713 \u2713 \u2713 \u2713 \u2713 \u2713 \u2713 \u2713 \u2713 \u2713 \u2713 \u2713 \u2713 \u2713 \u2713 \u2713 \u2713 \u2713 \u2713 \u2713 \u2713 \u2713 \u2713 \u2713 ",\
				passed_time * tmul, timeunit);/* If this was a straight test with no subtests, just print out test passed message*/\
		else\
			printf("%.*s\b\b\b\b" TERMYELLOW "%dx " TERMGREENBOLD "\u2713 "TERMRESET TERMGREENBGBLACK" PASS "TERMRESET" "TERMBLUEBG" %04.0Lf %s " TERMRESET "\n",\
			/*Length of the number in `assert`*/ asserts < 10 ? 1 : asserts < 100 ? 2 : asserts < 1000 ? 3 : asserts < 10000 ? 4 : asserts < 100000 ? 5 : asserts < 1000000 ? 6 : asserts < 10000000 ? 7 : asserts < 100000000 ? 8 : asserts < 1000000000 ? 9 : 10,\
			"\b\b\b\b\b\b\b\b\b\b\b\b", asserts, passed_time * tmul, timeunit);\
	else {\
		int allpassed = subtests_run == subtests_passed;/* Otherwise check subtests*/\
		if(allpassed) puts("");\
		else printf("\n"SUBTESTINDENT TERMREDBGBLACK" ERROR "TERMRESET" %d subtests tests failed.\n", subtests_run - subtests_passed);\
		subtests_run = 0;\
		subtests_passed = 0;\
		if(!allpassed) return 1;\
	}\
	return 0;\
}\
static inline void TESTCONCAT(test_internal_, N)(void)


#define TESTFUNGEN__(N) TESTFUNC##N
#define TESTFUNGEN_(N) TESTFUNGEN__(N)
#define TESTFUNGEN() TESTFUNGEN_(__COUNTER__)

#define TDELIM ,
#define TESTFUNC0
#define TESTFUNC1 test_0
#define TESTFUNC2 TESTFUNC1 TDELIM test_1
#define TESTFUNC3 TESTFUNC2 TDELIM test_2
#define TESTFUNC4 TESTFUNC3 TDELIM test_3
#define TESTFUNC5 TESTFUNC4 TDELIM test_4
#define TESTFUNC6 TESTFUNC5 TDELIM test_5
#define TESTFUNC7 TESTFUNC6 TDELIM test_6
#define TESTFUNC8 TESTFUNC7 TDELIM test_7
#define TESTFUNC9 TESTFUNC8 TDELIM test_8
#define TESTFUNC10 TESTFUNC9 TDELIM test_9
#define TESTFUNC11 TESTFUNC10 TDELIM test_10
#define TESTFUNC12 TESTFUNC11 TDELIM test_11
#define TESTFUNC13 TESTFUNC12 TDELIM test_12
#define TESTFUNC14 TESTFUNC13 TDELIM test_13
#define TESTFUNC15 TESTFUNC14 TDELIM test_14
#define TESTFUNC16 TESTFUNC15 TDELIM test_15
#define TESTFUNC17 TESTFUNC16 TDELIM test_16
#define TESTFUNC18 TESTFUNC17 TDELIM test_17
#define TESTFUNC19 TESTFUNC18 TDELIM test_18
#define TESTFUNC20 TESTFUNC19 TDELIM test_19
#define TESTFUNC21 TESTFUNC20 TDELIM test_20
#define TESTFUNC22 TESTFUNC21 TDELIM test_21
#define TESTFUNC23 TESTFUNC22 TDELIM test_22
#define TESTFUNC24 TESTFUNC23 TDELIM test_23
#define TESTFUNC25 TESTFUNC24 TDELIM test_24
#define TESTFUNC26 TESTFUNC25 TDELIM test_25
#define TESTFUNC27 TESTFUNC26 TDELIM test_26
#define TESTFUNC28 TESTFUNC27 TDELIM test_27
#define TESTFUNC29 TESTFUNC28 TDELIM test_28
#define TESTFUNC30 TESTFUNC29 TDELIM test_29
#define TESTFUNC31 TESTFUNC30 TDELIM test_30
#define TESTFUNC32 TESTFUNC31 TDELIM test_31
#define TESTFUNC33 TESTFUNC32 TDELIM test_32
#define TESTFUNC34 TESTFUNC33 TDELIM test_33
#define TESTFUNC35 TESTFUNC34 TDELIM test_34
#define TESTFUNC36 TESTFUNC35 TDELIM test_35
#define TESTFUNC37 TESTFUNC36 TDELIM test_36
#define TESTFUNC38 TESTFUNC37 TDELIM test_37
#define TESTFUNC39 TESTFUNC38 TDELIM test_38
#define TESTFUNC40 TESTFUNC39 TDELIM test_39
#define TESTFUNC41 TESTFUNC40 TDELIM test_40
#define TESTFUNC42 TESTFUNC41 TDELIM test_41
#define TESTFUNC43 TESTFUNC42 TDELIM test_42
#define TESTFUNC44 TESTFUNC43 TDELIM test_43
#define TESTFUNC45 TESTFUNC44 TDELIM test_44
#define TESTFUNC46 TESTFUNC45 TDELIM test_45
#define TESTFUNC47 TESTFUNC46 TDELIM test_46
#define TESTFUNC48 TESTFUNC47 TDELIM test_47
#define TESTFUNC49 TESTFUNC48 TDELIM test_48
#define TESTFUNC50 TESTFUNC49 TDELIM test_49
#define TESTFUNC51 TESTFUNC50 TDELIM test_50
#define TESTFUNC52 TESTFUNC51 TDELIM test_51
#define TESTFUNC53 TESTFUNC52 TDELIM test_52
#define TESTFUNC54 TESTFUNC53 TDELIM test_53
#define TESTFUNC55 TESTFUNC54 TDELIM test_54
#define TESTFUNC56 TESTFUNC55 TDELIM test_55
#define TESTFUNC57 TESTFUNC56 TDELIM test_56
#define TESTFUNC58 TESTFUNC57 TDELIM test_57
#define TESTFUNC59 TESTFUNC58 TDELIM test_58
#define TESTFUNC60 TESTFUNC59 TDELIM test_59
#define TESTFUNC61 TESTFUNC60 TDELIM test_60
#define TESTFUNC62 TESTFUNC61 TDELIM test_61
#define TESTFUNC63 TESTFUNC62 TDELIM test_62
#define TESTFUNC64 TESTFUNC63 TDELIM test_63
#define TESTFUNC65 TESTFUNC64 TDELIM test_64
#define TESTFUNC66 TESTFUNC65 TDELIM test_65
#define TESTFUNC67 TESTFUNC66 TDELIM test_66
#define TESTFUNC68 TESTFUNC67 TDELIM test_67
#define TESTFUNC69 TESTFUNC68 TDELIM test_68
#define TESTFUNC70 TESTFUNC69 TDELIM test_69
#define TESTFUNC71 TESTFUNC70 TDELIM test_70
#define TESTFUNC72 TESTFUNC71 TDELIM test_71
#define TESTFUNC73 TESTFUNC72 TDELIM test_72
#define TESTFUNC74 TESTFUNC73 TDELIM test_73
#define TESTFUNC75 TESTFUNC74 TDELIM test_74
#define TESTFUNC76 TESTFUNC75 TDELIM test_75
#define TESTFUNC77 TESTFUNC76 TDELIM test_76
#define TESTFUNC78 TESTFUNC77 TDELIM test_77
#define TESTFUNC79 TESTFUNC78 TDELIM test_78
#define TESTFUNC80 TESTFUNC79 TDELIM test_79
#define TESTFUNC81 TESTFUNC80 TDELIM test_80
#define TESTFUNC82 TESTFUNC81 TDELIM test_81
#define TESTFUNC83 TESTFUNC82 TDELIM test_82
#define TESTFUNC84 TESTFUNC83 TDELIM test_83
#define TESTFUNC85 TESTFUNC84 TDELIM test_84
#define TESTFUNC86 TESTFUNC85 TDELIM test_85
#define TESTFUNC87 TESTFUNC86 TDELIM test_86
#define TESTFUNC88 TESTFUNC87 TDELIM test_87
#define TESTFUNC89 TESTFUNC88 TDELIM test_88
#define TESTFUNC90 TESTFUNC89 TDELIM test_89
#define TESTFUNC91 TESTFUNC90 TDELIM test_90
#define TESTFUNC92 TESTFUNC91 TDELIM test_91
#define TESTFUNC93 TESTFUNC92 TDELIM test_92
#define TESTFUNC94 TESTFUNC93 TDELIM test_93
#define TESTFUNC95 TESTFUNC94 TDELIM test_94
#define TESTFUNC96 TESTFUNC95 TDELIM test_95
#define TESTFUNC97 TESTFUNC96 TDELIM test_96
#define TESTFUNC98 TESTFUNC97 TDELIM test_97
#define TESTFUNC99 TESTFUNC98 TDELIM test_98
#define TESTFUNC100 TESTFUNC99 TDELIM test_99



/*
	Test format:
TEST("name")
	subtest("name")
TEND()
*/

