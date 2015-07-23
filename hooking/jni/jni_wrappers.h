#ifndef _JNIWRAPPERSH_
#define _JNIWRAPPERSH_

#include "utils.h"
#include "globals.h" 
#include "artstuff.h"

extern JNIEnv* getEnv();

extern jobject createDexClassLoader(JNIEnv* , jobject, char*, char* );

extern jobject getSystemClassLoader(JNIEnv*);

extern jclass findClassFromClassLoader(JNIEnv* , jobject , char*);

extern jclass loadClassFromClassLoader(JNIEnv* , jobject , char*);

#endif
