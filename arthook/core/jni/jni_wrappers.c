#include "jni_wrappers.h"

JNIEnv* getEnv() 
{
    JNIEnv *env;
    int status = vms[0]->GetEnv(vms,(void**)&env, JNI_VERSION_1_6);
    if(status < 0) {    
        status = vms[0]->AttachCurrentThread(vms, &env, NULL);
        if(status < 0) {        
            return NULL;
        }
    }
    return env;
}

extern JNIEnv* get_jnienv(){
	jsize vm_count = NULL;
	jsize size = NULL;
    if(vms == NULL)
    	_GetCreatedJavaVMs(&d, (void**) &vms, size, &vm_count);
	log("called getcreatedjavavms, results: %p , count = %d \n", vms[0], vm_count);
    JNIEnv* env = getEnv(vms);
	log("jnienv = 0x%08x \n", (unsigned int) env);
    return env;
}


jobject createDexClassLoader(JNIEnv* env, jobject classLoader, char* mydexpath, char* myoptdir)
{
    jthrowable exc;
    LOGI("%s , pid: %d\n",__PRETTY_FUNCTION__, getpid());
    jclass dexclassloader_cls = (*env)->FindClass(env, "dalvik/system/DexClassLoader");
    LOGI("dexclassloader class = %p \n", dexclassloader_cls);
    jmethodID constructor = (*env)->GetMethodID(env, dexclassloader_cls, "<init>", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/ClassLoader;)V");
    LOGI("init methodid = %p \n", constructor);
    char* dexPath = mydexpath;
    // il proprietario di questa dir deve essere l'app
    char* optDir = myoptdir;
    jstring dexPathJ = (*env)->NewStringUTF(env, dexPath);
    jstring optDirJ = (*env)->NewStringUTF(env, optDir);
    char* libraryPath = "";
    jobject dexloader = (*env)->NewObject(env, dexclassloader_cls, constructor, dexPathJ, optDirJ, NULL, classLoader);
    exc = (*env)->ExceptionOccurred(env);
    if(exc){
        (*env)->ExceptionDescribe(env);
        (*env)->ExceptionClear(env);
    }
    LOGI("created dex loader = %p \n ", dexloader);
    return dexloader;
}

jobject getSystemClassLoader(JNIEnv* env)
{
    jclass classLoader = (*env)->FindClass(env, "java/lang/ClassLoader");
    LOGI("classloader class =  %p \n ", classLoader);
    jmethodID getSystemCL = (*env)->GetStaticMethodID(env, classLoader, "getSystemClassLoader", "()Ljava/lang/ClassLoader;");
    jobject systemCL = (*env)->CallStaticObjectMethod(env, classLoader, getSystemCL);
    LOGI("systemclassloader = %p \n ", systemCL);
    return systemCL;

}

jclass loadClassFromClassLoader(JNIEnv* env, jobject classLoader, char* targetName)
{
    LOGI(" %s, targetname = %s \n ",__PRETTY_FUNCTION__, targetName);
    jclass classLoader_cls = (*env)->FindClass(env,"java/lang/ClassLoader");
    jmethodID loadClass = (*env)->GetMethodID(env, classLoader_cls, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
    LOGI("loadClass mid = %p \n", loadClass);
    //jstring name = (*env)->NewStringUTF(env,"test/sid/org/ndksample/HookCls");   
    jstring name = (*env)->NewStringUTF(env,targetName);   
    jclass loaded = (*env)->CallObjectMethod(env, classLoader, loadClass, name);
    LOGI("loaded class = %p \n" , loaded);
    return (jclass) (*env)->NewGlobalRef(env, loaded);
}

jclass findClassFromClassLoader(JNIEnv* env, jobject classLoader, char* targetName)
{
    jclass classLoader_cls = (*env)->FindClass(env,"java/lang/ClassLoader");
    jmethodID findClass = (*env)->GetMethodID(env, classLoader_cls, "findClass", "(Ljava/lang/String;)Ljava/lang/Class;");
    LOGI("findClass mid = %p \n", findClass);
    jstring name = (*env)->NewStringUTF(env,targetName);   
    jclass res = (*env)->CallObjectMethod(env,classLoader,findClass,name);
    LOGI("HookClass =  %p\n" , res);
    return (jclass) (*env)->NewGlobalRef(env, res) ; 
}

