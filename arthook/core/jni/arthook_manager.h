#ifndef ARTHOOK_MANAGER_H
#define ARTHOOK_MANAGER_H

#include <sys/types.h>
#include <pthread.h>
#include "utils.h"
#include "hook_demo.h"
#include "globals.h"


//int arthook_manager_init(JNIEnv*);


// arthook_manager
extern int add_hook(arthook_t*);

extern arthook_t* get_hook_by_key(char* key);

extern int is_method_in_hashtable();

extern void* get_method_from_hashtable(unsigned int* target);

extern void print_hashtable();
#endif

