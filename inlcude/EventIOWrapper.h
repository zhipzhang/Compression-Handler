#ifdef __cplusplus
extern "C" {
#include "stddef.h"
#endif
void* EventIOHandler_create(const char* fname, const char mode);
int userfunction1(void*, unsigned char* buffer, long size);
int userfunction2(void*, unsigned char* buffer, long size);
int userfunction3(void*, unsigned char* buffer, long size);
int userfunction4(void*, unsigned char* buffer, long size);
void EventIOHandler_delete(void*);
#ifdef __cplusplus
}
#endif
