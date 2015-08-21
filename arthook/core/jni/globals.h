#ifndef _GLOBALSH_
#define _GLOBALSH_

#include <jni.h>
#include "arthook_t.h"

#define bool int
#define true 1
#define false 0

#define TAG "ARTvtablehook"
#define DEBUG 0

#define LOGI(...) \
        {if(DEBUG) {__android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__);}}

#define LOGG(...) \
        {__android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__);}

extern void (*log_function)(char *logmsg);

#undef arthooklog

/*
#define arthooklog(...) \
        {FILE *fp = fopen("/data/local/tmp/arthook.log", "a+"); if (fp) {\
        fprintf(fp, __VA_ARGS__);\
        fclose(fp);}}
*/


#define arthooklog(...) \
        {if(DEBUG) {char __msg[1024] = {0};\
        snprintf(__msg, sizeof(__msg)-1, __VA_ARGS__);\
        log_function(__msg); } }




// shared between process/threads
extern JavaVM* vms;



extern JNIEnv* get_jnienv();


#endif

