#ifndef IME_DIALOG_H
#define IME_DIALOG_H

#include <stdint.h>

#define IME_DIALOG_RESULT_NONE 0
#define IME_DIALOG_RESULT_RUNNING 1
#define IME_DIALOG_RESULT_FINISHED 2
#define IME_DIALOG_RESULT_CANCELED 3

#ifdef __cplusplus
extern "C" {
#endif

int initImeDialog(char *title, char *initial_text, int max_text_length);
uint16_t *getImeDialogInputTextUTF16();
uint8_t *getImeDialogInputTextUTF8();
int isImeDialogRunning();
int updateImeDialog();

#ifdef __cplusplus
}
#endif

#endif /* IME_DIALOG_H */

