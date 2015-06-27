#!/bin/sh

cd hijack/jni
ndk-build
cd ../..


cd hooking/jni
ndk-build
cd ../..



