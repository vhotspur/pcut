#
# Copyright (c) 2014 Vojtech Horky
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# - Redistributions of source code must retain the above copyright
#   notice, this list of conditions and the following disclaimer.
# - Redistributions in binary form must reproduce the above copyright
#   notice, this list of conditions and the following disclaimer in the
#   documentation and/or other materials provided with the distribution.
# - The name of the author may not be used to endorse or promote products
#   derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
# OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
# NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

#
# We expect following variables would be set:
# TEST_EXECUTABLE - PCUT executable with the tests to perform
# EXPECTED_OUTPUT - file with expected stdout from ${TEST_EXECUTABLE}
#

execute_process(
	COMMAND ${TEST_EXECUTABLE}
	OUTPUT_FILE ${TEST_EXECUTABLE}.test.output
	RESULT_VARIABLE test_result
)

if(NOT test_result EQUAL 0)
	message(FATAL_ERROR "Test execution failed for some reason.")
endif()

execute_process(
	COMMAND ${CMAKE_COMMAND} -E compare_files ${TEST_EXECUTABLE}.test.output ${EXPECTED_OUTPUT}
	RESULT_VARIABLE cmp_result
)

if(NOT cmp_result EQUAL 0)
	message(FATAL_ERROR "Output does not match the expected one.")
endif()

