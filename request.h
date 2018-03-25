#define MAX_KEYWORD_LEN 100

typedef struct request_t {
    uint32_t index; 
    char keyword[MAX_KEYWORD_LEN];
} request_t;

void set_request(request_t *request, uint32_t index, char *keyword) {
    int i = 0 ;
    while (i < MAX_KEYWORD_LEN - 1 && keyword[i]) {
        request->keyword[i] = keyword[i];
        i++;
    }
    request->keyword[i] = '\0';
    request->index = index; 
}
