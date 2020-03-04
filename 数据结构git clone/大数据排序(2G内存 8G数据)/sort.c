#include "type.h"
#include "sort.h"
#define cutoff 3

// 交换两个元素的值
static void swap(ElementType *one, ElementType *two)
{
    ElementType tmp;

    tmp = *one;
    *one = *two;
    *two = tmp;
}

// 插入排序
static void insertion_sort(ElementType *arr, int n)
{
    int i, j;
    ElementType tmp;

    for (i = 1; i < n; ++i) {
        tmp = arr[i];
        for (j = i-1; j >= 0 && arr[j] > tmp; --j) {
            arr[j+1] = arr[j];
        }
        arr[j+1] = tmp;
    }
}

// 找枢轴
static ElementType median3(ElementType *arr, int left, int right)
{
    int center;

    center = (left + right) / 2;
    if (arr[left] > arr[center]) {
        swap(&arr[left], &arr[center]);
    }
    if (arr[left] > arr[right]) {
        swap(&arr[left], &arr[right]);
    }
    if (arr[center] > arr[right]) {
        swap(&arr[center], &arr[right]);
    }
    swap(&arr[center], &arr[right-1]);

    return arr[right-1];
}

// 让数组arr中索引为k的元素归位
void quick_select(int k, ElementType *arr, int left, int right)
{
    int i, j;
    ElementType pivot;

    if (left + cutoff <= right) {
        pivot = median3(arr, left, right);
        i = left;
        j = right - 1;
        while (1) {
            while (arr[++i] < pivot) {
                continue;
            }
            while (arr[--j] > pivot) {
                continue;
            }
            if (i < j) {
                swap(&arr[i], &arr[j]);
            } else {
                break;
            }
        }
        swap(&arr[i], &arr[right-1]);

        if (k < i) {
            quick_select(k, arr, left, i-1);
        } else if (k > i) {
            quick_select(k, arr, i+1, right);
        }
    } else {
        insertion_sort(arr+left, right-left+1);
    }
}

static void qsort(ElementType *arr, int left, int right)
{
    int i, j;
    ElementType pivot;

    if (left + cutoff <= right) {
        pivot = median3(arr, left, right);
        i = left;
        j = right - 1;
        while (1) {
            while (arr[++i] < pivot) {
                continue;
            }
            while (arr[--j] > pivot) {
                continue;
            }
            if (i < j) {
                swap(&arr[i], &arr[j]);
            } else {
                break;
            }
        }
        swap(&arr[i], &arr[right-1]);

        qsort(arr, left, i-1);
        qsort(arr, i+1, right);
    } else {
        insertion_sort(arr+left, right-left+1);
    }
}

void quick_sort(ElementType *arr, int n)
{
    qsort(arr, 0, n-1);
}
