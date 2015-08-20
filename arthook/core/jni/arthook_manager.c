#include "arthook_manager.h"

static struct arthook_t *h = NULL;

pthread_rwlock_t lock;


int arthook_manager_init(JNIEnv* env)
{
    if (pthread_rwlock_init(&lock,NULL) != 0) return 1;
    
}

int add_hook(arthook_t* new)
{
    if (pthread_rwlock_wrlock(&lock) != 0) return 1;
    HASH_ADD_STR(h, key, new);
    pthread_rwlock_unlock(&lock);
}
arthook_t* get_hook_by_key(char* key)
{
    arthook_t* myhook = NULL;
    if (pthread_rwlock_rdlock(&lock) != 0) return NULL;
    HASH_FIND_STR(h, key, myhook);
    pthread_rwlock_unlock(&lock);
    return myhook;
}

void print_hashtable(){
    arthook_t* tmp = NULL;
    for(tmp = h; tmp != NULL; tmp = tmp->hh.next){
        LOGI("item: %s \n", tmp->clsname);
        LOGI("original mid : %p \n",  tmp->original_meth_ID);
    }    
}

void* get_method_from_hashtable(unsigned int* target)
{
    arthook_t* tmp = NULL;
    int res = -1;
    for(tmp = h; tmp != NULL; tmp = tmp->hh.next){
        LOGI("item : %s \n", tmp->clsname);
        LOGI("original  mid: %p \n", tmp->original_meth_ID);
        LOGI("sto cercando : %p \n", *target);
        res = memcmp(&tmp->original_meth_ID, target,4);
        LOGI("memcmp res: %d \n", res);
        if(res == 0)
            return tmp;
    }
    return NULL;
}

int is_method_in_hashtable(unsigned int* target)
{
    arthook_t* tmp = NULL;

    for(tmp = h; tmp != NULL; tmp = tmp->hh.next){
        LOGI("item: %s \n", tmp->clsname);
        LOGI("original mid : %x \n", (unsigned int) tmp->original_meth_ID);
        LOGI("searching mid : %x \n", (unsigned int)  *target);
        int res = memcmp(&tmp->original_meth_ID, target,4);
        LOGI("memcpm res: %d \n", res);
        if(res == 0)
            return 1;
    }
    return 0;
}



