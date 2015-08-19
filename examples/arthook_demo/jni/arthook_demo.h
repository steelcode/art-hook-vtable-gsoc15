#ifndef HOOK_DEMO_H
#define HOOK_DEMO_H

#include "arthook_t.h"
#include "arthook_bridge.h"
#include "arthook_manager.h"
#include "artstuff.h"

#define MYDEX  "/data/local/tmp/dex/target.dex"
#define MYOPTDIR  "/data/local/tmp/dex/opt"

#define MYHOOKCLASS "org/sid/arthookbridge/HookCls"

typedef struct{
    char* cname;
    char* mname;
    char* msig;
    char* hookclsname;
    char* hookmname;
    char* hookmsig;
    jmethodID target;
} WrapMethodsToHook;


int hookdemo_init(JNIEnv*);

#endif