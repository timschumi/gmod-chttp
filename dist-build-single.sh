#!/bin/bash -e

BASE_DIR="$(dirname "$(realpath "${0}")")"

if [ "$#" -ne 2 ]; then
	echo "error: Need at least a build target and build type"
	exit 1
fi

# Ensure the output folder exists
mkdir -p "${BASE_DIR}/dist"

BUILD_TARGET="${1}"
BUILD_TYPE="${2}"

BUILD_DIR="chttp-${BUILD_TARGET}-${BUILD_TYPE}"

# Determine the correct file name
OUTPUT_FILE="gmsv_chttp_${BUILD_TARGET}"

rm -rf "${BUILD_DIR}"
mkdir -p "${BUILD_DIR}"
cd "${BUILD_DIR}"
cmake -DCMAKE_TOOLCHAIN_FILE=toolchain-${BUILD_TARGET}.cmake -DCMAKE_BUILD_TYPE=${BUILD_TYPE} "${BASE_DIR}"
make

if [ "${BUILD_TYPE}" = "Debug" ]; then
	cp "${OUTPUT_FILE}.dll" "${BASE_DIR}/dist/${OUTPUT_FILE}-dbg.dll"
else
	cp "${OUTPUT_FILE}.dll" "${BASE_DIR}/dist/${OUTPUT_FILE}.dll"
fi
