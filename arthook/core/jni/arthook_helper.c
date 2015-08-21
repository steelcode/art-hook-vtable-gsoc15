#include "arthook_helper.h"
/*
   wrapper function to hijack the vtable's pointer
*/
static int set_hook(JNIEnv *env, arthook_t *h)
{
    unsigned int* res;
    arthooklog("set_hook\n");
    if(h->isSingleton){
        // search the pointer inside the vtable
        res = searchInMemoryVtable( (unsigned int) h->original_cls, (unsigned int) h->original_meth_ID, isLollipop(env), false);
    }
    else{
        res = searchInMemoryVtable( (unsigned int) h->original_meth_ID, (unsigned int) h->original_meth_ID, isLollipop(env), false);
    }
    if(res == 0) {
        LOGI("search returned 0\n");
        return 0;
    }
    LOGI("set_pointer 0x%08x with 0x%08x \n", (unsigned int) h->original_meth_ID, (unsigned int) h->hook_meth_ID);
    // change the pointer in the vtable
    set_pointer(res, (unsigned int ) h->hook_meth_ID);
/*    
       res = searchInMemoryVmeths( (unsigned int) h->original_meth_ID, (unsigned int) h->original_meth_ID, isLollipop(env));
       if(res == 0) return 0;
       LOGI("trovato SECONDO GIRO pointer at 0x%x \n",  res);
       set_pointer(res, (unsigned int ) h->hook_meth_ID); //TODO: set_vtable_pointer
*/    
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

    // TODO: addding of this hook in an automate way
    // SmsManager uses a singleton which in create at loading time
    if(strstr(tmp->key, "sendTextMessage")) {
        jclass cls = (*env)->FindClass(env, clsname);
        jmethodID getd = (*env)->GetStaticMethodID(env,cls, "getDefault", "()Landroid/telephony/SmsManager;");

        jobject res = (*env)->CallStaticObjectMethod(env, cls, getd);

        jfieldID field = (*env)->GetStaticFieldID(env, cls, "sInstance", "Landroid/telephony/SmsManager;");
        jobject o =  (*env)->GetStaticObjectField(env, cls, field);
        LOGI("singleton : %p \n", o);
        LOGI("SmsManager obj : %p\n", res);


        jclass target =(*env)->GetObjectClass(env, o);
        jmethodID mid = (*env)->GetMethodID(env,target, mname, msig);
        LOGI("singleton mid: %p \n", mid);
        target_meth_ID = (*env)->GetMethodID(env, target, mname, msig);
        LOGI("target mid: %p \n", target_meth_ID);
        gTarget = field;

        tmp->isSingleton = false;

    }
    else{
        // find the target class using JNI
        target = (*env)->FindClass(env, clsname);
        // make it a global ref for future access
        gTarget = (jclass) (*env)->NewGlobalRef(env, target);
        // get mid of the target method to hook
        target_meth_ID = (*env)->GetMethodID(env, target, mname, msig);
        tmp->isSingleton = false;
        
    }    
    // populate the arthook_t
    tmp->hook_meth_ID = hookm;
    tmp->original_meth_ID = target_meth_ID;
    tmp->original_obj = NULL;
    tmp->original_cls = gTarget;
    tmp->hook_cls = hook_cls;
    // set the hook in memory
    set_hook(env, tmp);
    LOGI("------------------------------------------------------------------------------\n");    
    return tmp;
}

/*
 * @get_artmethod_from_reflection:
 *
 * Called when an call to native MethodInvoke function is trapped by the framework.
 * This function retrive from the memory the called ArtMethod which is compared with
 * the hooked methods hashdict.
 *
 * @return the mid or NULL
 */

void* get_artmethod_from_reflection(JNIEnv* env, jobject javaMethod){
    LOGI("called %s with %p \n", __PRETTY_FUNCTION__, javaMethod);
    jclass c = (*env)->FindClass(env, "java/lang/reflect/AbstractMethod");
    jclass c1 = (*env)->FindClass(env, "java/lang/reflect/Method");
    jfieldID fid = (*env)->GetFieldID(env, c, "artMethod", "Ljava/lang/reflect/ArtMethod;");
    unsigned char* myfid = (unsigned char*) fid;
    unsigned char* off = (unsigned char*) (myfid + 0x14); //memory offset
    unsigned int* myclazz = javaMethod;
    unsigned int* mymid = (unsigned int*) (*myclazz + *off);
    if(mymid) {
        LOGI("%s,founded mid: %p \n", __PRETTY_FUNCTION__,mymid);
        return mymid;
    }
    else {
        LOGI("%s, Error!!\n", __PRETTY_FUNCTION__);
        return NULL;
    }
}

/*
 * @hh_check_javareflection_call:
 *
 * Check if the trapped call (made using Java reflection) is to a hooked method
 *
 * @return arthook_t or NULL
 *
 */
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
    return NULL;
}

/*
 * @tryToUnbox
 *
 * Unbox arguments of a Java reflection call
 *
 */
static jvalue* tryToUnbox(JNIEnv* env, arthook_t* hook, unsigned int* javaArgs,jobject thiz, bool call_patchmeth)
{
    arthooklog("called %s with : %p \n", __PRETTY_FUNCTION__, javaArgs);
    int * p = (int*) *javaArgs + 0x2;
    jobjectArray joa = (jobjectArray) javaArgs;
    int num = (*env)->GetArrayLength(env, joa);
    arthooklog("array con numero elementi: %d !!!! \n", num);
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
            arthooklog("Object %p  = %x, counter = %d\n", o, (unsigned int) o, counter);
            args[counter].l = (*env)->NewGlobalRef(env, o);
            counter++;
            index++;
        }
        else if(*tok == 'I'){
            i = callGetInt(env, joa, (jint) index);
            arthooklog("int: %d\n", i);
            args[counter].i = i;
            counter++;
            index++;
        }
        else if(*tok == 'D'){
            d = callGetDouble(env, joa, (jint) index);
            arthooklog("double: %d\n", i);
            args[counter].d = d;
            counter++;
            index++;
        }
        else if(*tok == 'J'){
            j = callGetLong(env, joa, (jint) index);
            arthooklog("trovato long: %d\n", i);
            args[counter].j = j;
            counter++;
            index++;
        }
        else if(*tok == 'Z'){
            z = callGetBoolean(env, joa, (jint) index);
            arthooklog("result int: %d\n", i);
            args[counter].z = z;
            counter++;
            index++;
        }
        tok = strtok(NULL, "|");
    }
    return args;
}

/*
 * @callOriginalReflectionmethod
 *
 * used to call the original method of a java reflection call.
 *
 * @return a global ref to the returned value (if any)
 */
void* callOriginalReflectedMethod(JNIEnv* env, jobject thiz, arthook_t* hook, jobject javaArgs){
    arthooklog("%s, target hook clsname: %s \n", __PRETTY_FUNCTION__, hook->clsname);
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
        if(strstr(hook->key, "sendTextMessage")){
            (*env)->CallNonvirtualVoidMethodA(env, thiz, t, hook->original_meth_ID,args);
        }
        else
            res = (*env)->CallNonvirtualObjectMethodA(env, thiz, t, hook->original_meth_ID,args);
        if(res != NULL)
           return (*env)->NewGlobalRef(env, res);
        else return NULL;
    }
}

/*
 * @call_patch_method
 *
 * Call the patch method associated to the arthook_t h
 *
 */
jobject call_patch_method(JNIEnv* env, arthook_t* h, jobject thiz, jobject javaArgs){
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
        return (*env)->NewGlobalRef(env, res);
    }
    else{
        return NULL;
    }
}

/*
   Search a pointer in  the process' virtual memory, starting from address @start.

*/
static unsigned int* searchInMemoryVtable(unsigned int start, int gadget, int lollipop, bool isClass){
    LOGI("------------------------------------------------------------------------------\n");
    LOGI("start : 0x%08x, gadget 0x%08x, lollipop = %d \n", start, gadget, lollipop);
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

        pClazz = (unsigned int *) (start + CLAZZ_OFF);
        pAccess_flags = (unsigned int *) (start + ACCESS_FLAG_OFF);
        vtable =  (unsigned int*) ((*pClazz) + VTABLE_OFF);
        vmethods_len = (unsigned int*) ((*vtable) + VMETHS_LEN_OFF);
        LOGI("pclazz: %x = %x\n", pClazz, *pClazz);
        LOGI("vmethods len: %zd \n", *vmethods_len);

    }
    LOGI("clazz: 0x%08x, * = 0x%08x [] vtable=0x%08x, * = 0x%08x \n", pClazz , *pClazz, vtable, *vtable);
    if(!vtable) return 0; //TODO: error
    unsigned char *g2 = (unsigned char*) &gadget; 
    unsigned char *p = (unsigned char*)  *vtable; 
    //searching the method reference inside the class's vtable
    while(1){
        //vtable bound check
        if(i > (*vmethods_len * 4) + 4  )
            return 0;
        LOGI("check p 0x%08x and gadget 0x%08x \n",p, gadget);
        if(! memcmp(p, g2, 4 )){
            unsigned int* found_at = (unsigned int*) (*vtable + i) ;
            LOGI("target VTABLE: 0x%08x at 0x%08x. vtable = %x, bytes = %d \n", gadget, found_at, *vtable, i);
            return found_at;
        }
        p += 4; i += 4;
    }
    LOGI("------------------------------------------------------------------------------\n");
}

// TODO: to be completed
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

