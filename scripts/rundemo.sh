#!/bin/bash

function test {
    "$@"
    local status=$?
    if [ $status -ne 0 ]; then
        echo "error with $1" >&2
	exit $status
    fi
}

echo "starting in: `pwd` "

test "./install.sh"

echo "now in `pwd` "

test ./push_patch_code.sh -t ../examples/classes.dex

test "./startdemo.sh"



