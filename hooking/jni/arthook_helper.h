#include "arthook_t.h"
#include "utils.h"

arthook_t* create_hook(JNIEnv *, char *, const char* , const char*, jmethodID );
static unsigned int* searchInMemory(unsigned int , int , int );
int search_and_set(JNIEnv *, jobject , jobject ,  int);
int search_and_set_str(JNIEnv* , char*, char* );
void* call_original_method(JNIEnv*, arthook_t*);
