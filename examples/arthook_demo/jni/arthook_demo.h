#ifndef HOOK_DEMO_H
#define HOOK_DEMO_H

#include "arthook_t.h"
#include "arthook_bridge.h"
#include "arthook_manager.h"
#include "artstuff.h"
typedef struct{
    char* cname;
    char* mname;
    char* msig;
    char* hookclsname;
    char* hookmname;
    char* hookmsig;
    jmethodID target;
} WrapMethodToHook;


int hook_demo_init(JNIEnv*);

#endif