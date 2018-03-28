#include <string.h>
#define MAX_KEYWORD_LEN 100

typedef struct request_t {
    uint32_t index; 
    char keyword[MAX_KEYWORD_LEN];
} request_t;

void set_request(request_t *request, uint32_t index, char *keyword) {
    strcpy(request->keyword, keyword);
    request->index = index; 
}
