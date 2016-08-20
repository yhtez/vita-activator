#ifndef VITA_ACTIVATE_H
#define VITA_ACTIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

int vita_activate(const char* email, const char* password, const char* idps, const char* file);
    
#ifdef __cplusplus
}
#endif

#endif /* VITA_ACTIVATE_H */

