#ifndef _ARTHOOK_TH_
#define _ARTHOOK_TH_
#include <jni.h>
#include "uthash.h"

#define MAXSIZE 512

typedef struct arthook_t{
    char clsname[MAXSIZE];
    char mname[MAXSIZE];
    char msig[MAXSIZE];
    char key[MAXSIZE*3];

    jmethodID original_meth_ID;
    jclass hook_cls;
    jmethodID hook_meth_ID;
    jobject original_obj;
    jclass original_cls;
    int type;

    UT_hash_handle hh;
}arthook_t;

#endif
