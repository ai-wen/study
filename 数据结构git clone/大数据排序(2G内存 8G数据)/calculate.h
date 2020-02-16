#ifndef CALCULATE_H_
#define CALCULATE_H_

// 把64位有符号数扩展成128位有符号数
void extend(n128 *integer);

// sum += elem
void add_to(n128 *elem, n128 *sum);

// 把integer逻辑左移times位
void shl(u128 *integer, int times);

// 把integer逻辑右移times位
void shr(u128 *integer, int times);

// 把integer算术右移times位
void sar(u128 *integer, int times);

// 负数先取相反数（补码）进行运算，最后再确定商和余数的符号
void divide(n128 *quotient, u64 *remainder, n128 dividend, u64 divisor);

#endif
