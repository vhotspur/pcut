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

cmake_minimum_required(VERSION 2.6)
if(CMAKE_MAJOR_VERSION GREATER 2)
	cmake_policy(SET CMP0026 OLD)
endif()

include(CheckCCompilerFlag)
enable_testing()

project(PCUT C)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})

set(PCUT_ROOT ${PROJECT_SOURCE_DIR})
find_package(PCUT)

macro(add_cc_flag_when_supported flag variable)
	check_c_compiler_flag(${flag} ${variable})
	if(${variable})
		add_definitions(${flag})
	endif()
endmacro()

set(SELF_TESTS)
function(add_self_test testname sources)
	set(sources ${ARGV})
	list(REMOVE_AT sources 0)
	add_pcut_executable("test-${testname}" ${sources})
	get_property(TEST_LOCATION TARGET "test-${testname}" PROPERTY LOCATION)
	
	if(${UNIX})
		add_test(NAME "${testname}"
			COMMAND "${PROJECT_SOURCE_DIR}/run_test.sh"
				${TEST_LOCATION}
				"${PROJECT_SOURCE_DIR}/tests/${testname}.expected"
		)
	endif()
endfunction()

add_cc_flag_when_supported(-pedantic CC_FLAG_PEDANTIC)
add_cc_flag_when_supported(-Wall CC_FLAG_WARNING_ALL)
add_cc_flag_when_supported(-Wextra CC_FLAG_WARNING_EXTRA)
add_cc_flag_when_supported(-Werror CC_FLAG_WARNING_INTO_ERROR)
add_cc_flag_when_supported(-Wno-variadic-macros CC_FLAG_VARIADIC_MACROS_ARE_OKAY)



set(SOURCES
	src/assert.c
	src/list.c
	src/main.c
	src/print.c
	src/report/report.c
	src/report/tap.c
	src/report/xml.c
	src/run.c
)
if(${UNIX})
	list(APPEND SOURCES src/os/stdc.c src/os/unix.c)
endif()
if(${WIN32})
	list(APPEND SOURCES src/os/stdc.c src/os/generic.c)
endif()


include_directories(include)

add_library(pcut ${SOURCES})
get_property(PCUT_LIBRARIES TARGET pcut PROPERTY LOCATION)

add_executable(pcutpp src/preproc.c)
get_property(PCUT_PREPROCESSOR TARGET pcutpp PROPERTY LOCATION)


add_self_test(alloc tests/alloc.c)
add_self_test(asserts tests/asserts.c)
add_self_test(errno tests/errno.c)
add_self_test(manytests tests/manytests.c)
add_self_test(multisuite tests/suite_all.c tests/suite1.c tests/suite2.c
	tests/tested.c)
add_self_test(null tests/null.c)
add_self_test(nullteardown tests/nullteardown.c)
add_self_test(printing tests/printing.c)
add_self_test(simple tests/simple.c tests/tested.c)
add_self_test(skip tests/skip.c)
add_self_test(suites tests/suites.c tests/tested.c)
add_self_test(teardown tests/teardown.c tests/tested.c)
add_self_test(timeout tests/timeout.c)


install(TARGETS pcut DESTINATION lib)
install(TARGETS pcutpp DESTINATION bin)
install(DIRECTORY include/pcut DESTINATION include)

