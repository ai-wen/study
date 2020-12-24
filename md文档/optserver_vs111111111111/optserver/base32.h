#include <sys/types.h>

void base32_encode(const unsigned char *plain, size_t len, unsigned char *coded);
size_t base32_decode(const unsigned char *coded, unsigned char *plain);
