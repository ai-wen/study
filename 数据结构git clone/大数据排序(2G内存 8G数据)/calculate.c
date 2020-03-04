#include <stdio.h>
#include <stdlib.h>
#include "type.h"
#include "calculate.h"

// 把64位有符号数扩展成128位有符号数
void extend(n128 *integer)
{
    u64 mask;

    // 常数默认为int类型，左移63结果是错的，必须加ULL
    mask = 1ULL << 63;
    if (integer->high == 0ULL && integer->low & mask) {
        integer->high = ~0ULL;
    }
}

// sum += elem
void add_to(n128 *elem, n128 *sum)
{
    sum->low += elem->low;
    if (sum->low < elem->low) {
        sum->high += 1ULL;
    }
    sum->high += elem->high;
}

// 把integer逻辑左移times位
void shl(u128 *integer, int times)
{
    int i, loop;
    u64 mask, mask_clr, tmp;

    times %= 128;
    if (times <= 63) {
        mask_clr = 1ULL << times;
        mask_clr -= 1ULL;
        mask = mask_clr << (64 - times);

        integer->high <<= times;
        tmp = integer->low & mask;
        tmp >>= 64 - times;
        // 不能确定是算术右移还是逻辑右移，必须把高位清零
        tmp &= mask_clr;
        integer->high |= tmp;

        integer->low <<= times;
    } else {
        loop = times / 63;
        for (i = 0; i < loop; ++i) {
            shl(integer, 63);
        }
        if (times % 63) {
            shl(integer, times % 63);
        }
    }
}

// 把integer逻辑右移times位
void shr(u128 *integer, int times)
{
    int i, loop;
    u64 mask, mask_clr, tmp;

    times %= 128;
    if (times <= 63) {
        mask = 1ULL << times;
        mask -= 1ULL;
        mask_clr = ~(mask << 64-times);

        integer->low >>= times;
        integer->low &= mask_clr;
        tmp = integer->high & mask;
        tmp <<= 64 - times;
        integer->low |= tmp;

        integer->high >>= times;
        integer->high &= mask_clr;
    } else {
        loop = times / 63;
        for (i = 0; i < loop; ++i) {
            shr(integer, 63);
        }
        if (times % 63) {
            shr(integer, times % 63);
        }
    }
}

// 把integer算术右移times位
void sar(u128 *integer, int times)
{
    int i, loop;
    u64 mask, mask_clr, tmp;

    times %= 128;
    if (times <= 63) {
        mask = 1ULL << times;
        mask -= 1ULL;
        mask_clr = ~(mask << 64-times);

        integer->low >>= times;
        integer->low &= mask_clr;
        tmp = integer->high & mask;
        tmp <<= 64 - times;
        integer->low |= tmp;

        // 分别处理符号位为0和1的两种情况
        if (integer->high & 1ULL<<63) {
            integer->high >>= times;
            integer->high &= mask_clr;
            tmp = mask;
            tmp <<= 64 - times;
            integer->high |= tmp;
        } else {
            integer->high >>= times;
            integer->high &= mask_clr;
        }
    } else {
        loop = times / 63;
        for (i = 0; i < loop; ++i) {
            sar(integer, 63);
        }
        if (times % 63) {
            sar(integer, times % 63);
        }
    }
}

// 返回integer的高bits位
static u64 get_top_bits(u128 *integer, int bits)
{
    u64 mask, mask_clr, top;

    mask_clr = 1ULL << bits;
    mask_clr -= 1ULL;
    mask = mask_clr << 64-bits;

    top = integer->high & mask;
    top >>= 64-bits;
    top &= mask_clr;

    return top;
}

// 负数先取相反数（补码）进行运算，最后再确定商和余数的符号
void divide(n128 *quotient, u64 *remainder, n128 dividend, u64 divisor)
{
    int i, flag_quotient, flag_remainder, bits, times;
    u64 mask;

    mask = 1ULL << 63;
    // 无法计算有符号数的最小值，因为它的相反数（补码）是它本身
    if (dividend.high == mask && dividend.low == 0ULL || divisor == mask) {
        fprintf(stderr, "Can't calculate!\n");
        exit(EXIT_FAILURE);
    }
    if (divisor == 0ULL) {
        fprintf(stderr, "Division by zero!\n");
        exit(EXIT_FAILURE);
    }

    flag_quotient = 0;
    // 被除数和除数异号，则商为负
    if (dividend.high & mask ^ divisor & mask) {
        flag_quotient = 1;
    }

    // 如果除数为负，则取相反数（补码）
    if (divisor & mask) {
        divisor = ~divisor;
        divisor += 1ULL;
    }

    flag_remainder = 0;
    // 如果被除数为负，则取相反数（补码），另外余数和被除数同号
    if (dividend.high & mask) {
        dividend.high = ~dividend.high;
        dividend.low = ~dividend.low;
        dividend.low += 1ULL;
        if (dividend.low < 1ULL) {
            dividend.high += 1ULL;
        }
        flag_remainder = 1;
    }

    times = 0;
    // times是除数高位连续0的个数
    while (!(divisor<<times & mask)) {
        ++times;
    }
    // bits是除数真正占用的二进制位，例如5真正占用的只有3个二进制位
    bits = 64 - times;

    // 用做十进制除法的方法来做二进制除法
    quotient->low = 0ULL;
    quotient->high = 0ULL;
    *remainder = get_top_bits(&dividend, bits);
    shl(&dividend, bits);
    if (*remainder >= divisor) {
        quotient->low = 1ULL;
        *remainder -= divisor;
    }
    for (i = 0; i < 128-bits; ++i) {
        shl(quotient, 1);
        *remainder <<= 1;
        *remainder += get_top_bits(&dividend, 1);
        shl(&dividend, 1);
        if (*remainder >= divisor) {
            quotient->low |= 1ULL;
            *remainder -= divisor;
        }
    }

    // 对余数取相反数（补码）
    if (flag_remainder) {
        *remainder = ~*remainder;
        *remainder += 1ULL;
    }

    // 对商取相反数（补码）
    if (flag_quotient) {
        quotient->high = ~quotient->high;
        quotient->low = ~quotient->low;
        quotient->low += 1ULL;
        if (quotient->low < 1ULL) {
            quotient->high += 1ULL;
        }
    }
}
