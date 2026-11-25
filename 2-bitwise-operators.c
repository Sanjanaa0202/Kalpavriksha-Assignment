#include <stdio.h>

//Complete the following function.

#define MAX(a, b) ((a) > (b) ? (a) : (b))


void calculate_the_maximum(int n, int k) {
    int and_max = 0, or_max = 0, xor_max = 0;
    int and_op, or_op, xor_op;
    //Write your code here.
    for (int i = 1; i < n; i++) {
        for (int j = i+1; j <= n; j++) {
            and_op = i & j;
            or_op = i | j;
            xor_op = i ^ j;
            if (and_op < k) {
                and_max = MAX(and_max, and_op);
            }
            if (or_op < k) {
                or_max = MAX(or_max, or_op);
            }
            if (xor_op < k) {
                xor_max = MAX(xor_max, xor_op);
            }
        }
    }
    printf("%d\n%d\n%d\n", and_max, or_max, xor_max);
}

int main() {
    int n, k;
  
    scanf("%d %d", &n, &k);
    calculate_the_maximum(n, k);
 
    return 0;
}
