PCUT: Plain C Unit Testing mini-framework
=========================================

PCUT is a very simple framework for unit testing of C code.
PCUT itself is written in plain C and the main target when writing the
framework was easiness of use.
The framework is definitely not complete: it is possible only to write
tests and group them into suites and nothing more.
I do not promise to turn PCUT into a complete testing framework but I hope
I will be able to continually add various features that can be found
in other unit testing tools for C.





Main goal - simple to use
-------------------------

Let's illustrate how PCUT aims to be simple when creating unit tests for
function ``intmin`` that ought to return smaller of its two arguments.::

	int intmin(int a, int b) {
		return a > b ? b : a;
	}
	
Individual test-cases for such method could cover following cases: getting
minimal of

- two same numbers
- negative and positive number
- two negative numbers
- two positive numbers
- "corner case" numbers, such as minimal and maximal represented number

In PCUT, that would be translated to the following code:::

	#include <pcut/test.h>
	#include <limits.h>
	/* Other include to have declaration of intmin */
	
	PCUT_INIT

	PCUT_TEST_SUITE("intmin() tests");

	PCUT_TEST(same_number) {
		PCUT_ASSERT_INT_EQUALS(719, intmin(719, 719) );
		PCUT_ASSERT_INT_EQUALS(-4589, intmin(-4589, -4589) );
	}
	
	PCUT_TEST(positive_and_negative) {
		PCUT_ASSERT_INT_EQUALS(-5, intmin(-5, 71) );
		PCUT_ASSERT_INT_EQUALS(-17, intmin(423, -17) );
	}
	
	PCUT_TEST(same_sign) {
		PCUT_ASSERT_INT_EQUALS(22, intmin(129, 22) );
		PCUT_ASSERT_INT_EQUALS(-37, intmin(-37, -1) );
	}
	
	PCUT_TEST(corner_cases) {
		PCUT_ASSERT_INT_EQUALS(INT_MIN, intmin(INT_MIN, -1234) );
		PCUT_ASSERT_INT_EQUALS(9876, intmin(9876, INT_MAX) );
	}

	PCUT_MAIN

And that's all.
As you can see, no need to manually add tests or to create the ``main``
function that launches the tests.
Everything is done magically via the ``PCUT_INIT``, ``PCUT_MAIN`` and
``PCUT_TEST`` macros.
	
All that remains to be done is to compile this code and link it with
``libpcut``.
Result would be an executable that runs the tests and reports results.





Creating tests with PCUT
------------------------

Will be described in more detail later, for now see examples in
``tests/`` directory.
Individual files represents features available in PCUT.

``tested.h`` and ``tested.c``
	The library of functions we want to test.

``simple.c``
	Example similar to the one above.
	No explicit test suite is created, everything resides in single file.

``suites.c``
	Single file example where two suites are created to separate individual
	tests.

``suite1.c``, ``suite2.c`` and ``suite_all.c``
	Example how to separate tests and suites into more files.
	
``null.c``
	Example of a test that does not fail but rather aborts the whole
	program (accessing NULL pointer).





Running PCUT
------------

Once the test is linked with ``libpcut``, you will have an executable
binary that is able to run the tests and report results.
Running the executable with no parameters will launch all tests,
each test is run in a separate process.

It is also possible to run only a single test or suite.
So far, these are identified by numbers that are assigned by the PCUT
framework.
To see those numbers, run your executable with ``-l`` option.
The output for the ``suites.c`` example mentioned above looks like this:::

	  Suite `Default' [1]
	  Suite `Tests for intpow()' [2]
	    Test `zero_exponent' [3]
	    Test `one_exponent' [4]
	  Suite `Tests for intmin()' [5]
	    Test `test_min' [6]
 
To run a single test only, run with ``-t`` parameter followed by test
id.
Thus, to run only the ``zero_exponent`` test, execute with ``-t3``
(notice that there is no space between the parameter and the number).

Running single test suite is possible with ``-s`` parameter.

Please notice, that running single test will not spawn a new process:
if the test aborts (such as the ``null.c`` example in ``tests/``) no
message would be printed (the shell will inform you about abnormal
program termination).
Running the whole suite does not have this drawback, though.





Using PCUT in more detail
-------------------------

TODO: describe available macros (creating tests, asserts)

TODO: compiling, linking, installing





Comparison with other C unit testing frameworks
-----------------------------------------------
TODO: illustrate on some example how much extra code is needed
(compare with Check, AceUnit, CUnit?)





Internals
---------

TODO: describe the magic behind using ``__COUNTER__``
