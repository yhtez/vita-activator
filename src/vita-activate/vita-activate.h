#ifndef VITA_ACTIVATE_H
#define VITA_ACTIVATE_H

#ifdef __cplusplus
extern "C" {
#endif
    
enum activateResponses {
    ACTIVATE_SUCCESS        = 1,
    ACTIVATE_INVALID_PSN    = -1,
    ACTIVATE_INVALID_IDPS   = -2,
    ACTIVATE_UNKNOWN_ERROR  = -3
};
    
/**
 * Writes activation data to provided file path
 * @param[in] email - PSN Account Email
 * @param[in] password - PSN Account Password
 * @param[in] idps - IDPS of device to activate
 * @param[in] file - Directory to write activation data to
 * @return Returns HTTP Status code
 */
int vita_activate(const char* email, const char* password, const char* idps, const char* file);

/**
 * Writes activation data to provided file path
 * @param[in] email - PSN Account Email
 * @param[in] password - PSN Account Password
 * @param[in] idps - IDPS of device to activate
 * @return Returns HTTP Status code
 */
int vita_deactivate(const char* email, const char* password, const char* idps);

#ifdef __cplusplus
}
#endif

#endif /* VITA_ACTIVATE_H */

