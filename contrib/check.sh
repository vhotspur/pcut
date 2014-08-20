#!/bin/sh
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

run_echo() {
	echo "[exec]:" "$@" >&2
	"$@"
}

check_it() {
	echo ">>>> Running $1 configuration" >&2
	rm -rf "build.$1"
	mkdir "build.$1"
	(
		cd "build.$1"
		set -o errexit
		
		$RUN env $2 cmake $3 ..
		$RUN make all $4
	)
	if [ $? -eq 0 ]; then
		OKAY_CONFIGS="$OKAY_CONFIGS $1"
	else
		FAILED_CONFIGS="$FAILED_CONFIGS $1"
	fi
}

check_build_types() {
	check_it "$1.default" "$2" "$3" ""
	check_it "$1.debug" "$2" "$3 -DCMAKE_BUILD_TYPE=Debug" ""
	check_it "$1.release" "$2" "$3 -DCMAKE_BUILD_TYPE=Release" "test"
}

has_compiler() {
	which "$1" >/dev/null 2>/dev/null
}
		

RUN=run_echo
OKAY_CONFIGS=""
FAILED_CONFIGS=""
MISSING_COMPILERS=""

# Check with the default compiler (on Linux, that is often GCC)
check_build_types "default" "" ""

# Check with other compiler available
for compiler in gcc clang tcc; do
	if has_compiler $compiler; then
		check_build_types "$compiler" "CC=$compiler" ""
	else
		MISSING_COMPILERS="$MISSING_COMPILERS $compiler"
	fi
done

(
	echo; echo; echo
	echo "====== Summary ====="
	echo "==> Success:$OKAY_CONFIGS"
	echo "==> Failure:$FAILED_CONFIGS"
	if [ -n "$MISSING_COMPILERS" ]; then
		echo "==> Skipped:$MISSING_COMPILERS"
	fi
	echo
) >&2

