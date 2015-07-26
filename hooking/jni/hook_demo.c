#include "hook_demo.h"

static WrapMethodToHook methodsToHook[] = {

    {"android/telephony/TelephonyManager","getDeviceId","()Ljava/lang/String;",
        "org/sid/arthookbridge/HookCls", "getDeviceId", "(Ljava/lang/Object;)Ljava/lang/String;", NULL},   
    {"android/app/ContextImpl","openFileOutput","(Ljava/lang/String;I)Ljava/io/FileOutputStream;", "org/sid/arthookbridge/HookCls", "openFileOutput", "(Ljava/lang/Object;Ljava/lang/String;I)Ljava/io/FileOutputStream;", NULL},
    


    /*
       {"android/telephony/TelephonyManager","getDeviceSoftwareVersion","()Ljava/lang/String;",
       "org/sid/arthookbridge/HookCls", NULL},
    {"android/telephony/TelephonyManager","getNetworkOperator","()Ljava/lang/String;",
        "org/sid/arthookbridge/HookCls", "getNetworkOperator", "(Ljava/lang/Object;)Ljava/lang/String;", NULL},

       {"android/telephony/SmsManager", "sendTextMessage", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Landroid/app/PendingIntent;Landroid/app/PendingIntent;)V", "org/sid/arthookbridge/HookCls", NULL},

       */
};


static jobject gDexLoader;

jobject get_dexloader()
{
    return gDexLoader;
}
jint printStackTraceFromJava(JNIEnv* env)
{
    jclass test = findClassFromClassLoader(env,gDexLoader,"org/sid/arthookbridge/Utils" );
    jmethodID mid = (*env)->GetStaticMethodID(env, test, "printStackTraces", "()I");
    jint res = (*env)->CallStaticIntMethod(env, test, mid);
    return res;
}

void set_dexloader(JNIEnv* env)
{
    jobject systemCL = getSystemClassLoader(env);   
    jobject dexloader  = createDexClassLoader(env, systemCL,  MYDEXDIR,  MYOPTDIR); 
    gDexLoader = (*env)->NewGlobalRef(env, dexloader);  
    jclass c = loadClassFromClassLoader(env, dexloader,"org/sid/arthookbridge/HookCls" ); 
    jclass c2 = loadClassFromClassLoader(env, dexloader,"org/sid/arthookbridge/Utils" );         
}
void aatest(JNIEnv* env)
{
       jclass cls = (*env)->FindClass(env, "android/content/Context");

       jmethodID mid = (*env)->GetMethodID(env, cls, "getSystemService",
       "(Ljava/lang/String;)Ljava/lang/Object;");
       jfieldID fid = (*env)->GetStaticFieldID(env, cls, "TELEPHONY_SERVICE",
       "Ljava/lang/String;");
       jstring str = (*env)->GetStaticObjectField(env, cls, fid);
       jobject telephony = (*env)->CallObjectMethod(env, cls, mid, str);

       cls = (*env)->FindClass(env, "android/telephony/TelephonyManager");
       mid =(*env)->GetMethodID(env, cls, "getDeviceId", "()Ljava/lang/String;");

       LOGI("my getdeviceid :  %x \n", mid);

}

int hook_demo_init(JNIEnv* env)
{
    set_dexloader(env);
    jclass test = findClassFromClassLoader(env,gDexLoader,"org/sid/arthookbridge/HookCls" );
    if(jniRegisterNativeMethods(env, test) == 1 ){
        LOGI("JNI REGISTER NATIVE METHODS ERROR!!! \n");
    }

    int i = 0;
    int nelem = NELEM(methodsToHook);
    jobject dexloader  = get_dexloader();
    //for(i=0;i<10;i++){
    //        aatest(env);
    //}
    for(i=0; i < nelem ; i++){
        //jclass c = loadClassFromClassLoader(env, dexloader,methodsToHook[i].hookclsname );   
        test = findClassFromClassLoader(env,dexloader, methodsToHook[i].hookclsname);
        jclass gtest = (*env)->NewGlobalRef(env, test);
        jmethodID testID = (*env)->GetStaticMethodID(env,gtest,methodsToHook[i].hookmname, methodsToHook[i].hookmsig);
        arthook_t* tmp = create_hook(env,methodsToHook[i].cname, methodsToHook[i].mname, methodsToHook[i].msig, gtest,testID);
        add_hook(tmp);
    }    

    LOGG("[ %s ]  init phase terminated, happy hooking !! \n", __PRETTY_FUNCTION__);
}
