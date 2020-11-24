
#include "skbuff.h"


#define MAX_PKBS 200
static int free_pkbs = 0;
static int alloc_pkbs = 0;

#define pkb_safe() \
do {\
    if ((alloc_pkbs - free_pkbs) > MAX_PKBS) {\
        DEBUG_PRINT("oops: too many pkbuf\r\n");\
        exit(EXIT_FAILURE);\
    }\
} while (0)

/* referred from linux-2.6: handing packet l2 padding */
void pkb_trim(struct pkbuf *pkb, int len)
{
    pkb->pk_len = len;
    if (realloc(pkb, sizeof(*pkb) + len) == NULL) {
        ERROR_PRINT("realloc");
    }
}

struct pkbuf *alloc_pkb(int size)
{
    struct pkbuf *pkb = (struct pkbuf *)malloc(sizeof(*pkb) + size);
    if (pkb) {
        pkb->pk_len = size;
        pkb->pk_pro = 0xffff;
        pkb->pk_type = 0;
        pkb->pk_refcnt = 1;
        pkb->pk_indev = NULL;
        pkb->pk_rtdst = NULL;
        list_init(&(pkb->pk_list));
        alloc_pkbs++;
        pkb_safe();
    }
    return pkb;
}

struct pkbuf *copy_pkb(struct pkbuf *pkb)
{
    struct pkbuf *cpkb = (struct pkbuf *)malloc(pkb->pk_len);
    if (cpkb) {
        memcpy(cpkb, pkb, pkb->pk_len);
        cpkb->pk_refcnt = 1;
        list_init(&cpkb->pk_list);
        alloc_pkbs++;
        pkb_safe();
    }
    return cpkb;
}

void free_pkb(struct pkbuf *pkb)
{
    if (--pkb->pk_refcnt == 0) {
        free_pkbs++;
        free(pkb);
    }
    if (pkb->pk_refcnt < 0) {
        DEBUG_PRINT("you free too many times!\r\n");
        pkb->pk_refcnt = 0;
    }
}

void get_pkb(struct pkbuf *pkb)
{
    pkb->pk_refcnt++;
}

void pkbdbg(struct pkbuf *pkb)
{
    int i;
    DEBUG_PRINT("packet size: %d bytes\n", pkb->pk_len);
    DEBUG_PRINT("packet buffer(ascii):\n");
    for (i = 0; i < pkb->pk_len; i++) {
        if ((i % 16) == 0)
            DEBUG_PRINT("%08x: ", i);
        if (isprint(pkb->pk_data[i])) {
            DEBUG_PRINT("%c", pkb->pk_data[i]);
        } else {
            DEBUG_PRINT(".");
        }
        if ((i % 16) == 15)
            DEBUG_PRINT("\n");
    }
    if ((i % 16) != 0)
        DEBUG_PRINT("\n");
    DEBUG_PRINT("packet buffer(raw):\n");
    for (i = 0; i < pkb->pk_len; i++) {
        if ((i % 16) == 0)
            DEBUG_PRINT("%08x: ", i);
        if ((i % 2) == 0)
            DEBUG_PRINT(" ");
        DEBUG_PRINT("%02x", pkb->pk_data[i]);
        if ((i % 16) == 15)
            DEBUG_PRINT("\n");
    }
    if ((i % 16) != 0)
        DEBUG_PRINT("\n");
}


