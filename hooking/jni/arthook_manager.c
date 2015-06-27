#include <sys/types.h>
#include <pthread.h>

#include "arthook_manager.h"


static struct arthook_t *h = NULL;

pthread_rwlock_t lock;

int init()
{
    if (pthread_rwlock_init(&lock,NULL) != 0) return 1;
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
jstring hooks_manager_OriginalCall
  (JNIEnv *env, jstring hook_key)
{
    char *key = getCharFromJstring(env, hook_key);
    LOGI("dentro hooks manager, cerco : %s \n", key);
    arthook_t* myhook = find_key(key);
    if(myhook == NULL)
      return NULL;
    return (jstring) call_original_method(env, myhook);    
}

