#ifndef ARTHOOK_BRIDGE_H
#define ARTHOOK_BRIDGE_H

#include "arthook_t.h"
#include "arthook_helper.h"
#include "arthook_manager.h"
#include "utils.h"

#define BRIDGE_UTILS "org/sid/arthookbridge/Utils"

int jniRegisterNativeMethods(JNIEnv* env, jclass cls);

jclass load_class_from_dex(JNIEnv* env, jobject, char* clsname);
jobject set_dexloader(JNIEnv* env, char*, char*);
jobject get_dexloader();
jint printStackTraceFromJava(JNIEnv* env);
jint callGetInt(JNIEnv* env, jobject javaArgs, int index);
jobject callGetObj(JNIEnv* env, jobject javaArgs, int index);
jdouble callGetDouble(JNIEnv* env, jobject javaArgs, int index);
jfloat callGetFloat(JNIEnv* env, jobject javaArgs, int index);
jboolean callGetBoolean(JNIEnv* env, jobject javaArgs, int index);
jlong callGetLong(JNIEnv* env, jobject javaArgs, int index);


#endif