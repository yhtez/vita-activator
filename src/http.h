#ifndef HTTP_H
#define HTTP_H

#ifdef __cplusplus
extern "C" {
#endif
    
void netInit();
void netTerm();
    
void httpInit();
void httpTerm();

#ifdef __cplusplus
}
#endif

#endif /* HTTP_H */

