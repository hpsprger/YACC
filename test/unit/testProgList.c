/*
 * Cmput 415 - Team YACC
 * Unit tests for the ProgList module.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "Error.h"
#include "ErrorLL.h"
#include "testProgList.h"

extern struct ErrorLL *errors;


char *
test_printProgramListing() 
{
	/* 
	 * WARNING: dependent on the proper function of Error.c and ErrorLL.c
	 * both of which have been vetted before the production of this code.
	 * However, if these tests fail, first assure that it is not these
	 * modules which are causing failure. 
	 */

	/* 
	 * Correctness verified by manual inspection of output file.
	 * TODO: automate correctness checking.
	 */

	FILE *in;
	int i, longLineLen = 10240;
	char *lines[5];
	char line1[] = "program hello (input, output);\n";
	char line2[] = "{ Let's make pretend like a real {error} }}}}\n";
	char line3[] = "The next line is purposefully longer than the default buffer size we set in printProgramListing\n";
	char line4[longLineLen];
	char line5[] = "end. // end";

	for (i = 0; i < longLineLen-2; i++) {
		line4[i] = 'a';
	}
	line4[longLineLen-2] = '\n';
	line4[longLineLen-1] = '\0';

	lines[0] = line1;
	lines[1] = line2;
	lines[2] = line3;
	lines[3] = line4;
	lines[4] = line5;


	in = fopen("./test/unit/ProgListTestFile.txt", "w");

	for (i = 0; i < 5; i++) {
		fprintf(in, "%s", lines[i]);
	}
	
	fclose(in);
	fopen("./test/unit/ProgListTestFile.txt", "r");

       
	errors = NULL;
	recordError("I am testing you.", 2);
	recordError("multiple errors on same line test", 2);
	recordError("That's a lot of a's. Error number 3.", 4);
	printProgramListing(in, "./test/unit/ProgListTestFileOut.lst");
	return NULL;
	
}

char *
test_all_ProgList()
{
	mu_run_test(test_printProgramListing);
	tests_run -= 2;
	return NULL;

}
