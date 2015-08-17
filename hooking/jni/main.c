#define _GNU_SOURCE
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <sys/epoll.h>

#include <jni.h>
#include <stdlib.h>


// adbi include
#include "base.h"

#include "artstuff.h"
#include "arthook_manager.h"


// this file is going to be compiled into a thumb mode binary

void __attribute__ ((constructor)) my_init(void);

static struct hook_t eph;
static struct hook_t invokeh;

char artlogfile[]  = "/data/local/tmp/arthook.log";

JavaVM* vms = NULL;


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
    LOGI("INIT HOOK INIZIO\n");
    JNIEnv* env = get_jnienv();
    arthook_manager_init(env);
    LOGI("FINITO INIT HOOK !!!\n");
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
        
    log("dentro my epoll wait \n");
	int (*orig_epoll_wait)(int epfd, struct epoll_event *events, int maxevents, int timeout);
	orig_epoll_wait = (void*)eph.orig;

    hook_precall(&eph);
	int res = orig_epoll_wait(epfd, events, maxevents, timeout);
	/*if (counter) {
		hook_postcall(&eph);
		log("epoll_wait() called\n");
		counter--;
		if (!counter)
			log("removing hook for epoll_wait()\n");
	}
	*/
    init_hook();
	return res;
}

void my_init(void)
{

    // adbi and arthook log functions
	set_logfunction(my_log);
    set_arthooklogfunction(artlogmsgtofile);
	log("%s started\n", __FILE__)
    
    // resolve libart.so symbols used by artstuff.c
	resolve_symbols(&d);
  
    // hook native functions
	hook(&eph, getpid(), "libc.", "epoll_wait", my_epoll_wait_arm, my_epoll_wait);
    hook(&invokeh, getpid(), "libart.", "_ZN3art12InvokeMethodERKNS_18ScopedObjectAccessEP8_jobjectS4_S4_", my_epoll_wait_arm, my_invoke_method);

    log("fine %s \n", __PRETTY_FUNCTION__);
}

