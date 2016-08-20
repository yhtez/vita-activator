#include "http.h"

#include <stdio.h>
#include <malloc.h>
#include <string.h>
    
#include <psp2/sysmodule.h>

#include <psp2/net/net.h>
#include <psp2/net/netctl.h>
#include <psp2/net/http.h>
#include <psp2/libssl.h>

void netInit() {
    sceSysmoduleLoadModule(SCE_SYSMODULE_NET);
    
    SceNetInitParam param;
    int size = 1024 * 1024;
    
    param.memory = malloc(size);
    param.size = size;
    param.flags = 0;
    
    sceNetInit(&param);
    sceNetCtlInit();
}

void netTerm() {
    sceNetCtlTerm();
    sceNetTerm();
    
    sceSysmoduleUnloadModule(SCE_SYSMODULE_NET);
}

void httpInit() {
    sceSysmoduleLoadModule(SCE_SYSMODULE_HTTP);
    sceSysmoduleLoadModule(SCE_SYSMODULE_HTTPS);
    sceSysmoduleLoadModule(SCE_SYSMODULE_SSL);
    
    sceHttpInit(1024 * 1024);
    sceSslInit(1024 * 1024);
}

void httpTerm() {
    sceSslTerm();
    sceHttpTerm();
    
    sceSysmoduleUnloadModule(SCE_SYSMODULE_SSL);
    sceSysmoduleUnloadModule(SCE_SYSMODULE_HTTPS);
    sceSysmoduleUnloadModule(SCE_SYSMODULE_HTTP);
}