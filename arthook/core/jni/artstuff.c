#include "artstuff.h"

/* wrapper per dlsym() */
static void* mydlsym(void *hand, const char *name)
{
	void* ret = dlsym(hand, name);
	arthooklog("%s = %p\n", name, ret);
	return ret;
}

void resolve_symbols(struct artstuff_t *d){
	d->art_hand = dlopen("libart.so", RTLD_NOW);
	arthooklog("art_hand = 0x%08x \n", (unsigned int) d->art_hand);
	if(d->art_hand){
		d->JNI_GetCreatedJavaVMs_fnPtr = mydlsym(d->art_hand, "JNI_GetCreatedJavaVMs");
        d->art_th_currentFromGdb_fnPtr = mydlsym(d->art_hand, "_ZN3art6Thread14CurrentFromGdbEv");
        d->art_dbg_SuspendVM_fnPtr = mydlsym(d->art_hand, "_ZN3art3Dbg9SuspendVMEv");
        d->art_dbg_ResumeVM_fnPtr = mydlsym(d->art_hand, "_ZN3art3Dbg8ResumeVMEv");
        d->art_dbg_SuspendSelf_fnPtr = mydlsym(d->art_hand, "_ZN3art3Dbg11SuspendSelfEv");
        d->art_thlist_resumeAll_fnPtr = mydlsym(d->art_hand, "_ZN3art10ThreadList9ResumeAllEv");
	}
    
}

void _GetCreatedJavaVMs(struct artstuff_t* d, void** vms, jsize size, jsize* vm_count){
	arthooklog("dentro getcreatedjavavms \n");
	jint res = 3;
	arthooklog("chiamo: 0x%08x \n ", (unsigned int) d->JNI_GetCreatedJavaVMs_fnPtr);
	res = d->JNI_GetCreatedJavaVMs_fnPtr(vms, size, vm_count);
	arthooklog("res = %d\n", res);
	if(res != JNI_OK){
		arthooklog("error!!!!\n");
	}
}

void _suspendAllForDbg(struct artstuff_t *d)
{
    arthooklog("suspend vm: %x \n", (unsigned int) d->art_dbg_SuspendVM_fnPtr );
    //d->art_thlist_suspendAllForDbg_fnPtr();
    //d->art_th_DumpFromDbg_fnPtr();
    //d->art_dbg_SuspendVM_fnPtr();
    //d->art_dbg_SuspendSelf_fnPtr();
    //d->art_thlist_resumeAll_fnPtr();
}


