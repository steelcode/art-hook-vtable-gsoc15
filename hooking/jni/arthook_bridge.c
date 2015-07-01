#include "arthook_bridge.h"

jobject _callOriginalMethod(JNIEnv* env, jobject this, jstring jstr, jobject thiz, jstring arg1)
{
    LOGI(" %s thiz: %x, jstr: %x  \n", __PRETTY_FUNCTION__, thiz, jstr);
    jobject gThiz = (*env)->NewGlobalRef(env, thiz);
    jstring gStrArg = (jstring) (*env)->NewGlobalRef(env, arg1);
    return hooks_manager_OriginalCall(env, jstr, gThiz, gStrArg);
}

void _init_from_java(JNIEnv* env, jobject thiz, jobject arg)
{
    LOGI("---------------DENTRO INIT FROM JAVA\n");
    jmethodID testID;
    jclass cls;
    jmethodID originalID;
    arthook_t* tmp;
    
    jobject dexloader = get_dexloader();
    jclass test = findClassFromClassLoader(env,dexloader,"org/sid/arthookbridge/HookCls" );


    LOGI("\n\n");

    //testID = (*env)->GetStaticMethodID(env, test, "initFile", "(Ljava/lang/String;)V");

/* 
    LOGI("creo hook SENDTESTSMS\n");

    testID = (*env)->GetStaticMethodID(env, test, "sendTextMessage", "(Ljava/lang/Object;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Landroid/app/PendingIntent;Landroid/app/PendingIntent;)V");    
    LOGI("2 testid: %x\n", testID);


    callForSms(env);       


    jobject gObj = (*env)->NewGlobalRef(env, arg);
    //LOGI("3 globalobj: %x \n", gObj);
    cls = (*env)->FindClass(env, "android/telephony/SmsManager");
    //cls = (jclass)(*env)->GetObjectClass(env, arg);
    //LOGI("trovata smsclazz: %x\n", cls);
    
    originalID = (*env)->GetMethodID(env, cls, "sendTextMessage", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Landroid/app/PendingIntent;Landroid/app/PendingIntent;)V");
    
    LOGI("trovato PRIMA  ORIGINALID: %x\n", originalID);

    tmp = create_hook_obj(env, cls, gObj,"android/telephony/SmsManager", "sendTextMessage","(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Landroid/app/PendingIntent;Landroid/app/PendingIntent;)V",testID);
    
    //originalID = (*env)->GetMethodID(env, cls,"sendTextMessage", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Landroid/app/PendingIntent;Landroid/app/PendingIntent;)V");
    //LOGI("trovato DOPO ORIGINALID: %x\n", originalID);

    LOGI("ARTHOOK BRIDGE DOPO CREATE HOOK\n");
    add_hook(tmp);

    callForSms(env);    
*/  
    
}


static JNINativeMethod artHookMethods[] = {
    /* name, signature, funcPtr */ 
    { "callOriginalMethod", "(Ljava/lang/String;Ljava/lang/Object;Ljava/lang/String;)Ljava/lang/Object;",
            (void*) _callOriginalMethod },
    { "init_from_java", "(Ljava/lang/Object;)V",
            (void*) _init_from_java },
};


int jniRegisterNativeMethods(JNIEnv* env, jclass cls)
{


    if ((*env)->RegisterNatives(env, cls, artHookMethods, NELEM(artHookMethods)) < 0) {
        return 1;
    }
    return 0;
}

