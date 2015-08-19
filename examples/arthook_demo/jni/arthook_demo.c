#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <sys/epoll.h>
#include <pthread.h>
#include <jni.h>
#include <stdlib.h>

// adbi include
#include "base.h"

#include "arthook_demo.h"

static WrapMethodsToHook methodsToHook[] = {

    {"android/telephony/TelephonyManager","getDeviceId","()Ljava/lang/String;",
        MYHOOKCLASS, "getDeviceId", "(Ljava/lang/Object;)Ljava/lang/String;", NULL},
    
    {"android/app/ContextImpl","openFileOutput","(Ljava/lang/String;I)Ljava/io/FileOutputStream;",
            MYHOOKCLASS, "openFileOutput", "(Ljava/lang/Object;Ljava/lang/String;I)Ljava/io/FileOutputStream;", NULL},

    {"android/app/Activity","openFileOutput","(Ljava/lang/String;I)Ljava/io/FileOutputStream;",
        MYHOOKCLASS, "openFileOutput", "(Ljava/lang/Object;Ljava/lang/String;I)Ljava/io/FileOutputStream;", NULL},


    /*
       {"android/telephony/TelephonyManager","getDeviceSoftwareVersion","()Ljava/lang/String;",
       MYHOOKCLASS, NULL},
    {"android/telephony/TelephonyManager","getNetworkOperator","()Ljava/lang/String;",
        MYHOOKCLASS, "getNetworkOperator", "(Ljava/lang/Object;)Ljava/lang/String;", NULL},

       {"android/telephony/SmsManager", "sendTextMessage", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Landroid/app/PendingIntent;Landroid/app/PendingIntent;)V", MYHOOKCLASS, NULL},

       */
};



int hookdemo_init(JNIEnv* env)
{
    arthook_manager_init(env);
    jobject dexloader = set_dexloader(env, MYDEX, MYOPTDIR);

    jclass test = load_class_from_dex(env, dexloader, MYHOOKCLASS);
    //jclass test = findClassFromClassLoader(env,dexloader,MYHOOKCLASS );
    //if(jniRegisterNativeMethods(env, test) == 1 ){
    //    LOGI("JNI REGISTER NATIVE METHODS ERROR!!! \n");
    //}

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
    LOGG("[ %s ]  init terminated, happy hooking !! \n", __PRETTY_FUNCTION__);
}


static struct hook_t eph;
static struct hook_t invokeh;
static int done = 0;
char artlogfile[]  = "/data/local/tmp/arthook.log";

JavaVM* vms = NULL;
pthread_mutex_t lock;

// arm version of hook
extern int my_epoll_wait_arm(int epfd, struct epoll_event *events, int maxevents, int timeout);
/*
 *  log function to pass to the hooking library to implement central loggin
 *
 *  see: set_logfunction() in base.h
 */
static void my_log(char *msg)
{
    log("%s", msg);
}
void artlogmsgtofile(char* msg){
    int fp = open(artlogfile, O_WRONLY|O_APPEND);
    if (fp != -1) {
        write(fp, msg, strlen(msg));
        close(fp);
    }
}

void* set_arthooklogfunction(void* func){
    void* old = log_function;
    log_function = func;
    return old;
}


void init_hook()
{
    JNIEnv* env = get_jnienv();
    //arthook_manager_init(env);
    hookdemo_init(env);
}

void* my_invoke_method(void* soa, jobject javaMethod, void* javaReceiver, jobject javaArgs){
    log("!!! dentro my invoke method, receiver vale: 0x%08x, javamethod : 0x%08x \n", (unsigned int) javaReceiver,  (unsigned int) javaMethod);

    void* checkcalledmethod = (void*) 1;
    void* res = NULL;
    JNIEnv* th_env = get_jnienv();
    log("env vvale: %p \n", th_env);
    jint checkstack = printStackTraceFromJava(th_env);

    // check if an hooked method is the target of the reflection call
    checkcalledmethod = hh_check_javareflection_call(th_env, javaMethod, javaReceiver);

    // checks:
    // 1. the method called with reflection is hooked?
    // 2. the call is from our patch method?
    //
    void* (*orig_invoke_method)(void* soa, void* javaMethod, void* javaReceiver, void* javaArgs);
    orig_invoke_method = (void*) invokeh.orig;

    // called method is not an hooked method
    if(!checkcalledmethod){
        log("chiamo il metodo originale richiesto con la reflection \n");
        hook_precall(&invokeh);

        res = orig_invoke_method(soa,javaMethod,javaReceiver,javaArgs);

        hook_postcall(&invokeh);
    }
    else{
        if(checkstack){
            log("!!! NON chiamo il metodo usando la reflection! \n");
            // trapped call is from a "patch method"
            // so we have to direct call the original method
            return callOriginalReflectedMethod(th_env, javaReceiver, (arthook_t*) checkcalledmethod, javaArgs);
        }
        else{
            // we have to call the "patch method"
            //
            log("!!! CHIAMO IL PATCH METHOD !!!  \n");
            return call_patch_method(th_env, (arthook_t*) checkcalledmethod, javaReceiver, javaArgs);
        }

    }
    return res;

}

int my_epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout)
{
    if (pthread_mutex_lock(&lock) != 0) return 1;
    log("inside %s \n", __PRETTY_FUNCTION__);
    int (*orig_epoll_wait)(int epfd, struct epoll_event *events, int maxevents, int timeout);
    orig_epoll_wait = (void*)eph.orig;

    hook_precall(&eph);
    int res = orig_epoll_wait(epfd, events, maxevents, timeout);
    if(!done) {
        init_hook();
        done = 1;
    }
    pthread_mutex_unlock(&lock);
    return res;
}

void __attribute__ ((constructor)) my_init(void);

void my_init(void)
{

    // adbi and arthook log functions
    set_logfunction(my_log);
    set_arthooklogfunction(artlogmsgtofile);
    log("ARTDroid %s started\n", __FILE__);

    // resolve libart.so symbols used by artstuff.c
    resolve_symbols(&d);
    if (pthread_mutex_init(&lock,NULL) != 0) return;
    // hook native functions
    hook(&eph, getpid(), "libc.", "epoll_wait", my_epoll_wait_arm, my_epoll_wait);
    hook(&invokeh, getpid(), "libart.", "_ZN3art12InvokeMethodERKNS_18ScopedObjectAccessEP8_jobjectS4_S4_", my_epoll_wait_arm, my_invoke_method);

    log("fine %s \n", __PRETTY_FUNCTION__);
}

