#ifndef TYPE_H_
#define TYPE_H_

typedef int n32;
typedef unsigned int u32;
typedef long long n64;
typedef unsigned long long u64;
typedef struct s_n128 {
    u64 low;
    u64 high;
}u128;
typedef u128 n128;
struct s_part;
union section {
    struct s_part *part;
    char filename[11];
};
typedef struct s_part {
    union section left;
    union section right;
    n64 median;
    int flag;       // if flag == 0, left and right is a file, else is a part
}Part;
typedef struct s_node {
    Part *elem;
    struct s_node *next;
}Node;
typedef Node *List;

#endif
