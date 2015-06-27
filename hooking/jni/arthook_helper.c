/*
    Functions to help with the hooking steps.
*/
#include "arthook_helper.h"

void *oldpointer;

/*
    set the hook, pointed by arthook_t*, in memory.
*/
int set_hook(JNIEnv *env, arthook_t *h)
{
    LOGI("set_hook\n");
    unsigned int *res = searchInMemory( (unsigned int) h->original_meth_ID, (unsigned int) h->original_meth_ID, isLollipop(env));
        if(res == 0) return 0;
    LOGI("trovato pointer at 0x%p \n",  res);
    LOGI("set_pointer 0x%08x con 0x%08x \n", (unsigned int) h->original_meth_ID, (unsigned int) h->hook_meth_ID);
    breakMe();
    set_pointer(res, (unsigned int ) h->hook_meth_ID); //TODO: set_vtable_pointer
    LOGI("analyze2 finito set_pointer!!!! \n");
    return 1;
}

void* call_original_method(JNIEnv* env, arthook_t* h)
{
    LOGI("!!!!!!!!!!!!!!!dentro call original method: %x, key: %s \n", h->original_meth_ID, h->key);
    jstring res = (*env)->CallStaticObjectMethod(env, h->original_cls, h->original_meth_ID, NULL);
    if(res){
        char* s = (*env)->GetStringUTFChars(env, res, 0);
        LOGI("deviceid original: %s \n", s);
        (*env)->ReleaseStringUTFChars(env, res, s);
    }
    return res;
}

/*
    @create_hook:

    This function create an arthook_t which represent the hook on the method "mname" of the "clsname"
    inside the "target" object.

    @param clsname target class name
    @param mname traget method name
    @param msig target method signature

    @return arthook_t the created arthook_t

*/
arthook_t* create_hook(JNIEnv *env, char *clsname, const char* mname,const  char* msig, jmethodID hookm)
{
    jclass hookcls = NULL;
    arthook_t *tmp;
    tmp = (arthook_t*) malloc(sizeof(struct arthook_t));
    LOGI("chiamato create_hook con mname: %s , msig: %s \n ", mname, msig);
    jobject target = (*env)->NewGlobalRef(env, createInstanceFromClsName(env, clsname) );
    jclass target_cls = (*env)->NewGlobalRef(env, (*env)->FindClass(env, clsname) );
    jobject gTarget = (*env)->NewGlobalRef(env, (*env)->NewGlobalRef(env, target) );

    //arthook_t* tmp = (arthook_t*) malloc(sizeof(struct arthook_t));
    LOGI("creata struct %p \n", tmp);
    strncpy(tmp->clsname, clsname, sizeof(tmp->clsname));
    strncpy(tmp->mname, mname, sizeof(tmp->mname));
    strncpy(tmp->msig, msig, sizeof(tmp->msig));
    memset(tmp->key, 0, sizeof(tmp->key));
    strcat(tmp->key,clsname);
    strcat(tmp->key,mname);
    strcat(tmp->key,msig);
    LOGI("MY KEY: %s \n", tmp->key);

    jmethodID target_meth_ID = fromObjToMethodID(env, target, (char*) mname, (char*) msig);
    if(hookm == NULL){
        LOGI("cerco cls: %s mname = %s , msig = %s \n " , HOOKCLS, HOOKM, HOOKMSIG);
        hookcls = (*env)->FindClass(env, HOOKCLS);
        hookm = (*env)->GetMethodID(env, hookcls, HOOKM, HOOKMSIG);
    }
    LOGI("trovata class: 0x%08x, trovato methodid 0x%08x, target: 0x%08x \n", hookcls, hookm, target_meth_ID);

    tmp->hook_cls = hookcls; //unused
    tmp->hook_meth_ID = hookm;
    tmp->original_meth_ID = target_meth_ID;
    tmp->original_obj = target;
    tmp->original_cls = target_cls;
    set_hook(env, tmp);
    
    return tmp;


}
/*
    Search a pointer in  the process' virtual memory, starting from address @start.

*/
static unsigned int* searchInMemory(unsigned int start, int gadget, int lollipop){
    LOGI("start: 0x%08x, gadget 0x%08x, lollipop = %d \n", start, gadget, lollipop);
    unsigned int i = 0;
    unsigned int *pClazz;
    unsigned int* pAccess_flags;
    unsigned int *vtable;
    unsigned int *vmethods;
    // > 4.4.4 offsets
    if(lollipop){
        pClazz = (unsigned int*) (start + LOLLIPOP_CLAZZ_OFF);
        vtable =  (unsigned int*) ((*pClazz) +  LOLLIPOP_VMETHODS_OFF);
        vmethods =  (unsigned int*) ((*pClazz) +  LOLLIPOP_VMETHODS_OFF);
        LOGI("LOLLIPOP clazz: 0x%08x, * = 0x%08x -> vtable=0x%08x, * = 0x%08x, vmethods = 0x%08x, * = 0x%08x \n", pClazz , *pClazz, vtable, *vtable, vmethods, *vmethods );

    }
    //kitkat 4.4.4 with art offsets
    else{
        pClazz = (unsigned int*) (start + CLAZZ_OFF);
        pAccess_flags = (unsigned int*) (start + ACCESS_FLAG_OFF);
        LOGI("access flag metodo: %x \n", pAccess_flags);
        *pAccess_flags = *pAccess_flags | kAccStatic; // set method static
        vtable =  (unsigned int*) ((*pClazz) + VTABLE_OFF);
    }
    LOGI("clazz: 0x%08x, * = 0x%08x -> vtable=0x%08x, * = 0x%08x \n", pClazz , *pClazz, vtable, *vtable);

    breakMe();

    unsigned char *g2 = (unsigned char*) &gadget; //this must be an uint*
    unsigned char *p = (unsigned char*)  *vtable; //this too
    //searching the method reference inside the class's vtable
    while(1){
        //LOGI("check p 0x%08x and gadget 0x%08x \n",p, gadget);
        if(! memcmp(p, g2, 4 )){
            unsigned int* found_at = (unsigned int*) (*vtable + i) ;
            LOGI("target: 0x%08x at 0x%08x. vtable = %x, i = %d \n", gadget, found_at, *vtable, i);
            //changed = 1;

            return found_at;
        }
        p += 4; i += 4;
    }
    return 0;
}
/*
    Create an instance of the classes named target_cls and patch_cls
    call search_and_set with these objects as arguments
*/
int search_and_set_str(JNIEnv* env, char* target_cls, char* patch_cls)
{
    jobject target = createInstanceFromClsName(env, target_cls);
    jobject patch = createInstanceFromClsName(env, patch_cls);

    return search_and_set(env, target, patch, isLollipop(env));

}
/*
    NOP
*/
void search_and_set2(unsigned long targetm, int proxym, int lollipop)
{
    unsigned int *res = searchInMemory( targetm, targetm, lollipop);
    if(res == 0) return;
    LOGI("trovato pointer at 0x%08x \n", res);
    LOGI("set_pointer 0x%08x con 0x%08x \n", targetm, proxym);
    set_pointer(res, proxym);
    LOGI("analyze2 finito set_pointer!!!! \n");
}

/*
    Wrapper function
*/
int search_and_set(JNIEnv *env, jobject c1, jobject c2, int lollipop){
    int changed = 0;
    LOGI("pointer to obj: %p e %p\n", c1, c2);
    //get pointer to class object
    jclass cls1 = (*env)->GetObjectClass(env, c1);
    jclass cls2 = (*env)->GetObjectClass(env, c2);
    LOGI("pointer to classes: 0x%08x, %p e %p\n", &cls1, cls1, cls2);
    //get methodID
    jmethodID meth = getMethodID(env, cls1, MNAME, MSIG);
    jmethodID meth2 = getMethodID(env, cls2, MNAME, MSIG);
    if(!meth || !meth2) return changed;
    LOGI("meth c1 vale: 0x%08x, meth c2 vale: %p \n", meth, meth2);
    callTestJNI(env,c1); //helloprinter!
    breakMe();
    unsigned int *res = searchInMemory( (unsigned long) meth, (int) meth, lollipop);
    if(res == 0) return changed;
    else changed = 1;
    LOGI("trovato pointer at 0x%08x \n", res);
    //unsigned int *p = &(*res);
    //swap meth with meth2
    oldpointer = (void*) *res;;
    set_pointer(res, (unsigned int) meth2);
    LOGI("oldpointer vale: 0x%0x \n", oldpointer);
    //check successful set_pointer
    meth = getMethodID(env, cls1, MNAME, MSIG);
    meth2 = getMethodID(env, cls2, MNAME, MSIG);
    if(!meth || !meth2) return changed;
    LOGI("SECONDO GIRO: meth c1 vale: 0x%08x, meth c2 vale: 0x%0x \n", meth, meth2);
    breakMe();
    return changed;
}

    /*
    unsigned int revgadget;
    revgadget =  ((gadget>>24)&0xff) | // move byte 3 to byte 0
                        ((gadget<<8)&0xff0000) | // move byte 1 to byte 2
                        ((gadget>>8)&0xff00) | // move byte 2 to byte 1
                        ((gadget<<24)&0xff000000); // byte 0 to byte 3
    LOGI("reverse : 0x%x\n ", revgadget);
    */
