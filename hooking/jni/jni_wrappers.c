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
	log("chiamato getcreatedjavavms, valore: %p , count = %d \n", vms[0], vm_count);
    JNIEnv* env = getEnv(vms);
	log("jnienv = 0x%08x \n", (unsigned int) env);
    return env;
}


jobject createDexClassLoader(JNIEnv* env, jobject classLoader, char* mydexpath, char* myoptdir)
{
    jthrowable exc;
    LOGI("dentro create dex class loader!!! pid: %d\n", getpid());
    jclass dexclassloader_cls = (*env)->FindClass(env, "dalvik/system/DexClassLoader");
    LOGI("trovata classe dexclassloader %x \n", dexclassloader_cls);
    jmethodID constructor = (*env)->GetMethodID(env, dexclassloader_cls, "<init>", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/ClassLoader;)V");
    LOGI("trovato methodid: %x \n", constructor);
    char* dexPath = mydexpath;
    // il proprietario di questa dir deve essere l'app
    char* optDir = myoptdir;
    jstring dexPathJ = (*env)->NewStringUTF(env, dexPath);
    jstring optDirJ = (*env)->NewStringUTF(env, optDir);
    char* libraryPath = "";
    LOGI("sono %d, creo il dexclassloader \n", getpid());
    jobject dexloader = (*env)->NewObject(env, dexclassloader_cls, constructor, dexPathJ, optDirJ, NULL, classLoader);
    exc = (*env)->ExceptionOccurred(env);
    if(exc){
        (*env)->ExceptionDescribe(env);
        (*env)->ExceptionClear(env);
    }
    LOGI("create dex loader, risultato: %x \n ", dexloader);
    return dexloader;
}

jobject getSystemClassLoader(JNIEnv* env)
{
    jclass classLoader = (*env)->FindClass(env, "java/lang/ClassLoader");
    LOGI("trovato classloader class: %x \n ", classLoader);
    jmethodID getSystemCL = (*env)->GetStaticMethodID(env, classLoader, "getSystemClassLoader", "()Ljava/lang/ClassLoader;");
    jobject systemCL = (*env)->CallStaticObjectMethod(env, classLoader, getSystemCL);
    LOGI("trovato systemclassloader: %x \n ", systemCL);
    return systemCL;

}

jclass loadClassFromClassLoader(JNIEnv* env, jobject classLoader, char* targetName)
{
    LOGI("dentro loadclass from classloader, targetname = %s \n ", targetName);
    jclass classLoader_cls = (*env)->FindClass(env,"java/lang/ClassLoader");
    jmethodID loadClass = (*env)->GetMethodID(env, classLoader_cls, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
    LOGI("trovato meth loadClass: %x \n", loadClass);
    //jstring name = (*env)->NewStringUTF(env,"test/sid/org/ndksample/HookCls");   
    jstring name = (*env)->NewStringUTF(env,targetName);   
    jclass loaded = (*env)->CallObjectMethod(env, classLoader, loadClass, name);
    LOGI("caricata class: %x \n" , loaded);
    return (jclass) (*env)->NewGlobalRef(env, loaded);
}

jclass findClassFromClassLoader(JNIEnv* env, jobject classLoader, char* targetName)
{
    jclass classLoader_cls = (*env)->FindClass(env,"java/lang/ClassLoader");
    jmethodID findClass = (*env)->GetMethodID(env, classLoader_cls, "findClass", "(Ljava/lang/String;)Ljava/lang/Class;");
    LOGI("trovato meth findClass: %x \n", findClass);
    jstring name = (*env)->NewStringUTF(env,targetName);   
    jclass res = (*env)->CallObjectMethod(env,classLoader,findClass,name);
    LOGI("trovata Hookcls class: %x \n" , res);
    return (jclass) (*env)->NewGlobalRef(env, res) ; 
}


