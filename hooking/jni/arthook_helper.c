/*
   Helper functions

   
*/


#include "arthook_helper.h"


/*
   set the hook, pointed by arthook_t*, in memory.
   */
int set_hook(JNIEnv *env, arthook_t *h)
{
    arthooklog("set_hook\n");
    unsigned int *res = searchInMemoryVtable( (unsigned int) h->original_meth_ID, (unsigned int) h->original_meth_ID, isLollipop(env));
    if(res == 0) return 0;
    LOGI("set_pointer 0x%08x with 0x%08x \n", (unsigned int) h->original_meth_ID, (unsigned int) h->hook_meth_ID);
    breakMe();
    set_pointer(res, (unsigned int ) h->hook_meth_ID); //TODO: set_vtable_pointer
/*    
       res = searchInMemoryVmeths( (unsigned int) h->original_meth_ID, (unsigned int) h->original_meth_ID, isLollipop(env));
       if(res == 0) return 0;
       LOGI("trovato SECONDO GIRO pointer at 0x%x \n",  res);
       set_pointer(res, (unsigned int ) h->hook_meth_ID); //TODO: set_vtable_pointer
*/    
    LOGI("analyze2 finito set_pointer!!!! \n");
    return 1;
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
arthook_t* create_hook(JNIEnv *env, char *clsname, const char* mname,const  char* msig, jclass hook_cls, jmethodID hookm)
{
    LOGI("------------------------------------------------------------------------------\n\n");  
    LOGI("%s mname: %s , msig: %s \n ", __PRETTY_FUNCTION__, mname, msig);
  
    arthook_t *tmp = NULL;
    jclass target = NULL;
    jclass gTarget = NULL;
    jmethodID target_meth_ID = NULL;
    
    tmp = (arthook_t*) malloc(sizeof(struct arthook_t));
    if(!tmp){
        LOGI("error malloc!\n");
        return NULL;           
    }

    strncpy(tmp->clsname, clsname, sizeof(tmp->clsname));
    strncpy(tmp->mname, mname, sizeof(tmp->mname));
    strncpy(tmp->msig, msig, sizeof(tmp->msig));

    memset(tmp->key, 0, sizeof(tmp->key));
    strcat(tmp->key,clsname);
    strcat(tmp->key,mname);
    strcat(tmp->key,msig);
    jclass test;
    jmethodID testid;
    jobject tryme;
    
    if(strstr(tmp->key, "sendTextMessage")) {
        LOGI(" CREATE HOOK DI TIPO SMS \n");    
        jclass cls = (*env)->FindClass(env, "android/telephony/SmsManager");
        jfieldID field = (*env)->GetStaticFieldID(env, cls, "sInstance", "Landroid/telephony/SmsManager;");
        LOGI("preso static fiels sINstance: %x \n" , field);
        jobject o =  (*env)->GetStaticObjectField(env, cls, field);
        LOGI("preso static obj: %x \n", o);
        jclass target =(*env)->GetObjectClass(env, o);
        LOGI("preso CONFRONTAMI class from obj: 0x%08x\n", target);
        target_meth_ID = (*env)->GetMethodID(env, target, mname, msig);
        LOGI("preso target method id sms: %x \n", target_meth_ID);
        gTarget = (jclass) (*env)->NewGlobalRef(env, target);
        LOGI("target1 class: 0x%08x, target methodid 0x%08x, \n", target, target_meth_ID);

    }
    else{
        target = (*env)->FindClass(env, clsname);
        LOGI("TARGET CLAZZ %s at: %x \n", clsname, target);
        gTarget = (jclass) (*env)->NewGlobalRef(env, target);
        target_meth_ID = (*env)->GetMethodID(env, target, mname, msig);
        LOGI("TARGET METHODID is %d \n", target_meth_ID);
        //try to resolve hook class and method here
        //test = (*env)->FindClass(env, "java/lang/Object");
        //LOGI("trovata porcodio di classe %x \n", test);
        //testid = (*env)->GetMethodID(env, test, "toString", "()Ljava/lang/String;");
        //LOGI("avevo: %x , trovato : %x \n", hookm, testid);
        
    }    

    tmp->hook_meth_ID = hookm;
    //tmp->hook_meth_ID = target_meth_ID;
    tmp->original_meth_ID = target_meth_ID;
    tmp->original_obj = NULL;//little_hack(env,gTarget);
    tmp->original_cls = gTarget;
    tmp->hook_cls = hook_cls;
    //tmp->hook_obj = (*env)->NewGlobalRef(env,hook_thiz);
    set_hook(env, tmp);
    LOGI("------------------------------------------------------------------------------\n");    
    return tmp;
}


void* hh_check_javareflection_call(JNIEnv *env, jobject javaMethod, jobject javaReceiver)
{
    LOGI("chiamato %s \n", __PRETTY_FUNCTION__);
    jclass c = (*env)->FindClass(env, "java/lang/reflect/AbstractMethod");
    LOGI("trovata abstractmethod %x \n", c);

    jclass c1 = (*env)->FindClass(env, "java/lang/reflect/Method");
    LOGI("trovata method clazz : %x \n", c1);

    jfieldID fid = (*env)->GetFieldID(env, c, "artMethod", "Ljava/lang/reflect/ArtMethod;");
    LOGI("trovato fieldID %x \n", fid);

    // devo prendere l'offset dal fid (sta a +24)
    // e applicarlo al javamethod
    //

    unsigned char* myfid = fid;
    LOGI("primo : %x = %x , dopo sum: %x \n", myfid, *myfid, myfid+0x14);
    unsigned char* off = (unsigned char*) (myfid + 0x14);
    LOGI("!!! offset vale: %x %x \n", off, *off);

    unsigned int* myclazz = javaMethod;

    LOGI("my clazz: %x = %x \n", myclazz, *myclazz);

    unsigned int* mymid = (unsigned int*) (*myclazz + *off);

    LOGI("mio metodo target: %x = %x \n", mymid, *mymid);

    int res = is_method_in_hashtable(mymid);
    void* ret = NULL;
    if(res){
        arthook_t* tmp = (arthook_t*) get_method_from_hashtable(mymid);
        return tmp;
    }

    return res;
}

void tryToUnbox(JNIEnv* env, unsigned int* javaArgs)
{
    arthooklog("chiamato trytounbox con : %x \n", javaArgs);
    int * p = (int*) *javaArgs + 0x2;
    

    arthooklog("trovata size array: %d a %x \n", *p, p);

    calldiocane(env, (jobject) javaArgs);

}

void* callOriginalReflectedMethod(JNIEnv* env, jobject javaReceiver, arthook_t* tmp, jobject javaArgs){
    LOGI("trovato hook su : %s \n", tmp->clsname);
    tryToUnbox( env, (unsigned int) javaArgs);
    return call_original_method(env, tmp, javaReceiver, javaArgs);
}

jobject call_patch_method(JNIEnv* env, arthook_t* h, jobject thiz){

    LOGI("!!!!!!!!!!!!!!! call patch method from reflection, obj: %x ,  cls: %x method: %x, key: %s \n", thiz, h->original_cls, h->original_meth_ID, h->key);

    return (*env)->CallStaticObjectMethod(env, h->hook_cls, h->hook_meth_ID, thiz);    
}

// used by reflection calls
jobject call_original_method(JNIEnv* env, arthook_t* h, jobject thiz, jobject javaArgs) // jstring arg1)
{

    LOGI("!!!!!!!!!!!!!!! call original, obj: %x ,  cls: %x method: %x, key: %s \n", thiz, h->original_cls, h->original_meth_ID, h->key);

    jobject res = NULL;

    if(strstr(h->key, "sendTextMessage") ) 
    {
        //callOriginalSms(env, h);
    }
    else{
        jclass t = (*env)->GetObjectClass(env, thiz);
        LOGI("original  cls: %x\n", t);
        if(strstr(h->key, "openFileOutput")) {
            /*
            jsize len = (*env)->GetStringUTFLength(env, arg1);
            char* fileName = calloc(len + 1, 1);
            (*env)->GetStringUTFRegion(env, arg1, 0, len, fileName);
            LOGG("ORIGINAL FILE NAME: %s\n", fileName);
            jint arg2 = 0;
            LOGI("calling original openFileOutput \n");
            */
            res = (*env)->CallNonvirtualObjectMethod(env, thiz, t, h->original_meth_ID, javaArgs);    
        }
        else{
            res = (*env)->CallNonvirtualObjectMethod(env, thiz, t, h->original_meth_ID, NULL);    
        }
    }
    if(res != NULL)
        return (*env)->NewGlobalRef(env, res);
    return NULL;
}


/*
   Search a pointer in  the process' virtual memory, starting from address @start.

*/
static unsigned int* searchInMemoryVtable(unsigned int start, int gadget, int lollipop){
    LOGI("------------------------------------------------------------------------------\n");
    LOGI("start VTABLE : 0x%08x, gadget 0x%08x, lollipop = %d \n", start, gadget, lollipop);
    unsigned int i = 0;
    unsigned int* pClazz;
    unsigned int* pAccess_flags;
    unsigned int* vtable;
    unsigned int* vmethods;
    unsigned int* vmethods_len;
    unsigned int* csize;
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
        LOGI("access flag: %d \n", (*pAccess_flags & 0xFFFF) );
        //*pAccess_flags = *pAccess_flags | kAccStatic; // set method static
        //LOGI("access flag metodo: %d \n", (*pAccess_flags & 0xFFFF) );        
        vtable =  (unsigned int*) ((*pClazz) + VTABLE_OFF);
        vmethods_len = (unsigned int*) ((*vtable) + VMETHS_LEN_OFF);
        LOGI("vmethods len: %zd \n", *vmethods_len);

    }
    LOGI("clazz: 0x%08x, * = 0x%08x [] vtable=0x%08x, * = 0x%08x \n", pClazz , *pClazz, vtable, *vtable);
    if(!vtable) return 0;
    unsigned char *g2 = (unsigned char*) &gadget; 
    unsigned char *p = (unsigned char*)  *vtable; 
    //searching the method reference inside the class's vtable
    while(1){
        //vtable bound check
        if(i > (*vmethods_len * 4) )
            return 0;
        //LOGI("check p 0x%08x and gadget 0x%08x \n",p, gadget);
        if(! memcmp(p, g2, 4 )){
            unsigned int* found_at = (unsigned int*) (*vtable + i) ;
            LOGI("target VTABLE: 0x%08x at 0x%08x. vtable = %x, bytes = %d \n", gadget, found_at, *vtable, i);
            //changed = 1;
            return found_at;
        }
        p += 4; i += 4;
    }
    return 0;
    LOGI("------------------------------------------------------------------------------\n");

}

static unsigned int* searchInMemoryVmeths(unsigned int start, int gadget, int lollipop){
    LOGI("start VMETHODS: 0x%08x, gadget 0x%08x, lollipop = %d \n", start, gadget, lollipop);
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
        LOGI("access flag metodo: %x \n", *pAccess_flags & 0xFFFF );
        //*pAccess_flags = *pAccess_flags | kAccStatic; // set method static
        vtable =  (unsigned int*) ((*pClazz) + VTABLE_OFF);
        vmethods = (unsigned int*) ((*pClazz) + ITABLE_OFF);
        if(! (*vmethods) ) return 0;
        LOGI(" VMETHS clazz: 0x%08x, * = 0x%08x -> vmeths=0x%08x, * = 0x%08x \n", pClazz , *pClazz, vmethods, *vmethods);        
    }
    breakMe();
    unsigned char *g2 = (unsigned char*) &gadget; 
    unsigned char *p = (unsigned char*)  *vmethods; 
    LOGI("INIZIO RICERCA\n");
    //searching the method reference inside the class's vtable
    while(1){
        //LOGI("check p 0x%08x and gadget 0x%08x \n",p, gadget);
        if(! memcmp(p, g2, 4 )){
            unsigned int* found_at = (unsigned int*) (*vmethods + i) ;
            LOGI("target: 0x%08x at 0x%08x. vmeths = %x, i = %d \n", gadget, found_at, *vmethods, i);
            //changed = 1;

            return found_at;
        }
        p += 4; i += 4;
    }
    return 0;
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
    unsigned int *res = searchInMemoryVtable( (unsigned long) meth, (int) meth, lollipop);
    if(res == 0) return changed;
    else changed = 1;
    LOGI("trovato pointer at 0x%08x \n", res);
    //unsigned int *p = &(*res);
    //swap meth with meth2
    //oldpointer = (void*) *res;;
    set_pointer(res, (unsigned int) meth2);
    //LOGI("oldpointer vale: 0x%0x \n", oldpointer);
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
