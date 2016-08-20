#include "http.h"
#include "debugscreen/graphics.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <psp2/display.h>
#include <psp2/net/http.h>
#include <psp2/io/fcntl.h>
#include <psp2/kernel/processmgr.h>

#define act_dat "ux0:act.dat" //"tm0:npdrm/act.dat"
#define printf psvDebugScreenPrintf

int _vshSblAimgrGetConsoleId(char CID[32]);
int sceRegMgrGetKeyStr(const char* reg, const char* key, char* str, const int buffer_size);

int urlencode(char *dest, const char *src) {
    for (int i = 0; src[i]; i++) {
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

int activate(const char* email, const char* password, const char* idps) {
    int tmpl, conn, req, ret;
    char email_encoded[8 * 16];
    urlencode(email_encoded, email);
    
    char data[53+strlen(email_encoded) + strlen(password) + strlen(idps)];
    snprintf(data, 54+strlen(email_encoded) + strlen(password) + strlen(idps), "loginid=%s&password=%s&consoleid=%s&platform=psp2&acttype=4", email_encoded, password, idps);

    sceHttpsDisableOption(SCE_HTTPS_ERROR_SSL_INVALID_CERT | SCE_HTTPS_ERROR_SSL_UNKNOWN_CA);
    tmpl    = sceHttpCreateTemplate("Glory to arstotzka", 1, 1);
    conn    = sceHttpCreateConnectionWithURL(tmpl, "https://commerce.np.ac.playstation.net/cap.m", 1);
    req     = sceHttpCreateRequestWithURL(conn, SCE_HTTP_METHOD_POST, "https://commerce.np.ac.playstation.net/cap.m", sizeof(data));
    ret     = sceHttpAddRequestHeader(req, "X-I-5-DRM-Version", "1.0", SCE_HTTP_HEADER_OVERWRITE);
    ret     = sceHttpAddRequestHeader(req, "Content-Type", "application/x-www-form-urlencoded", SCE_HTTP_HEADER_OVERWRITE);
    ret     = sceHttpSendRequest(req, data, sizeof(data));

    SceUID file = sceIoOpen(act_dat, SCE_O_WRONLY | SCE_O_CREAT, 0777);

    unsigned char buffer[128 * 128];
    int read = 0;
    int succ = 0;

    while ((read = sceHttpReadData(req, &buffer, sizeof(buffer))) > 0) {
        sceIoWrite(file, buffer, read);
        succ = 1;
    }

    sceHttpsEnableOption(SCE_HTTPS_ERROR_SSL_INVALID_CERT | SCE_HTTPS_ERROR_SSL_UNKNOWN_CA);

    return succ;
}

int main(int argc, char** argv) {
    char email[6 * 16];
    char password[2 * 16];
    char CID[2 * 16];
    char idps[(2 * 16) + 1];
    psvDebugScreenInit();

    netInit();
    httpInit();

    printf("Reading user info and console id\n");
    
    sceRegMgrGetKeyStr("/CONFIG/NP", "login_id", &email, 6 * 16);
    sceRegMgrGetKeyStr("/CONFIG/NP", "password", &password, 2 * 16);

    _vshSblAimgrGetConsoleId(CID);
    for (int i = 0; i < 16; i++) {
        snprintf(idps + (i * 2), (2 * 16) - (i * 2) + 1, "%02X", CID[i]);
    }
    
    printf("Attempting to download activation file...\n\n");
    
    if (activate(email, password, idps)) {
        printf("Activation data written to ux0:act.dat\nCheck that all is ok and copy to tm0:npdrm/act.dat\n\n");
    } else {
        printf("Activation failed, please try again\n\n");
    }

    httpTerm();
    netTerm();

    printf("App will close in 5 seconds...\n");
    
    sceKernelDelayThread(1000 * 1000 * 10);
    sceKernelExitProcess(0);
    return 0;
}