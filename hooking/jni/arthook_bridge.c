#include "arthook_bridge.h"

jstring cippa(JNIEnv* env, jobject thiz, jstring jstr)
{
    LOGI("STA CIPPPPPAAAAA thiz: %x, jstr: %x  \n", thiz, jstr);
    //return (*env)->NewStringUTF(env, "arthookcore native w00t w00t!");
    return hooks_manager_OriginalCall(env, jstr);
}




static JNINativeMethod artHookMethods[] = {
    /* name, signature, funcPtr */ 
    { "callOriginalMethod", "(Ljava/lang/String;)Ljava/lang/String;",
            (void*) cippa },

};


int jniRegisterNativeMethods(JNIEnv* env, jclass cls)
{

    //jclass cls = findClassFromClassLoader(env, classLoader );
    //jclass cls = (*env)->FindClass(env, clsname);
    LOGI("SALVOOOOOOOOOOOOOOO\n");
    if ((*env)->RegisterNatives(env, cls, artHookMethods, NELEM(artHookMethods)) < 0) {
        return 1;
    }
    return 0;
}

