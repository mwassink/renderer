#ifndef TEST_HEADER
#define TEST_HEADER
#include <setjmp.h>
jmp_buf env;

#define CHECK(expr) if (!(expr)) longjmp(env, 1);
#define BEGINTEST(name) {if (setjmp(env))  {fail(#name); return;}}
#define ENDTEST(name) {success(#name); return;}


void fail(char* failed_test);
void success(char* success);

void print_report(char* name, const char* title);


#endif
