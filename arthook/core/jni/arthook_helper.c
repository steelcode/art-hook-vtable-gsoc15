/*
   Helper functions

   
*/
#include "arthook_helper.h"


/*
   set the hook, pointed by arthook_t*, in memory.
   */
static int set_hook(JNIEnv *env, arthook_t *h)
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
        LOGI("preso static fiels sINstance: %p \n" , field);
        jobject o =  (*env)->GetStaticObjectField(env, cls, field);
        LOGI("preso static obj: %p \n", o);
        jclass target =(*env)->GetObjectClass(env, o);
        target_meth_ID = (*env)->GetMethodID(env, target, mname, msig);
        gTarget = (jclass) (*env)->NewGlobalRef(env, target);

    }
    else{
        target = (*env)->FindClass(env, clsname);
        LOGI("TARGET CLAZZ %s at: %p \n", clsname, target);
        gTarget = (jclass) (*env)->NewGlobalRef(env, target);
        target_meth_ID = (*env)->GetMethodID(env, target, mname, msig);
        //LOGI("TARGET METHODID is %d \n", target_meth_ID);
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

void* get_artmethod_from_reflection(JNIEnv* env, jobject javaMethod){
    LOGI("chiamato %s \n", __PRETTY_FUNCTION__);
    jclass c = (*env)->FindClass(env, "java/lang/reflect/AbstractMethod");
    LOGI("trovata abstractmethod %p \n", c);

    jclass c1 = (*env)->FindClass(env, "java/lang/reflect/Method");
    LOGI("trovata method clazz : %p \n", c1);

    jfieldID fid = (*env)->GetFieldID(env, c, "artMethod", "Ljava/lang/reflect/ArtMethod;");
    LOGI("trovato fieldID %p \n", fid);
    unsigned char* myfid = (unsigned char*) fid;
    //LOGI("primo : %x = %x , dopo sum: %x \n", myfid, *myfid, myfid+0x14);
    unsigned char* off = (unsigned char*) (myfid + 0x14);
    //LOGI("!!! offset vale: %x %x \n", off, *off);

    unsigned int* myclazz = javaMethod;

    //LOGI("my clazz: %x = %x \n", myclazz, *myclazz);

    unsigned int* mymid = (unsigned int*) (*myclazz + *off);

    //LOGI("mio metodo target: %x = %x \n", mymid, *mymid);

    if(mymid)
        return mymid;
    else
        return NULL;
}

void* hh_check_javareflection_call(JNIEnv *env, jobject javaMethod, jobject javaReceiver)
{
    unsigned int* mymid = get_artmethod_from_reflection(env,javaMethod);
    if(! mymid)
        return NULL;
    int res = is_method_in_hashtable(mymid);
    void* ret = NULL;
    if(res){
        arthook_t* tmp = (arthook_t*) get_method_from_hashtable(mymid);
        return tmp;
    }


    //porcodio
    jclass xxx = (*env)->FindClass(env, "android/content/Context");
    jmethodID xxxmid = (*env)->GetMethodID(env, xxx, "openFileOutput","(Ljava/lang/String;I)Ljava/io/FileOutputStream;");
    xxx = (*env)->FindClass(env, "android/app/ContextImpl");
    jmethodID xxxmid2 = (*env)->GetMethodID(env, xxx, "openFileOutput","(Ljava/lang/String;I)Ljava/io/FileOutputStream;");

    xxx = (*env)->FindClass(env, "android/app/Activity");
    jmethodID xxxmid3 = (*env)->GetMethodID(env, xxx, "openFileOutput","(Ljava/lang/String;I)Ljava/io/FileOutputStream;");
    unsigned int *pi = (unsigned int*) xxxmid;
    arthooklog("porcodio !!! forse cercavi: %p = %p\n", xxxmid3, xxxmid2);

    return NULL;
}

//i must try with array reflection

static jvalue* tryToUnbox(JNIEnv* env, arthook_t* hook, unsigned int* javaArgs,jobject thiz, bool call_patchmeth)
{
    arthooklog("chiamato trytounbox con : %p \n", javaArgs);
    int * p = (int*) *javaArgs + 0x2;
    

    arthooklog("trovata size array: %d a %p \n", *p, p);

    //calldiocane(env, (jobject) javaArgs);
    jobjectArray joa = (jobjectArray) javaArgs;

    int num = (*env)->GetArrayLength(env, joa);

    arthooklog("prova array con numero elementi: %d !!!! \n", num);

    jobject tmp;
    int counter = 0;
    int index = 0;

    if(call_patchmeth)
        num += 1;

    jvalue* args = calloc(num, sizeof(jvalue));

    char* res = parseSignature(hook->msig);

    char* tok = strtok(res, "|");

    jstring s;
    jobject o;
    jint i;
    jdouble d;
    jfloat f;
    jboolean z;
    jlong j;

    if(call_patchmeth) {
        args[0].l = thiz;
        counter++;
    }

    while(tok != NULL){
        arthooklog("trovato : %s\n", tok);
        if(*tok == 'L'){
            o = callGetObj(env, joa, (jint) index) ;
            arthooklog("gestisco un Object %p  = %x, counter = %d\n", o, (unsigned int) o, counter);
            args[counter].l = (*env)->NewGlobalRef(env, o);
            counter++;
            index++;
        }
        else if(*tok == 'I'){
            i = callGetInt(env, joa, (jint) index);
            arthooklog("gestisco un intero %d , counter = %d\n", i, counter);
            arthooklog("trovato int: %d\n", i);
            args[counter].i = i;
            counter++;
            index++;
        }
        else if(*tok == 'D'){
            arthooklog("gestisco un double, counter = %d\n", counter);
            d = callGetDouble(env, joa, (jint) index);
            arthooklog("trovato double: %d\n", i);
            args[counter].d = d;
            counter++;
            index++;
        }
        else if(*tok == 'J'){
              arthooklog("gestisco un long, counter = %d\n", counter);
            j = callGetLong(env, joa, (jint) index);
            arthooklog("trovato long: %d\n", i);
            args[counter].j = j;
            counter++;
            index++;
        }
        else if(*tok == 'Z'){
              arthooklog("gestisco un boolean, counter = %d\n", counter);
            z = callGetBoolean(env, joa, (jint) index);
            arthooklog("trovato int: %d\n", i);
            args[counter].z = z;
            counter++;
            index++;
        }
        tok = strtok(NULL, "|");
    }

    return args;
/*
    jclass t = (*env)->GetObjectClass(env, thiz);
    jsize len = (*env)->GetStringUTFLength(env, s);
    char* arg1 = calloc(len+1, 1);
    (*env)->GetStringUTFRegion(env, s, 0, len, arg1);
    arthooklog("creo filename: %s\n", arg1);
    int arg2 = 0;
    free(arg1);

    arthooklog("debug args: t = %x , thiz = %x, mid = %x, arg1 = %x , arg2= %x \n", t, thiz, hook->original_meth_ID, args[0].l, args[1].i);
    jobject ress = NULL;


    ress = (*env)->CallNonvirtualObjectMethod(env, thiz, t, hook->original_meth_ID, *args);

    arthooklog("fatta la chiamata da dentro trytounbox\n");
    if(ress != NULL)
       return (*env)->NewGlobalRef(env, ress);
    return NULL;
*/
}

void* callOriginalReflectedMethod(JNIEnv* env, jobject thiz, arthook_t* hook, jobject javaArgs){
    arthooklog("trovato hook su : %s \n", hook->clsname);
    arthooklog("mia signature: %s\n", hook->msig);

    jvalue* args;
    jobject res = NULL;

    if(javaArgs)
         args =  tryToUnbox( env, hook, (unsigned int*) javaArgs, thiz, false);
    else{
        jclass t = (*env)->GetObjectClass(env, thiz);
        res = (*env)->CallNonvirtualObjectMethod(env, thiz, t, hook->original_meth_ID);
    }
    if(args){
        jclass t = (*env)->GetObjectClass(env, thiz);
        res = (*env)->CallNonvirtualObjectMethodA(env, thiz, t, hook->original_meth_ID,args);

        arthooklog("fatta la chiamata da dentro trytounbox\n");
        if(res != NULL)
           return (*env)->NewGlobalRef(env, res);
    }

    //return call_original_method(env, tmp, javaReceiver, myargs);
}

//XXX: devo  gestire gli argomenti
jobject call_patch_method(JNIEnv* env, arthook_t* h, jobject thiz, jobject javaArgs){

    //LOGI("!!!!!!!!!!!!!!! call patch method from reflection, obj: %x ,  cls: %x method: %x, key: %s \n", thiz, h->original_cls, h->original_meth_ID, h->key);

    jvalue* args;
    jobject res = NULL;

    if(javaArgs){
        args = tryToUnbox(env, h, (unsigned int*) javaArgs, thiz, true);
    }
    else{
        res = (*env)->CallStaticObjectMethod(env, h->hook_cls, h->hook_meth_ID, thiz);
        return (*env)->NewGlobalRef(env, res);
    }
    if(args){
        jclass t = (*env)->GetObjectClass(env, thiz);
        res = (*env)->CallStaticObjectMethodA(env, h->hook_cls, h->hook_meth_ID, args);

        arthooklog("fatta la chiamata da dentro trytounbox\n");
        return (*env)->NewGlobalRef(env, res);
    }
    else{
        return NULL;
    }

//    return (*env)->CallStaticObjectMethod(env, h->hook_cls, h->hook_meth_ID, thiz);
}

// used by reflection calls
jobject call_original_method(JNIEnv* env, arthook_t* h, jobject thiz,jvalue* myargs) // jstring arg1)
{

    //LOGI("!!!!!!!!!!!!!!! call original, obj: %x ,  cls: %x method: %x, key: %s \n", thiz, h->original_cls, h->original_meth_ID, h->key);

    jobject res = NULL;

    if(strstr(h->key, "sendTextMessage") ) 
    {
        //callOriginalSms(env, h);
    }
    else{
        jclass t = (*env)->GetObjectClass(env, thiz);
        LOGI("original  cls: %p\n", t);
        if(strstr(h->key, "openFileOutput")) {
            /*
            jsize len = (*env)->GetStringUTFLength(env, arg1);
            char* fileName = calloc(len + 1, 1);
            (*env)->GetStringUTFRegion(env, arg1, 0, len, fileName);
            LOGG("ORIGINAL FILE NAME: %s\n", fileName);
            jint arg2 = 0;
            LOGI("calling original openFileOutput \n");
            */
            res = (*env)->CallNonvirtualObjectMethod(env, thiz, t, h->original_meth_ID, *myargs);
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
    //LOGI("start VTABLE : 0x%08x, gadget 0x%08x, lollipop = %d \n", start, gadget, lollipop);
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
        //LOGI("LOLLIPOP clazz: 0x%08x, * = 0x%08x -> vtable=0x%08x, * = 0x%08x, vmethods = 0x%08x, * = 0x%08x \n", pClazz , *pClazz, vtable, *vtable, vmethods, *vmethods );

    }
    //kitkat 4.4.4 with art offsets
    else{
        pClazz = (unsigned int*) (start + CLAZZ_OFF);
        pAccess_flags = (unsigned int*) (start + ACCESS_FLAG_OFF);
        //LOGI("access flag: %d \n", (*pAccess_flags & 0xFFFF) );
        //*pAccess_flags = *pAccess_flags | kAccStatic; // set method static
        //LOGI("access flag metodo: %d \n", (*pAccess_flags & 0xFFFF) );        
        vtable =  (unsigned int*) ((*pClazz) + VTABLE_OFF);
        vmethods_len = (unsigned int*) ((*vtable) + VMETHS_LEN_OFF);
        //LOGI("vmethods len: %zd \n", *vmethods_len);

    }
    //LOGI("clazz: 0x%08x, * = 0x%08x [] vtable=0x%08x, * = 0x%08x \n", pClazz , *pClazz, vtable, *vtable);
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
            //LOGI("target VTABLE: 0x%08x at 0x%08x. vtable = %x, bytes = %d \n", gadget, found_at, *vtable, i);
            //changed = 1;
            return found_at;
        }
        p += 4; i += 4;
    }
    return 0;
    LOGI("------------------------------------------------------------------------------\n");

}

static unsigned int* searchInMemoryVmeths(unsigned int start, int gadget, int lollipop){
    //LOGI("start VMETHODS: 0x%08x, gadget 0x%08x, lollipop = %d \n", start, gadget, lollipop);
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
        //LOGI("LOLLIPOP clazz: 0x%08x, * = 0x%08x -> vtable=0x%08x, * = 0x%08x, vmethods = 0x%08x, * = 0x%08x \n", pClazz , *pClazz, vtable, *vtable, vmethods, *vmethods );
    }
    //kitkat 4.4.4 with art offsets
    else{
        pClazz = (unsigned int*) (start + CLAZZ_OFF);
        pAccess_flags = (unsigned int*) (start + ACCESS_FLAG_OFF);
        //LOGI("access flag metodo: %x \n", *pAccess_flags & 0xFFFF );
        //*pAccess_flags = *pAccess_flags | kAccStatic; // set method static
        vtable =  (unsigned int*) ((*pClazz) + VTABLE_OFF);
        vmethods = (unsigned int*) ((*pClazz) + ITABLE_OFF);
        if(! (*vmethods) ) return 0;
        //LOGI(" VMETHS clazz: 0x%08x, * = 0x%08x -> vmeths=0x%08x, * = 0x%08x \n", pClazz , *pClazz, vmethods, *vmethods);
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
            //LOGI("target: 0x%08x at 0x%08x. vmeths = %x, i = %d \n", gadget, found_at, *vmethods, i);
            //changed = 1;

            return found_at;
        }
        p += 4; i += 4;
    }
    return 0;
}

