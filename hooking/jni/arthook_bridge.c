#include "arthook_bridge.h"


jobject _callOriginalMethod(JNIEnv* env, jobject this, jstring jstr, jobject thiz, jstring arg1)
{
    LOGI(" %s thiz: %x, jstr: %x  \n", __PRETTY_FUNCTION__, thiz, jstr);
    jobject gThiz = (*env)->NewGlobalRef(env, thiz);
    jstring gStrArg = (jstring) (*env)->NewGlobalRef(env, arg1);
    return hook_bridge_OriginalCall(env, jstr, gThiz, gStrArg);
}
jobject _get_original_method(JNIEnv* env, jobject trashme, jstring hook_key)
{
    LOGI("chiamato %s \n ",  __PRETTY_FUNCTION__ ); 
    char *key = getCharFromJstring(env, hook_key);
    LOGI("cerco il metodo : %s \n", key);    
    arthook_t* h = get_hook_by_key(key);
    if(!h) return NULL;
    void* asd = malloc(4);
    LOGI("io valgo: %x \n", &h->original_meth_ID);
    memcpy(asd, &h->original_meth_ID, 4);
    LOGI("ora valgo %x \n", asd);
    return (jobject) (*env)->NewGlobalRef(env, asd);
    
}

//DEPRECATED
jobject hook_bridge_OriginalCall
  (JNIEnv *env, jstring hook_key, jobject o, jstring arg1)
{
    char *key = getCharFromJstring(env, hook_key);
    LOGI("dentro hooks manager, cerco : %s \n", key);
    arthook_t* myhook = get_hook_by_key(key);
    if(myhook == NULL)
        return NULL;
    return  call_original_method(env, myhook, o, arg1);    
}

static JNINativeMethod artHookMethods[] = {
    /* name, signature, funcPtr */ 
    //DEPRECATED
    { "callOriginalMethod", "(Ljava/lang/String;Ljava/lang/Object;Ljava/lang/String;)Ljava/lang/Object;",
            (void*) _callOriginalMethod },
    //UNUSED
    {"get_original_method", "(Ljava/lang/String;)Ljava/lang/Object;", (void*) _get_original_method },
};


int jniRegisterNativeMethods(JNIEnv* env, jclass cls)
{

    if ((*env)->RegisterNatives(env, cls, artHookMethods, NELEM(artHookMethods)) < 0) {
        return 1;
    }
    return 0;
}

