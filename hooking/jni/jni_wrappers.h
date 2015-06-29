#include <jni.h>
#include "utils.h"

JNIEnv* getEnv(JavaVM*);

jobject createDexClassLoader(JNIEnv* , jobject, char*, char* );

jobject getSystemClassLoader(JNIEnv*);

jclass findClassFromClassLoader(JNIEnv* , jobject , char*);

jclass loadClassFromClassLoader(JNIEnv* , jobject , char*);
