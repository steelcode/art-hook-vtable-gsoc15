#ifndef ARTHOOK_BRIDGE_H
#define ARTHOOK_BRIDGE_H

#include "arthook_t.h"
#include "arthook_helper.h"
#include "arthook_manager.h"
#include "utils.h"

int jniRegisterNativeMethods(JNIEnv* env, jclass cls);

jobject hook_bridge_OriginalCall(JNIEnv*, jstring, jobject, jstring);

void set_dexloader(JNIEnv* env);
jobject get_dexloader();
jint printStackTraceFromJava(JNIEnv* env);
jint callGetInt(JNIEnv* env, jobject javaArgs, int index);
jobject callGetObj(JNIEnv* env, jobject javaArgs, int index);
jdouble callGetDouble(JNIEnv* env, jobject javaArgs, int index);
jfloat callGetFloat(JNIEnv* env, jobject javaArgs, int index);
jboolean callGetBoolean(JNIEnv* env, jobject javaArgs, int index);
jlong callGetLong(JNIEnv* env, jobject javaArgs, int index);
/*

jobject _callOriginalMethod(JNIEnv* env, jobject this, jstring jstr, jobject thiz, jstring arg1)
{
    LOGI(" %s thiz: %x, jstr: %x  \n", __PRETTY_FUNCTION__, thiz, jstr);
    jobject gThiz = (*env)->NewGlobalRef(env, thiz);
    jstring gStrArg = (jstring) (*env)->NewGlobalRef(env, arg1);
    return hook_bridge_OriginalCall(env, jstr, gThiz, gStrArg);
}
 */

#endif