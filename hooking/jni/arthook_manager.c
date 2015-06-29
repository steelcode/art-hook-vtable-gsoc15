#include "arthook_manager.h"


static struct arthook_t *h = NULL;

pthread_rwlock_t lock;

int arthook_manager_init(JNIEnv* env)
{
    if (pthread_rwlock_init(&lock,NULL) != 0) return 1;
    hook_demo_init(env);
}

int add_hook(arthook_t* new)
{
    if (pthread_rwlock_wrlock(&lock) != 0) return 1;
    HASH_ADD_STR(h, key, new);
    pthread_rwlock_unlock(&lock);
}
arthook_t* find_key(char* key)
{
    arthook_t* myhook = NULL;
    if (pthread_rwlock_rdlock(&lock) != 0) return NULL;
    HASH_FIND_STR(h, key, myhook);
    pthread_rwlock_unlock(&lock);
    return myhook;

}
jobject hooks_manager_OriginalCall
  (JNIEnv *env, jstring hook_key, jobject o, jstring arg1)
{
    char *key = getCharFromJstring(env, hook_key);
    LOGI("dentro hooks manager, cerco : %s \n", key);
    arthook_t* myhook = find_key(key);
    if(myhook == NULL)
        return NULL;
    return  call_original_method(env, myhook, o, arg1);    
}

