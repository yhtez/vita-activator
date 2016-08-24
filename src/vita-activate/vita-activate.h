#ifndef VITA_ACTIVATE_H
#define VITA_ACTIVATE_H

#ifdef __cplusplus
extern "C" {
#endif
    
/**
 * Writes activation data to provided file path
 * @param[in] email
 * @param[in] password
 * @param[in] idps
 * @param[in] file
 * @return 1 if activatio
 */
int vita_activate(const char* email, const char* password, const char* idps, const char* file);

/**
 * Get the controller state information (polling, positive logic).
 *
 * @param[in] port - use 0.
 * @param[out] *pad_data - see ::SceCtrlData.
 * @param[in] count - Buffers count.
 *
 * @return Buffers count, between 1 and 'count'. <0 on error.
 */
int vita_deactivate(const char* email, const char* password, const char* idps, const char* file);

#ifdef __cplusplus
}
#endif

#endif /* VITA_ACTIVATE_H */

