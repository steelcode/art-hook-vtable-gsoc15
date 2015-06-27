#define _GNU_SOURCE
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dlfcn.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <string.h>
#include <termios.h>
#include <pthread.h>
#include <sys/epoll.h>

#include <jni.h>
#include <stdlib.h>

// adbi include
#include "base.h"
#include "hook.h"

#include "artstuff.h"
#include "arthook_manager.h"
#include "jni_wrappers.h"
#include "arthook_bridge.h"
#include "arthook_helper.h"
#include "utils.h"



// this file is going to be compiled into a thumb mode binary

void __attribute__ ((constructor)) my_init(void);

static struct hook_t eph;
static struct artstuff_t d;


// for demo code only
static int counter;

// arm version of hook
extern int my_epoll_wait_arm(int epfd, struct epoll_event *events, int maxevents, int timeout);
/*  
 *  log function to pass to the hooking library to implement central loggin
 *
 *  see: set_logfunction() in base.h
 */
static void my_log(char *msg)
{
	log("%s", msg)
}

/*
JNIEXPORT jstring JNICALL Java_test_sid_org_ndksample_HookCls_callOriginalMethod
  (JNIEnv * env, jstring s)
{
   return hooks_manager_OriginalCall(env, s);
}
*/
jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    LOGI(" WOOWOWOOWOWOWO ARTHOOKCORE dentro JNI ONLOAD!! \n ");
    return JNI_VERSION_1_6;
}
void init_hook()
{
    LOGI("INIT HOOK INIZIO\n");
  	JavaVM* vms;
	jsize vm_count;
	jsize size;
	_GetCreatedJavaVMs(&d, (void**) &vms, size, &vm_count);
	log("chiamato getcreatedjavavms, valore: %p , count = %d \n", vms[0], vm_count);
	JNIEnv* env;
    env = getEnv(vms);
	log("jnienv = 0x%08x \n", (unsigned int) env);
    //(*env)->FindClass(env, "aasd");

    jobject systemCL = getSystemClassLoader(env);   
    jobject dexloader  = createDexClassLoader(env, systemCL);   
    jclass c = loadClassFromClassLoader(env, dexloader);   
    jclass test = findClassFromClassLoader(env,dexloader);
    
    //jclass test = (*env)->FindClass(env, "org/sid/arthookbridge/HookCls");
    LOGI("trovato test : %x \n " , (unsigned int) test);
    if(jniRegisterNativeMethods(env, test) == 1 ){
        LOGI("ERRORE REGISTRAZIONE METODI NATIVI!!!\n");
    }

    jmethodID testID = (*env)->GetStaticMethodID(env, test, "getDeviceId", "()Ljava/lang/String;");
    log("trovato il metodo test nella mia class: %x \n", (unsigned int) testID);
    arthook_t* tmp = create_hook(env,"android/telephony/TelephonyManager", "getDeviceId", "()Ljava/lang/String;", testID);
    add_hook(tmp);
    LOGI("FINITO INIT HOOK !!!\n");
    //call_original_method(env, tmp);
    //_suspendAllForDbg(&d);    

}

int my_epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout)
{
        
    log("dentro my epoll wait \n");
	int (*orig_epoll_wait)(int epfd, struct epoll_event *events, int maxevents, int timeout);
	orig_epoll_wait = (void*)eph.orig;

    hook_precall(&eph);
	int res = orig_epoll_wait(epfd, events, maxevents, timeout);
	if (counter) {
		hook_postcall(&eph);
		log("epoll_wait() called\n");
		counter--;
		if (!counter)
			log("removing hook for epoll_wait()\n");
	}
    init_hook();
	return res;
}
// se injetto in zygote va in freeze il lancio dell'activity
// e non mi funziona l hook su epoll_wait
// 
void my_init(void)
{
	counter = 0;

	log("%s started\n", __FILE__)
 
	set_logfunction(my_log);
	resolve_symbols(&d);
  
    
	hook(&eph, getpid(), "libc.", "epoll_wait", my_epoll_wait_arm, my_epoll_wait);
    log("fine init\n");
}

