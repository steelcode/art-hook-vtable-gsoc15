#include "utils.h"


/*
    @name checkAPIVersion
    @brief return the API version number
    @retval int API number version

*/
jint getAPIVersion(JNIEnv *env){
    jclass versionClass = (*env)->FindClass(env, "android/os/Build$VERSION");
    jfieldID sdkIntFieldID = (*env)->GetStaticFieldID(env, versionClass, "SDK_INT", "I");
    jint sdkInt = (*env)->GetStaticIntField(env, versionClass, sdkIntFieldID);
    LOGI("API version: %d \n", sdkInt);
    return sdkInt;

}

int isLollipop(JNIEnv *env)
{
    jint res = getAPIVersion(env);
    if(res > 19 )
        return 1;
    else return 0;
}

char* parseSignature(char* sig){
    arthooklog("mia signature: %s\n", sig);
    char* p = sig;
    char* result = calloc(strlen(sig)+1, sizeof(char));


    char* copy = calloc(strlen(sig)+1, sizeof(char));
    strcpy(copy, sig);

    char* token = strtok(copy, "( )");
    arthooklog("mio token: %s \n", token);

    char* obj = strtok(token, ";");
    arthooklog("trovato obj: %s \n", obj);

    while( *p ){
        size_t counter = 0;
        arthooklog("char: %c \n", *p);
        if(*p == ')')
            break;
        if(p[counter] == 'L'){
            arthooklog("trovata classe!!\n");
            strcat(result, "L|");
            arthooklog("result vale: %s \n", result);
            char* res = strchr(p, ';');
            p = res;
            counter = (res - sig + 1);
        }
        if(p[counter] == 'I'){
            strcat(result, "I|");
        }
        *p++;
    }
    return result;
}

jclass _findClass(JNIEnv* env, char* clsname)
{
    jclass cls = (*env)->FindClass(env, clsname);
    if(!cls){
        LOGI("findClass NULL!!\n");
        return (jclass) -1;
    }
    else
        return cls;
}

jmethodID fromObjToMethodID(JNIEnv *env, jobject target, char * mname, char*  msig)
{
    jclass cls = (*env)->GetObjectClass(env,  target);
    if(!cls){
        LOGI("resolve_target NULL!!! \n");
        return (jmethodID) -1;
    }
    else
        return getMethodID(env, cls, mname, msig);
}

jmethodID getMethodID(JNIEnv *env,jclass cls, char *mname, char* msig){
    return (*env)->GetMethodID(env, cls, mname, msig);
}


void callTestJNI(JNIEnv *env, jobject obj){
  LOGI("chiamato testjni\n");
  jclass cls = (*env)->GetObjectClass(env, obj);
  jmethodID mid = (*env)->GetMethodID(env, cls, MNAME, MSIG);
  if (mid == 0){
    LOGI("error mid\n");
    return;
    }
  jobject res = (*env)->CallObjectMethod(env, obj, mid);
    const char *str = (*env)->GetStringUTFChars(env, (jstring) res, NULL);
    LOGI("ritornato: %s \n", str);
    breakMe();
}
void findClass_test(JNIEnv* env)
{
    LOGI("chiamato findclass test \n");
    jclass cls = (*env)->FindClass(env, HOOKCLS);
}
void breakMe()
{
    return;
}

void set_pointer(unsigned int *s, unsigned int d){
    *s = d;
}


char * get_dev_id(JNIEnv *env, jobject context)
{
    jclass cls = (*env)->FindClass(env, "android/context/Context");
    // ---------------- cls == NULL all the time

    jmethodID mid = (*env)->GetMethodID(env, cls, "getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;");
    jfieldID fid = (*env)->GetStaticFieldID(env, cls, "TELEPHONY_SERVICE", "Ljava/lang/String;");
    jstring str = (jstring) (*env)->GetStaticObjectField(env, cls, fid);
    jobject telephony = (*env)->CallObjectMethod(env, context, mid, str);
    cls = (*env)->FindClass(env, "android/telephony/TelephonyManager");
    mid =(*env)->GetMethodID(env, cls, "getDeviceId", "()Ljava/lang/String;");
    str = (jstring) (*env)->CallObjectMethod( env, telephony, mid);
    return getCharFromJstring(env, str);
}

jobject createInstanceFromJClsName(JNIEnv* env, jstring jstr)
{
    const char * clsname;
    clsname = (*env)->GetStringUTFChars(env,jstr , NULL ) ;
    return createInstanceFromClsName(env, (char*) clsname);

}
char* getCharFromJstring(JNIEnv* env, jstring jstr)
{
    jsize len = (*env)->GetStringUTFLength(env, jstr);
    char* res = (char*) calloc(len+1,1);
    (*env)->GetStringUTFRegion(env, jstr, 0, len, res);
    //(*env)->DeleteLocalRef(env, jstr);
    LOGI("getcharfromj, res: %s \n", res);
    return res;
}
jobject createInstanceFromClsName(JNIEnv* env, char* clsname)
{
    LOGI("testjni str, search: %s \n", clsname);
    jclass target = (*env)->FindClass(env,clsname);
    if(!target)
        return (jobject)-1;
    jmethodID c = (*env)->GetMethodID(env,target, "<init>", "()V");
    jobject targetcls = (*env)->NewObject(env,target, c, NULL);
    LOGI("ok\n");
    return targetcls;
}
