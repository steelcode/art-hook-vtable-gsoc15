#include "hook_demo.h"

static WrapMethodToHook methodsToHook[] = {

    {"android/app/ContextImpl","openFileOutput","(Ljava/lang/String;I)Ljava/io/FileOutputStream;", "org/sid/arthookbridge/HookCls", "openFileOutput", "(Ljava/lang/Object;Ljava/lang/String;I)Ljava/io/FileOutputStream;", NULL},

    {"android/telephony/TelephonyManager","getDeviceId","()Ljava/lang/String;",
        "org/sid/arthookbridge/HookCls", "getDeviceId", "(Ljava/lang/Object;)Ljava/lang/String;", NULL},   

    {"android/telephony/TelephonyManager","getNetworkOperator","()Ljava/lang/String;",
        "org/sid/arthookbridge/HookCls", "getNetworkOperator", "(Ljava/lang/Object;)Ljava/lang/String;", NULL},

    //the following two not work
    {"java/net/URL", "openConnection", "()Ljava/net/URLConnection;",
        "org/sid/arthookbridge/HookCls","openConnection","(Ljava/lang/Object;)Ljava/net/URLConnection;",NULL},

    {"java/net/URL", "<init>", "(Ljava/lang/String;)V",
        "org/sid/arthookbridge/HookCls", "myURLinit","(Ljava/lang/Object;Ljava/lang/String;)V",NULL},

    /*
       {"android/telephony/TelephonyManager","getDeviceSoftwareVersion","()Ljava/lang/String;",
       "org/sid/arthookbridge/HookCls", NULL},

       {"android/telephony/SmsManager", "sendTextMessage", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Landroid/app/PendingIntent;Landroid/app/PendingIntent;)V", "org/sid/arthookbridge/HookCls", NULL},
       */
};


static jobject gDexLoader;

jobject get_dexloader()
{
    return gDexLoader;
}

void set_dexloader(JNIEnv* env)
{
    jobject systemCL = getSystemClassLoader(env);   
    jobject dexloader  = createDexClassLoader(env, systemCL,  MYDEXDIR,  MYOPTDIR); 
    gDexLoader = (*env)->NewGlobalRef(env, dexloader);  
    jclass c = loadClassFromClassLoader(env, dexloader,"org/sid/arthookbridge/HookCls" );     
}

int hook_demo_init(JNIEnv* env)
{
    LOGI("CHIAMO INIT DENTRO JAVA\n");
    set_dexloader(env);
    jclass test = findClassFromClassLoader(env,gDexLoader,"org/sid/arthookbridge/HookCls" );
    if(jniRegisterNativeMethods(env, test) == 1 ){
        LOGI("ERRORE REGISTRAZIONE METODI NATIVI!!!\n");
    }

    //jmethodID initmid = (*env)->GetStaticMethodID(env, test, "callInit", "()V");
    //(*env)->CallStaticVoidMethod(env, test, initmid , NULL);
    //LOGI("CHIAMATO INIT JAVA \n");


    int i = 0;
    int nelem = NELEM(methodsToHook);
    jobject dexloader  = get_dexloader();

    for(i=0; i < nelem ; i++){
        LOGI("sto per hookare la classe: %s \n", methodsToHook[i].cname);
        LOGI("il metodo: %s \n", methodsToHook[i].mname);
        LOGI("con HOOK CLS: %s \n", methodsToHook[i].hookclsname);

        jclass c = loadClassFromClassLoader(env, dexloader,methodsToHook[i].hookclsname );   
        jclass test = findClassFromClassLoader(env,dexloader, methodsToHook[i].hookclsname);

        jmethodID testID = (*env)->GetStaticMethodID(env, test,methodsToHook[i].hookmname, methodsToHook[i].hookmsig);
        LOGI("trovato il metodo di hook nella mia class: %x \n", (unsigned int) testID);

        arthook_t* tmp = create_hook(env,methodsToHook[i].cname, methodsToHook[i].mname, methodsToHook[i].msig, testID);
        add_hook(tmp);
    }    
}
