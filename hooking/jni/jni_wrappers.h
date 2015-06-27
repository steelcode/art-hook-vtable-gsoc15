#include <jni.h>
#include "utils.h"

JNIEnv* getEnv(JavaVM*);

jobject createDexClassLoader(JNIEnv* , jobject );

jobject getSystemClassLoader(JNIEnv*);

jclass findClassFromClassLoader(JNIEnv* , jobject );

jclass loadClassFromClassLoader(JNIEnv* , jobject );
