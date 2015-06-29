#include <sys/types.h>
#include <pthread.h>

#include "arthook_t.h"
#include "utils.h"
#include "hook_demo.h"

int arthook_manager_init(JNIEnv*);

int add_hook(arthook_t*);

jobject hooks_manager_OriginalCall(JNIEnv*, jstring, jobject, jstring);
