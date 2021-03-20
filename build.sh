#!/bin/bash -e

BASE_DIR="$(dirname "$(realpath "${0}")")"

if [ "$#" -lt 2 ]; then
	echo "error: Need at least a build target and build type"
	exit 1
fi

# Ensure the output folder exists
mkdir -p "${BASE_DIR}/dist"

echo "Got args: $@"

BUILD_TARGET="${1}"
BUILD_TYPE="${2}"

cmake_args=()

# Determine the correct file names and paths
OUTPUT_FILE="gmsv_chttp_${BUILD_TARGET}"
DIST_FILE="${OUTPUT_FILE}"
BUILD_DIR="chttp-${BUILD_TARGET}-${BUILD_TYPE}"

if [ "${BUILD_TYPE}" = "Debug" ]; then
	DIST_FILE="${DIST_FILE}-dbg"
fi

for i in "${@:3}"; do
	case "$i" in
	"clean")
		build_clean=1
		;;
	"verbose")
		build_verbose=1
		;;
	*)
		echo "Unknown argument: '$i'" >&2
		exit 1
	esac
done

if [ "$build_verbose" = "1" ]; then
	cmake_args+=("-DCMAKE_VERBOSE_MAKEFILE:BOOL=ON")
fi

if [ "$build_clean" = "1" ]; then
	rm -rf "${BUILD_DIR}"
fi

mkdir -p "${BUILD_DIR}"
cd "${BUILD_DIR}"
cmake \
    -DCMAKE_TOOLCHAIN_FILE=toolchain-${BUILD_TARGET}.cmake \
    -DCMAKE_BUILD_TYPE=${BUILD_TYPE} \
    "${cmake_args[@]}" \
    "${BASE_DIR}"
make

cp "${OUTPUT_FILE}.dll" "${BASE_DIR}/dist/${DIST_FILE}.dll"
