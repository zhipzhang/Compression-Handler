#include "EventIOWrapper.h"
#include "EventIOHandler.h"

void* EventIOHandler_create(const char* fname, const char mode) {
    EventIOHandler* eventiohandler = new EventIOHandler(fname, mode);
    return static_cast<void*>(eventiohandler);
}
void EventIOHandler_delete(void* eventiohandler) {
    delete static_cast<EventIOHandler*>(eventiohandler);
}

int userfunction1(void* eventiohandler, unsigned char* buffer, long size) {
    auto handler = static_cast<EventIOHandler*>(eventiohandler);
    return handler->user_function1(buffer, size);
}
int userfunction2(void* eventiohandler, unsigned char* buffer, long size) {
    auto handler = static_cast<EventIOHandler*>(eventiohandler);
    return handler->user_function2(buffer, size);
}
int userfunction3(void* eventiohandler, unsigned char* buffer, long size) {
    auto handler = static_cast<EventIOHandler*>(eventiohandler);
    return handler->user_function3(buffer, size);
}
int userfunction4(void* eventiohandler, unsigned char* buffer, long size) {
    auto handler = static_cast<EventIOHandler*>(eventiohandler);
    return handler->user_function4(buffer, size);
}
