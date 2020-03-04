#ifndef SORT_H_
#define SORT_H_

typedef n64 ElementType;

// 让数组arr中第k小的元素归位
void quick_select(int k, n64 *arr, int left, int right);

// 快速排序
void quick_sort(ElementType *arr, int n);

#endif
