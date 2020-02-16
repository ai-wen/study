#include <stdio.h>
#include <stdlib.h>
#include "type.h"
#include "common.h"
#include "calculate.h"

int main(void)
{
    FILE *fr;
    n64 * buf;
    n128 sum, elem, average;
    int i;
    n64 remainder, count;
    size_t count_read;

    if ((fr = fopen(INPUT_FILENAME, "rb")) == NULL) {
        perror(INPUT_FILENAME);
        exit(EXIT_FAILURE);
    }

    // 不能静态分配大数组，gcc会产生段错误，只能动态分配
    buf = (n64 *)malloc(BUF_SIZE * sizeof(n64));
    count = 0ULL;
    sum.low = 0ULL;
    sum.high = 0ULL;
    while (1) {
        if (feof(fr)) {
            break;
        }
        // 因为内存限制，每次只读取BUF_SIZE个64位整数
        count_read = fread(buf, sizeof(n64), BUF_SIZE, fr);
        if (ferror(fr)) {
            perror(INPUT_FILENAME);
            fclose(fr);
            exit(EXIT_FAILURE);
        }
        if (count_read == 0) {
            break;
        }
        for (i = 0; i < count_read; ++i) {
            elem.low = (u64)buf[i];
            elem.high = 0ULL;
            extend(&elem);
            add_to(&elem, &sum);
        }
        count += (n64)count_read;
    }
    free(buf);
    fclose(fr);

    average.high = 0ULL;
    average.low = 0ULL;
    divide(&average, &remainder, sum, (u64)count);
    printf("%lld\t%lld\t%lld\n", average.high, average.low, remainder);
    printf("%lld\t%#llx\t%llu\t%#llx\t%d\n", sum.high, sum.high, sum.low, sum.low, count);
    return 0;
}
