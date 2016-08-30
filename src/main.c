#include "main.h"
#include "vita-activate/vita-activate.h"
#include "ime/ime_dialog.h"

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include <psp2/types.h>
#include <psp2/apputil.h>
#include <psp2/ctrl.h>
#include <psp2/display.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/ime_dialog.h>
#include <psp2/io/fcntl.h>

#include <vita2d.h>

#define FONT_SIZE 1.0f
#define FONT_X_OFF 15.0f
#define FONT_Y_SPACE 23.0f * FONT_SIZE

#define TITLE "Vita Activator"
#define VER "1.1"

#define SCREEN_WIDTH 960
#define SCREEN_HEIGHT 544

int _vshSblAimgrGetConsoleId(char CID[32]);
int sceRegMgrGetKeyStr(const char* reg, const char* key, char* str, const int buffer_size);

int vita2d_printf(void* pgf, float line_number, unsigned int color, const char* format, ...) {
    char buf[512];

    va_list opt;
    va_start(opt, format);
    vsnprintf(buf, sizeof(buf), format, opt);
    va_end(opt);
    
    return vita2d_pgf_draw_text(pgf, FONT_X_OFF, line_number * FONT_Y_SPACE, color, FONT_SIZE, buf);
}

int vita2d_xprintf(void* pgf, float x_offset, float line_number, unsigned int color, const char* format, ...) {
    char buf[512];
    
    va_list opt;
    va_start(opt, format);
    vsnprintf(buf, sizeof(buf), format, opt);
    va_end(opt);
    
    return vita2d_pgf_draw_text(pgf, FONT_X_OFF + x_offset, line_number * FONT_Y_SPACE, color, FONT_SIZE, buf);
}

void getButtons(int *pressedButtons)
{
    static int current_buttons, old_buttons;
    SceCtrlData pad;

    memset(&pad, 0, sizeof(SceCtrlData));
    sceCtrlPeekBufferPositive(0, &pad, 1);

    old_buttons = current_buttons;
    current_buttons = pad.buttons;
    
    *pressedButtons = current_buttons & ~old_buttons;
}

int main(int argc, char** argv) {
    vita2d_pgf *pgf;
    char email[6 * 16] = {0};
    char password[2 * 16] = {0};
    char CID[2 * 16] = {0};
    char idps[2 * 16] = {0};
    char *title = "Welcome to Vita Activator";
    int width, buttons;
    int stage = STAGE_MAIN;
    int act_mode = 0;
    int ime = 0;
    int loop = 1;
    
    sceCtrlSetSamplingMode(SCE_CTRL_MODE_DIGITAL);
    sceAppUtilInit(&(SceAppUtilInitParam){}, &(SceAppUtilBootParam){});
    sceCommonDialogSetConfigParam(&(SceCommonDialogConfigParam){});
    
    vita2d_init();
    pgf = vita2d_load_default_pgf();
    
    while (loop == 1) {
        vita2d_start_drawing();
        vita2d_clear_screen();
        
        getButtons(&buttons);
        
        vita2d_printf(pgf, 1.0f, CYAN, "%s v%s", TITLE, VER);
        width = vita2d_printf(pgf, 2.0f, WHITE, "SSL Status: ");
#ifdef DEBUG
        vita2d_xprintf(pgf, width, 2.0f, RED, "DISABLED");
#else
        vita2d_xprintf(pgf, width, 2.0f, GREEN, "ENABLED");
#endif
        vita2d_printf(pgf, 4.0f, WHITE, title);
        
        switch (stage) {            
            case STAGE_MAIN:
                width = vita2d_printf(pgf, 6.0f, GREEN, "To activate PlayStation Vita press ");
                vita2d_xprintf(pgf, width, 6.0f, WHITE, "CROSS");
                width = vita2d_printf(pgf, 7.0f, RED, "To deactivate PlayStation Vita press ");
                vita2d_xprintf(pgf, width, 7.0f, WHITE, "CIRCLE");
//                width = vita2d_printf(pgf, 8.0f, BLUE, "To activate content on PlayStation Vita press ");
//                vita2d_xprintf(pgf, width, 8.0f, WHITE, "SQUARE ");
                
                if (buttons & SCE_CTRL_CROSS) {
                    stage = STAGE_ACC; //Get PSN Info
                    act_mode = 1; //PS Vita Activation
                    title = "PS Vita Activation";
                } else if (buttons & SCE_CTRL_CIRCLE) {
                    stage = STAGE_ACC; //Get PSN Info
                    act_mode = 2; //PS Vita Deactivation
                    title = "PS Vita Deactivation";
//                } else if (buttons & SCE_CTRL_SQUARE) {
//                    stage = STAGE_ACC; //Get PSN Info
//                    act_mode = 3; //PS Vita Content Activation
//                    title = "PS Vita Content Activation";
                }
                break;
                
            case STAGE_ACC: //Get PSN Info
                width = vita2d_printf(pgf, 6.0f, GREEN, "To read system PSN Account information press ");
                vita2d_xprintf(pgf, width, 6.0f, WHITE, "CROSS");
                width = vita2d_printf(pgf, 7.0f, RED, "To input PSN Account information press ");
                vita2d_xprintf(pgf, width, 7.0f, WHITE, "CIRCLE");
                width = vita2d_printf(pgf, 8.0f, BLUE, "To input a device setup key (idp.sn/device-password) press ");
                vita2d_xprintf(pgf, width, 8.0f, WHITE, "SQUARE");
                
                if (buttons & SCE_CTRL_CROSS)
                    stage = STAGE_SYSTEM; //Read System PSN Info
                else if (buttons & SCE_CTRL_CIRCLE)
                    stage = STAGE_MANUAL; //Input PSN info
                else if (buttons & SCE_CTRL_SQUARE)
                    stage = STAGE_TOKEN;
                break;
                
            case STAGE_SYSTEM: //Read System PSN Info
                sceRegMgrGetKeyStr("/CONFIG/NP", "login_id", email, 6 * 16);
                sceRegMgrGetKeyStr("/CONFIG/NP", "password", password, 2 * 16);
                
                stage = STAGE_CONFIRM;
                break;
            
            case STAGE_MANUAL:
                if (ime == 0) {
                    initImeDialog("PSN Account Email", "", 6 * 16, 0);
                    ime = 1;
                } else if (ime == 1) {
                    int update = updateImeDialog();
                    if (update == IME_DIALOG_RESULT_CANCELED || update == IME_DIALOG_RESULT_NONE) {
                        ime = 0;
                    } else if (update == IME_DIALOG_RESULT_FINISHED) {
                        strncpy(email, (char *)getImeDialogInputTextUTF8(), 6 * 16);
                        ime = 2;
                    }
                } else if (ime == 2) {              
                    initImeDialog("PSN Account Password", "", 6 * 16, 1);
                    ime = 3;
                } else if (ime == 3) {
                    int update = updateImeDialog();
                    if (update == IME_DIALOG_RESULT_CANCELED || update == IME_DIALOG_RESULT_NONE) {
                        ime = 2;
                    } else if (update == IME_DIALOG_RESULT_FINISHED) {
                        strncpy(password, (char *)getImeDialogInputTextUTF8(), 2 * 16);
                        ime = 4;
                    }
                } else {
                    stage = STAGE_CONFIRM;
                }
                break;
                
            case STAGE_TOKEN:
                sceRegMgrGetKeyStr("/CONFIG/NP", "login_id", email, 6 * 16);
                if (ime == 0) {
                    initImeDialog("Device Setup Key", "", 2 * 16, 0);
                    ime = 1;
                } else if (ime == 1) {
                    int update = updateImeDialog();
                    if (update == IME_DIALOG_RESULT_CANCELED || update == IME_DIALOG_RESULT_NONE) {
                        ime = 0;
                    } else if (update == IME_DIALOG_RESULT_FINISHED) {
                        strncpy(password, (char *)getImeDialogInputTextUTF8(), 2 * 16);
                        ime = 2;
                    }
                } else {
                    stage = STAGE_CONFIRM;
                }
                break;
                
            case STAGE_CONFIRM:
                _vshSblAimgrGetConsoleId(CID);
                for (int i = 0; i < 16; i++) {
                    snprintf(idps + (i * 2), (2 * 16) - (i * 2) + 1, "%02X", CID[i]);
                }
                
                width = vita2d_printf(pgf, 6.0f, WHITE, "To begin %s press ", title);
                vita2d_xprintf(pgf, width, 6.0f, CYAN, "CROSS");
                
                if (buttons & SCE_CTRL_CROSS) {
                    if (act_mode == 1) {
                        int res = vita_activate(email, password, idps, "ux0:temp/act.dat");
                        if (res == 1)
                            stage = STAGE_ACTCHOOSE;
                        else
                            stage = res;
                    } else if (act_mode == 2) {
                        int res = vita_deactivate(email, password, idps);
                        if (res == 1)
                            stage = STAGE_ACTCHOOSE;
                        else
                            stage = res;
//                    } else if (act_mode == 3) {
//                        //IMPLEMENT
                    }
                }
                break;
                
            case STAGE_ACTCHOOSE:
                if (act_mode == 1) {
                    vita2d_printf(pgf, 6.0f, GREEN, "PlayStation Vita activation data downloaded succesfully");
                    width = vita2d_printf(pgf, 8.0f, WHITE, "To replace system activation data press ");
                    vita2d_xprintf(pgf, width, 8.0f, CYAN, "CROSS");
                    width = vita2d_printf(pgf, 9.0f, WHITE, "To keep current system activation data press ");
                    vita2d_xprintf(pgf, width, 9.0f, CYAN, "CIRCLE");
                    
                    if (buttons & SCE_CTRL_CROSS) {
                        SceUID temp_act = sceIoOpen("ux0:temp/act.dat", SCE_O_RDONLY, 0777);
                        SceUID sys_act  = sceIoOpen("tm0:npdrm/act.dat", SCE_O_WRONLY | SCE_O_CREAT, 0777);
                        
                        unsigned char buffer[128 * 128];
                        int read = 0;

                        while ((read = sceIoRead(temp_act, buffer, sizeof(buffer))) > 0) {
                            sceIoWrite(sys_act, buffer, read);
                        }
                        stage = STAGE_DONE;
                    } else if (buttons & SCE_CTRL_CIRCLE) {
                        stage = STAGE_NONDONE;
                    }
                } else if (act_mode == 2) {
                    stage = STAGE_DONE;
//                } else if (act_mode == 3) {
//                    //IMPLEMENT
                }
                break;
                
            case STAGE_DONE:
                if (act_mode == 1) {
                    vita2d_printf(pgf, 6.0f, GREEN, "System Activation data has been replaced succesfully");
                    width = vita2d_printf(pgf, 7.0f, WHITE, "To exit Vita Activator press ");
                    vita2d_xprintf(pgf, width, 7.0f, CYAN, "CROSS");
                    if (buttons & SCE_CTRL_CROSS)
                        loop = 0;
                } else if (act_mode == 2) {
                    vita2d_printf(pgf, 6.0f, GREEN, "System has been deactivated succesfully");
                    width = vita2d_printf(pgf, 7.0f, WHITE, "To exit Vita Activator press ");
                    vita2d_xprintf(pgf, width, 7.0f, CYAN, "CROSS");
                    if (buttons & SCE_CTRL_CROSS)
                        loop = 0;
                }
                break;
                
            case STAGE_NONDONE:
                if (act_mode == 1) {
                    vita2d_printf(pgf, 6.0f, GREEN, "Activation data is stored in ux0:temp/act.dat");
                    width = vita2d_printf(pgf, 7.0f, WHITE, "To exit Vita Activator press ");
                    vita2d_xprintf(pgf, width, 7.0f, CYAN, "CROSS");
                    if (buttons & SCE_CTRL_CROSS)
                        loop = 0;
                }
                break;
                
            default: ;
                char error[8 * 16];
                if (stage == ACTIVATE_INVALID_PSN || stage == ACTIVATE_INVALID_IDPS || stage == ACTIVATE_UNKNOWN_ERROR) {
                    char err[4 * 16];
                    if (stage == ACTIVATE_INVALID_PSN)
                        strlcpy(err, "Invalid PSN Login or Device Setup Key", 4 * 16);
                    else if (stage == ACTIVATE_INVALID_IDPS)
                        strlcpy(err, "Invalid IDPS (you should be really scared)", 4 * 16);
                    else
                        strlcpy(err, "Unknown Error, contact @ZombieInHD on twitter for help", 4 * 16);
                    snprintf(error, 8 * 16, "%s failed, %s", title, err);
                } else
                    snprintf(error, 8 * 16, "%i contact @ZombieInHD on twitter for help", stage);
                vita2d_printf(pgf, 6.0f, WHITE, "An error has occurred, Printing debug information...");
                width = vita2d_printf(pgf, 7.0f, WHITE, "Error: ");
                vita2d_xprintf(pgf, width, 7.0f, CYAN, "%s", error);
                width = vita2d_printf(pgf, 8.0f, WHITE, "Email: ");
                vita2d_xprintf(pgf, width, 8.0f, email[0] == 0 ? RED : CYAN, "%s", email[0] == 0 ? "UNDEF" : email);
                width = vita2d_printf(pgf, 9.0f, WHITE, "Password: ");
                vita2d_xprintf(pgf, width, 9.0f, password[0] == 0 ? RED : CYAN, "%s", password[0] == 0 ? "UNDEF" : password);
                width = vita2d_printf(pgf, 10.0f, WHITE, "IDPS: ");
                vita2d_xprintf(pgf, width, 10.0f, idps[0] == 0 ? RED : CYAN, idps[0] == 0 ? "UNDEF" : idps);
                width = vita2d_printf(pgf, 11.0f, WHITE, "To exit Vita Activator press ");
                vita2d_xprintf(pgf, width, 11.0f, CYAN, "CROSS");
                if (buttons & SCE_CTRL_CROSS)
                    loop = 0;
        }
        
        vita2d_end_drawing();
        vita2d_common_dialog_update();
        vita2d_swap_buffers();
        sceDisplayWaitVblankStart();
    }
    
    vita2d_end_drawing();
    vita2d_common_dialog_update();
    vita2d_swap_buffers();
    vita2d_free_pgf(pgf);
    vita2d_fini();
    
    sceKernelExitProcess(0);
    return 0;
}