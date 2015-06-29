#include "arthook_t.h"
#include "arthook_helper.h"
#include "jni_wrappers.h"
#include "utils.h"


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
jobject get_dexloader();
