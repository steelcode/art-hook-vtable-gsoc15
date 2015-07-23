#include "arthook_t.h"
#include "utils.h"
#include "globals.h"


#define CLAZZ_OFF 0x8
//#define ACCESS_FLAG_OFF 28
#define ITABLE_OFF 0x18 //24
#define VMETHODS_OFF 0x30
#define VMETHS_LEN_OFF 0x8

#define VTABLE_OFF 0x34
#define ACCESS_FLAG_OFF 28
#define CSIZE_OFF 0x3c


//LOLLIPOP
#define LOLLIPOP_VTABLE_OFF 0x2c // 44
#define LOLLIPOP_VMETHODS_OFF 0x2c // 44
#define LOLLIPOP_CLAZZ_OFF 0x8 // 16



arthook_t* create_hook(JNIEnv *, char *, const char* , const char*, jmethodID);
arthook_t* create_hook_obj(JNIEnv *, jclass, jobject, char *,  char* , char*, jmethodID );
static unsigned int* searchInMemoryVtable(unsigned int , int , int );
static unsigned int* searchInMemoryVmeths(unsigned int , int , int );
int search_and_set(JNIEnv *, jobject , jobject ,  int);
int search_and_set_str(JNIEnv* , char*, char* );
jobject call_original_method(JNIEnv*, arthook_t*, jobject, jstring);
void* hh_check_javareflection_call(JNIEnv *, jobject, jobject );
