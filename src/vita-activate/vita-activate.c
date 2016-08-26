#include "vita-activate.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <psp2/sysmodule.h>

#include <psp2/io/fcntl.h>
#include <psp2/net/net.h>
#include <psp2/net/netctl.h>
#include <psp2/net/http.h>
#include <psp2/libssl.h>

void netInit() {
    sceSysmoduleLoadModule(SCE_SYSMODULE_NET);
    
    SceNetInitParam param;
    int size = 1 * 1024 * 1024;
    
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
    
    sceHttpInit(1 * 1024 * 1024);
    sceSslInit(1 * 1024 * 1024);
}

void httpTerm() {
    sceSslTerm();
    sceHttpTerm();
    
    sceSysmoduleUnloadModule(SCE_SYSMODULE_SSL);
    sceSysmoduleUnloadModule(SCE_SYSMODULE_HTTPS);
    sceSysmoduleUnloadModule(SCE_SYSMODULE_HTTP);
}

int urlencode(char *dest, const char *src) {
    int i;
    for (i = 0; src[i]; i++) {
        if (isalnum(src[i])) {
            *dest++ = src[i];
        } else {
            sprintf(dest, "%%%02X", src[i]);
            dest += 3;
        }
    }
    *dest++ = 0;
    return sizeof(dest);
}

int vita_activate(const char* email, const char* password, const char* idps, const char* file) {
    int tmpl, conn, req, ret;
    char email_encoded[8 * 16];
    unsigned int hdrs_len;
    char *hdrs;
    
    netInit();
    httpInit();
    
#ifdef DEBUG
    sceHttpsDisableOption(SCE_HTTPS_ERROR_SSL_INVALID_CERT | SCE_HTTPS_ERROR_SSL_UNKNOWN_CA);
#endif
    
    urlencode(email_encoded, email);
    
    char data[53+strlen(email_encoded) + strlen(password) + strlen(idps)];
    snprintf(data, 54+strlen(email_encoded) + strlen(password) + strlen(idps), "loginid=%s&password=%s&consoleid=%s&platform=psp2&acttype=4", email_encoded, password, idps);

    tmpl    = sceHttpCreateTemplate("", 2, 1);
    conn    = sceHttpCreateConnectionWithURL(tmpl, "https://commerce.np.ac.playstation.net/cap.m", 0);
    req     = sceHttpCreateRequestWithURL(conn, SCE_HTTP_METHOD_POST, "https://commerce.np.ac.playstation.net/cap.m", sizeof(data));
    ret     = sceHttpAddRequestHeader(req, "X-I-5-DRM-Version", "1.0", SCE_HTTP_HEADER_OVERWRITE);
    ret     = sceHttpAddRequestHeader(req, "Content-Type", "application/x-www-form-urlencoded", SCE_HTTP_HEADER_OVERWRITE);
    ret     = sceHttpSendRequest(req, data, sizeof(data));
    ret     = sceHttpGetAllResponseHeaders(req, &hdrs, &hdrs_len);
    
    int succ = 0;
    const char *hdr_val;
    unsigned int hdr_len;
    
    sceHttpParseResponseHeader(hdrs, hdrs_len, "X-I-5-DRM-STATUS", &hdr_val, &hdr_len);

    if (strncmp(hdr_val, "OK", 2) == 0) {
        succ = ACTIVATE_SUCCESS;
    } else if (strncmp(hdr_val, "NG; reason=21", 13) == 0) {
        succ = ACTIVATE_INVALID_PSN;
    } else if (strncmp(hdr_val, "NG; reason=24", 13) == 0) {
        succ = ACTIVATE_INVALID_IDPS;
    }
    
    if (succ == 0) {
        succ = ACTIVATE_UNKNOWN_ERROR;
    }
    
    SceUID file_io = sceIoOpen(file, SCE_O_WRONLY | SCE_O_CREAT, 0777);

    unsigned char buffer[128 * 128];
    int read = 0;

    while ((read = sceHttpReadData(req, &buffer, sizeof(buffer))) > 0) {
        sceIoWrite(file_io, buffer, read);
    }
    
    sceIoClose(file_io);
    
    httpTerm();
    netTerm();
    
    return succ;
}

int vita_deactivate(const char* email, const char* password, const char* idps) {
    int tmpl, conn, req, ret;
    char email_encoded[8 * 16];
    unsigned int hdrs_len;
    char *hdrs;
    
    netInit();
    httpInit();
    
#ifdef DEBUG
    sceHttpsDisableOption(SCE_HTTPS_ERROR_SSL_INVALID_CERT | SCE_HTTPS_ERROR_SSL_UNKNOWN_CA);
#endif
    
    urlencode(email_encoded, email);
    
    char data[53+strlen(email_encoded) + strlen(password) + strlen(idps)];
    snprintf(data, 54+strlen(email_encoded) + strlen(password) + strlen(idps), "loginid=%s&password=%s&consoleid=%s&platform=psp2&acttype=4", email_encoded, password, idps);

    tmpl    = sceHttpCreateTemplate("", 2, 1);
    conn    = sceHttpCreateConnectionWithURL(tmpl, "https://commerce.np.ac.playstation.net/cdp.m", 0);
    req     = sceHttpCreateRequestWithURL(conn, SCE_HTTP_METHOD_POST, "https://commerce.np.ac.playstation.net/cdp.m", sizeof(data));
    ret     = sceHttpAddRequestHeader(req, "X-I-5-DRM-Version", "1.0", SCE_HTTP_HEADER_OVERWRITE);
    ret     = sceHttpAddRequestHeader(req, "Content-Type", "application/x-www-form-urlencoded", SCE_HTTP_HEADER_OVERWRITE);
    ret     = sceHttpSendRequest(req, data, sizeof(data));
    ret     = sceHttpGetAllResponseHeaders(req, &hdrs, &hdrs_len);
    
    int succ = 0;
    const char *hdr_val;
    unsigned int hdr_len;
    
    sceHttpParseResponseHeader(hdrs, hdrs_len, "X-I-5-DRM-STATUS", &hdr_val, &hdr_len);
    
    if (strncmp(hdr_val, "OK", 2) == 0) {
        succ = ACTIVATE_SUCCESS;
    } else if (strncmp(hdr_val, "NG; reason=21", 13) == 0) {
        succ = ACTIVATE_INVALID_PSN;
    } else if (strncmp(hdr_val, "NG; reason=24", 13) == 0) {
        succ = ACTIVATE_INVALID_IDPS;
    }
    
    if (succ == 0) {
        succ = ACTIVATE_UNKNOWN_ERROR;
    }
    
    httpTerm();
    netTerm();
    
    return succ;
}