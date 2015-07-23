#include "arthook_t.h"
#include "arthook_helper.h"
#include "utils.h"

int jniRegisterNativeMethods(JNIEnv* env, jclass cls);

jobject hook_bridge_OriginalCall(JNIEnv*, jstring, jobject, jstring);

