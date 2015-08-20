#ifndef ARTHOOK_HELPER_H
#define ARTHOOK_HELPER_H

#include "arthook_t.h"
#include "utils.h"
#include "globals.h"
#include "arthook_manager.h"
#include "arthook_bridge.h"

#define CLAZZ_OFF 0x8
#define ITABLE_OFF 0x18 //24
#define VMETHODS_OFF 0x30
#define VMETHS_LEN_OFF 0x8

#define VTABLE_OFF 0x34
#define ACCESS_FLAG_OFF 28
#define CSIZE_OFF 0x3c


//LOLLIPOP
#define LOLLIPOP_VTABLE_OFF 0x2c // 44
#define LOLLIPOP_VMETHODS_OFF 0x2c // 44
#define LOLLIPOP_CLAZZ_OFF 0x8 // 16



arthook_t* create_hook(JNIEnv *, char *, const char* , const char*, jclass,jmethodID);
static unsigned int* searchInMemoryVtable(unsigned int , int , int, bool );
static unsigned int* searchInMemoryVmeths(unsigned int , int , int );
void* hh_check_javareflection_call(JNIEnv *, jobject, jobject );
void* callOriginalReflectedMethod(JNIEnv* env, jobject javaReceiver, arthook_t* tmp, jobject);
jobject call_patch_method(JNIEnv* env, arthook_t* h, jobject thiz, jobject);


#endif