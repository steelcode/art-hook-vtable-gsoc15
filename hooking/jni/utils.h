#ifndef _UTILSH_
#define _UTILSH_

#include <android/log.h>
#include <jni.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <pthread.h>
#include "modifiers.h"
#include "hook.h"
#include "globals.h"
#include "uthash.h"


#undef NELEM
#define NELEM(x)            (sizeof(x)/sizeof(*(x)))



#define MNAME "getId"
#define MSIG "()Ljava/lang/String;"

#define CNAME "test/sid/org/ndksample/HelloPrinter"
#define TARGET_CLS "android/telephony/TelephonyManager"

#define HOOKCLS "test/sid/org/ndksample/HookCls"
#define HOOKM "getDeviceId"
#define HOOKMSIG "()Ljava/lang/String;"

#define MAXSIZE 512

#define MYDEXDIR  "/data/local/tmp/dex/target.dex"
#define MYOPTDIR  "/data/local/tmp/dex/opt"


jint getAPIVersion(JNIEnv *);
void breakMe();
void set_pointer(unsigned int *, unsigned int );
void hookDevId(JNIEnv *, jobject , int );
char * get_dev_id(JNIEnv *, jobject );
jmethodID getMethodID(JNIEnv *,jclass , char *, char* );
void callTestJNI(JNIEnv *, jobject );
jmethodID fromObjToMethodID(JNIEnv *, jobject , char *, char *);
int isLollipop(JNIEnv *);
void findClass_test(JNIEnv*);
jclass _findClass(JNIEnv* , char* );
jobject createInstanceFromClsName(JNIEnv* , char* );
jobject createInstanceFromJClsName(JNIEnv* , jstring );
char* getCharFromJstring(JNIEnv* , jstring );

#endif
