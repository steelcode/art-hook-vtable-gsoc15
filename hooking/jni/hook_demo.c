#include "hook_demo.h"

static WrapMethodToHook methodsToHook[] = {

    {"android/telephony/TelephonyManager","getDeviceId","()Ljava/lang/String;",
        "org/sid/arthookbridge/HookCls", "getDeviceId", "(Ljava/lang/Object;)Ljava/lang/String;", NULL},  
    
    {"android/app/ContextImpl","openFileOutput","(Ljava/lang/String;I)Ljava/io/FileOutputStream;", "org/sid/arthookbridge/HookCls", "openFileOutput", "(Ljava/lang/Object;Ljava/lang/String;I)Ljava/io/FileOutputStream;", NULL},
        {"android/app/Activity","openFileOutput","(Ljava/lang/String;I)Ljava/io/FileOutputStream;", "org/sid/arthookbridge/HookCls", "openFileOutput", "(Ljava/lang/Object;Ljava/lang/String;I)Ljava/io/FileOutputStream;", NULL},


    /*
       {"android/telephony/TelephonyManager","getDeviceSoftwareVersion","()Ljava/lang/String;",
       "org/sid/arthookbridge/HookCls", NULL},
    {"android/telephony/TelephonyManager","getNetworkOperator","()Ljava/lang/String;",
        "org/sid/arthookbridge/HookCls", "getNetworkOperator", "(Ljava/lang/Object;)Ljava/lang/String;", NULL},

       {"android/telephony/SmsManager", "sendTextMessage", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Landroid/app/PendingIntent;Landroid/app/PendingIntent;)V", "org/sid/arthookbridge/HookCls", NULL},

       */
};



int hook_demo_init(JNIEnv* env)
{
    set_dexloader(env);
    jobject dexloader = get_dexloader();
    jclass test = findClassFromClassLoader(env,dexloader,"org/sid/arthookbridge/HookCls" );
    if(jniRegisterNativeMethods(env, test) == 1 ){
        LOGI("JNI REGISTER NATIVE METHODS ERROR!!! \n");
    }

    int i = 0;
    int nelem = NELEM(methodsToHook);

    for(i=0; i < nelem ; i++){
        //jclass c = loadClassFromClassLoader(env, dexloader,methodsToHook[i].hookclsname );   
        test = findClassFromClassLoader(env,dexloader, methodsToHook[i].hookclsname);
        jclass gtest = (*env)->NewGlobalRef(env, test);
        jmethodID testID = (*env)->GetStaticMethodID(env,gtest,methodsToHook[i].hookmname, methodsToHook[i].hookmsig);
        arthook_t* tmp = create_hook(env,methodsToHook[i].cname, methodsToHook[i].mname, methodsToHook[i].msig, gtest,testID);
        add_hook(tmp);
    }    
    print_hashtable();
    LOGG("[ %s ]  init phase terminated, happy hooking !! \n", __PRETTY_FUNCTION__);
}
