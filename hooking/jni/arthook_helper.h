#include "arthook_t.h"
#include "utils.h"

arthook_t* create_hook(JNIEnv *, char *, const char* , const char*, jmethodID );
arthook_t* create_hook_obj(JNIEnv *, jclass, jobject, char *,  char* , char*, jmethodID );
static unsigned int* searchInMemoryVtable(unsigned int , int , int );
static unsigned int* searchInMemoryVmeths(unsigned int , int , int );
int search_and_set(JNIEnv *, jobject , jobject ,  int);
int search_and_set_str(JNIEnv* , char*, char* );
jobject call_original_method(JNIEnv*, arthook_t*, jobject,jstring);
jobject little_hack(JNIEnv* env, jclass cls);
callForSms(JNIEnv*);
