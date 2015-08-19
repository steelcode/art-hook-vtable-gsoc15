#include "arthook_bridge.h"

static jobject gDexLoader;

jobject get_dexloader()
{
    return gDexLoader;
}

jclass load_class_from_dex(JNIEnv* env, jobject dexloader, char* clsname)
{
    jclass c = loadClassFromClassLoader(env, dexloader,clsname );
    return c;
}
jobject set_dexloader(JNIEnv* env, char* dexfile, char* optdir)
{
    jobject systemCL = getSystemClassLoader(env);
    jobject dexloader  = createDexClassLoader(env, systemCL, dexfile, optdir);
    gDexLoader = (*env)->NewGlobalRef(env, dexloader);
    jclass c2 = loadClassFromClassLoader(env, dexloader, BRIDGE_UTILS );
    return gDexLoader;
}

jobject _get_original_method(JNIEnv* env, jobject trashme, jstring hook_key)
{
    LOGI("chiamato %s \n ",  __PRETTY_FUNCTION__ ); 
    char *key = getCharFromJstring(env, hook_key);
    LOGI("cerco il metodo : %s \n", key);    
    arthook_t* h = get_hook_by_key(key);
    if(!h) return NULL;
    void* asd = malloc(4);
    LOGI("io valgo: %x \n", (unsigned int) &h->original_meth_ID);
    memcpy(asd, &h->original_meth_ID, 4);
    LOGI("ora valgo %x \n", (unsigned int ) asd);
    return (jobject) (*env)->NewGlobalRef(env, asd);
    
}
jint printStackTraceFromJava(JNIEnv* env)
{
    jclass test = findClassFromClassLoader(env,gDexLoader,BRIDGE_UTILS);
    jmethodID mid = (*env)->GetStaticMethodID(env, test, "printStackTraces", "()I");
    jint res = (*env)->CallStaticIntMethod(env, test, mid);
    return res;
}
jboolean callGetBoolean(JNIEnv* env, jobject javaArgs, int index)
{
    jclass test = findClassFromClassLoader(env,gDexLoader,BRIDGE_UTILS);
    jmethodID mid = (*env)->GetStaticMethodID(env, test, "getBoolean", "([Ljava/lang/Object;I)Z");
    jboolean res = (*env)->CallStaticIntMethod(env, test, mid, javaArgs, index);
    return res;

}
jdouble callGetDouble(JNIEnv* env, jobject javaArgs, int index)
{
    jclass test = findClassFromClassLoader(env,gDexLoader,BRIDGE_UTILS);
    jmethodID mid = (*env)->GetStaticMethodID(env, test, "getDouble", "([Ljava/lang/Object;I)D");
    jdouble res = (*env)->CallStaticDoubleMethod(env, test, mid, javaArgs, index);
    return res;

}
jfloat callGetFloat(JNIEnv* env, jobject javaArgs, int index)
{
    jclass test = findClassFromClassLoader(env,gDexLoader,BRIDGE_UTILS);
    jmethodID mid = (*env)->GetStaticMethodID(env, test, "getFloat", "([Ljava/lang/Object;I)F");
    jfloat res = (*env)->CallStaticFloatMethod(env, test, mid, javaArgs, index);
    return res;

}
jlong callGetLong(JNIEnv* env, jobject javaArgs, int index)
{
    jclass test = findClassFromClassLoader(env,gDexLoader,BRIDGE_UTILS);
    jmethodID mid = (*env)->GetStaticMethodID(env, test, "getLong", "([Ljava/lang/Object;I)J");
    jlong res = (*env)->CallStaticFloatMethod(env, test, mid, javaArgs, index);
    return res;

}
jint callGetInt(JNIEnv* env, jobject javaArgs, int index)
{
    jclass test = findClassFromClassLoader(env,gDexLoader,BRIDGE_UTILS);
    jmethodID mid = (*env)->GetStaticMethodID(env, test, "getInt", "([Ljava/lang/Object;I)I");
    jint res = (*env)->CallStaticIntMethod(env, test, mid, javaArgs, index);
    return res;

}
jobject callGetObj(JNIEnv* env, jobject javaArgs, int index)
{
    jclass test = findClassFromClassLoader(env,gDexLoader,BRIDGE_UTILS);
    jmethodID mid = (*env)->GetStaticMethodID(env, test, "getObj", "([Ljava/lang/Object;I)Ljava/lang/Object;");
    jobject res = (*env)->CallStaticObjectMethod(env, test, mid, javaArgs, index);
    return (*env)->NewGlobalRef(env,res);

}

//DEPRECATED
jobject hook_bridge_OriginalCall
  (JNIEnv *env, jstring hook_key, jobject o, jstring arg1)
{
    char *key = getCharFromJstring(env, hook_key);
    LOGI("dentro hooks manager, cerco : %s \n", key);
    arthook_t* myhook = (arthook_t*) get_hook_by_key(key);
    if(myhook == NULL)
        return NULL;
    return  call_original_method(env, myhook, o, arg1);    
}

static JNINativeMethod artHookMethods[] = {
    /* name, signature, funcPtr */ 
    //UNUSED
    //{"get_original_method", "(Ljava/lang/String;)Ljava/lang/Object;", (void*) _get_original_method },
};


int jniRegisterNativeMethods(JNIEnv* env, jclass cls)
{

    if ((*env)->RegisterNatives(env, cls, artHookMethods, NELEM(artHookMethods)) < 0) {
        return 1;
    }
    return 0;
}

