#include <stdio.h>

int main() 
{

    int n;
    scanf("%d", &n);
      // Complete the code to print the pattern.
    int size = 2 * n - 1;
    int left, right;
    
    for (int row = 0; row < size; row++) {
        left = row;
        right = size - row - 1;
        
        for (int col = 0; col < size; col++) {
            if (left <= right) {
                if (col < left) {
                    printf("%d ", n - col);
                }
                else if (col > right) {
                    printf("%d ", n - (size - col - 1));
                }
                else if (left <= right) {
                    printf("%d ", n - row);
                }
            }
            else if (right < left) {
                if (col < right) {
                    printf("%d ", n - col);
                }
                else if (col > left) {
                    printf("%d ", n - (size - col - 1));
                }
                else if (right <= left) {
                    printf("%d ", n - (size - row - 1));
                }
            }
        }
        printf("\n");
    }
    
    
    return 0;
}
