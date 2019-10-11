#!/bin/bash

echo "Building Module."
echo "Dependencies: GIT, PREMAKE5, MAKE."
echo

# Check if all dependencies are present
type git >/dev/null      2>&1 || { echo >&2 "git is not installed.";      exit 1; }
type premake5 >/dev/null 2>&1 || { echo >&2 "premake5 is not installed."; exit 1; }
type make >/dev/null     2>&1 || { echo >&2 "make is not installed.";     exit 1; }

# Hacky way to ensure we've got a git repository & submodules at our disposal
git init
git submodule update --init --recursive
echo

if premake5 gmake; then
	cd project

	if make; then
		# We're done!
		cd ..
		sleep 5
	else
		echo
		echo "Something went wrong when building the project."
		exit 1
	fi
else
	echo
	echo "Something went wrong with premake."
	exit 1
fi
