#!/bin/sh

cd arthook/core/jni
ndk-build
cd ../../../


cd examples/arthook_demo/jni
ndk-build
cd ../../../



