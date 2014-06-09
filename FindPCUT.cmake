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

#.rst:
# FindPCUT
# --------
#
# Find PCUT - Plain C unit testing - framework.
#
# Find the native PCUT includes and library.  Once done this will define
#
# ::
#
#   PCUT_INCLUDE_DIRS  - where to find pcut/pcut.h, etc.
#   PCUT_LIBRARIES     - List of libraries when using PCUT.
#   PCUT_PREPROCESSOR  - Preprocessor for compilers without __COUNTER__
#   PCUT_FOUND         - True if PCUT found.
#
#
#
# An includer may set PCUT_ROOT to a PCUT installation root to tell this
# module where to look.

include(FindPackageHandleStandardArgs)
include(CheckCSourceCompiles)

find_package_handle_standard_args(PCUT
	FOUND_VAR PCUT_FOUND
	REQUIRED_VARS PCUT_LIBRARIES PCUT_INCLUDE_DIRS PCUT_PREPROCESSOR)

set(PCUT_NAMES pcut)

find_path(PCUT_INCLUDE_DIRS
	NAMES pcut/pcut.h
	HINTS ${PCUT_ROOT}
	PATH_SUFFIXES include)
find_library(PCUT_LIBRARIES
	NAMES ${PCUT_NAMES}
	HINTS ${PCUT_ROOT}
	PATH_SUFFIXES lib)
find_program(PCUT_PREPROCESSOR
	NAMES pcutpp
	HINTS ${PCUT_ROOT}
	PATH_SUFFIXES bin)


if(PCUT_INCLUDE_DIRS)
	set(PCUT_FOUND true)
else()
	set(PCUT_FOUND false)
endif()


set(PCUT_SOURCE_CC_SUPPORTS_COUNTER
"
#ifndef __COUNTER__
#error \"Macro __COUNTER__ is not supported.\"
#endif

#define JOIN(a, b) a##b
#define MAKE_VAR_NAME(number) JOIN(no__counter__in_your_compiler, number)

#define DECLARE_VAR(number, value) static int MAKE_VAR_NAME(number) = (value)

// We assume that __COUNTER__ starts at 0 or 1, thus
// we can assume that variables with indices 2 and 3 will
// be available.

DECLARE_VAR(__COUNTER__, 0);
DECLARE_VAR(__COUNTER__, 1);
DECLARE_VAR(__COUNTER__, 2);
DECLARE_VAR(__COUNTER__, 3);
DECLARE_VAR(__COUNTER__, 4);
DECLARE_VAR(__COUNTER__, 5);

int main() {
	int sum = MAKE_VAR_NAME(2) + MAKE_VAR_NAME(3);
	return 0;
}
"
)

check_c_source_compiles("${PCUT_SOURCE_CC_SUPPORTS_COUNTER}"
	PCUT_CC_SUPPORTS_COUNTER)



function(add_pcut_executable executable sources)
	set(sources ${ARGV})
	list(REMOVE_AT sources 0)
	if(PCUT_CC_SUPPORTS_COUNTER)
		add_executable(${executable} ${sources})
	else()
		set(pp_sources)
		foreach(source ${sources})
			get_filename_component(source_base ${source} PATH)
			file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/${source_base})
			get_filename_component(source_full ${source} ABSOLUTE)
			set(outfile_i ${CMAKE_CURRENT_BINARY_DIR}/${source}.i)
			set(outfile_c ${CMAKE_CURRENT_BINARY_DIR}/${source}.pcut.c)
			list(APPEND pp_sources ${outfile_c})
			add_custom_command(
				OUTPUT ${outfile_i}
				COMMAND ${CMAKE_C_COMPILER}
					ARGS
						-E -o ${outfile_i}
						-I${PCUT_INCLUDE_DIRS}
						${source_full}
				DEPENDS ${source}
			)
			add_custom_command(
				OUTPUT ${outfile_c}
				COMMAND $<TARGET_FILE:pcutpp>
					ARGS
						<${outfile_i}
						>${outfile_c}
				DEPENDS ${outfile_i} ${PCUT_PREPROCESSOR}
			)
		endforeach()
		add_executable(${executable} ${pp_sources})
	endif()
	target_link_libraries(${executable} ${PCUT_LIBRARIES})
endfunction()
