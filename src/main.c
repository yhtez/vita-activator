#include "debugscreen/graphics.h"
#include "vita-activate/vita-activate.h"

#include <stdio.h>
#include <string.h>

#include <psp2/ctrl.h>

#include <psp2/io/fcntl.h>
#include <psp2/kernel/processmgr.h>

#define printf psvDebugScreenPrintf
#define setfg psvDebugScreenSetFgColor

int _vshSblAimgrGetConsoleId(char CID[32]);
int sceRegMgrGetKeyStr(const char* reg, const char* key, char* str, const int buffer_size);

int get_key(void) {
	static unsigned buttons[] = {
		SCE_CTRL_SELECT,
		SCE_CTRL_START,
		SCE_CTRL_UP,
		SCE_CTRL_RIGHT,
		SCE_CTRL_DOWN,
		SCE_CTRL_LEFT,
		SCE_CTRL_LTRIGGER,
		SCE_CTRL_RTRIGGER,
		SCE_CTRL_TRIANGLE,
		SCE_CTRL_CIRCLE,
		SCE_CTRL_CROSS,
		SCE_CTRL_SQUARE,
	};

	static unsigned prev = 0;
	SceCtrlData pad;
	while (1) {
		memset(&pad, 0, sizeof(pad));
		sceCtrlPeekBufferPositive(0, &pad, 1);
		unsigned new = prev ^ (pad.buttons & prev);
		prev = pad.buttons;
		for (int i = 0; i < sizeof(buttons)/sizeof(*buttons); ++i)
			if (new & buttons[i])
				return buttons[i];

		sceKernelDelayThread(1000); // 1ms
	}
}

int main(int argc, char** argv) {
    char email[6 * 16];
    char password[2 * 16];
    char CID[2 * 16];
    char idps[(2 * 16) + 1];
    int i;
    psvDebugScreenInit();
    
    printf("Reading account info and device IDPS\n\n");
    
    sceRegMgrGetKeyStr("/CONFIG/NP", "login_id", &email, 6 * 16);
    sceRegMgrGetKeyStr("/CONFIG/NP", "password", &password, 2 * 16);

    _vshSblAimgrGetConsoleId(CID);
    for (i = 0; i < 16; i++) {
        snprintf(idps + (i * 2), (2 * 16) - (i * 2) + 1, "%02X", CID[i]);
    }
    
    printf("Downloading activation data using registered PSN account\n\n");
    
    if (vita_activate(email, password, idps, "ux0:temp/act.dat") > 0) {
        setfg(COLOR_GREEN);
        printf("Activation data downloaded successfully\n\n");
        setfg(COLOR_WHITE);
        printf("To replace system act.dat press ");
        setfg(COLOR_YELLOW);
        printf("CROSS\n");
        setfg(COLOR_WHITE);
        printf("or\n");
        printf("To keep current system act.dat press ");
        setfg(COLOR_YELLOW);
        printf("CIRCLE\n\n");
        setfg(COLOR_WHITE);
        
        int key = get_key();
        while (key != SCE_CTRL_CROSS && key != SCE_CTRL_CIRCLE) {
            key = get_key();
        }
        
        if (key == SCE_CTRL_CROSS) {
            setfg(COLOR_CYAN);
            printf("Replacing system act.dat\n\n");
            setfg(COLOR_WHITE);
            SceUID temp_act = sceIoOpen("ux0:temp/act.dat", SCE_O_RDONLY, 0777);
            SceUID sys_act  = sceIoOpen("tm0:npdrm/act.dat", SCE_O_WRONLY | SCE_O_CREAT, 0777);
            
            unsigned char buffer[128 * 128];
            int read = 0;
            
            while ((read = sceIoRead(temp_act, buffer, sizeof(buffer))) > 0) {
                sceIoWrite(sys_act, buffer, read);
            }
            
            sceIoClose(temp_act);
            sceIoClose(sys_act);
        } else {
            setfg(COLOR_CYAN);
            printf("System act.dat left untouched\n");
            printf("Downloaded act.dat is in ux0:temp\n\n");
            setfg(COLOR_WHITE);
        }
        
    } else {
        setfg(COLOR_RED);
        printf("Activation data failed to download\n\n");
        setfg(COLOR_WHITE);
        printf("Please check these details are correct\n");
        printf("EMAIL:    ");
        setfg(COLOR_YELLOW);
        printf("%s", email);
        setfg(COLOR_WHITE);
        printf("PASSWORD: ");
        setfg(COLOR_YELLOW);
        printf("%s", password);
        setfg(COLOR_WHITE);
        printf("IDPS:     ");
        setfg(COLOR_YELLOW);
        printf("%s", idps);
        setfg(COLOR_WHITE);
    }

    printf("Press any key to exit\n");
    
    get_key();
    sceKernelExitProcess(0);
    return 0;
}