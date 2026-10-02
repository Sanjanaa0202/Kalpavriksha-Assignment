#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_LEN 1000

void validate(char *exp);
int operation(int num1, int num2, char op);
int precedence(char op);
int evaluation();

char operators[] = "+-*/";
int nums[MAX_LEN];
int numscount = 0;
char op[MAX_LEN];
int opcount = 0;
bool invalid = false;

int main() {
    char exp[MAX_LEN];
    printf("Enter an expression: ");
    fgets(exp, sizeof(exp), stdin);
    int len = strlen(exp);
    if (len > 0 && exp[len-1] == '\n') {
        exp[len-1] = '\0';
    }

    validate(exp);
    if (!invalid && numscount > 0) {
        int ans = evaluation();
        if (!invalid) {
            printf("%d\n", ans);
        }
    }
    return 0;
} 

void validate(char *exp) {
    int len = strlen(exp);

    if (len == 0) {
        printf("Error: Invalid Expression\n");
        invalid = true;
    }
    else {
        char num[MAX_LEN] = "";
        int numlen = 0;
        bool is_now_op = false;
        
        for (int i = 0; i < len; i++) {
            if (isspace(exp[i])){
                continue;
            }
            else if (isdigit(exp[i])) {
                if (is_now_op) {
                    printf("Error: Invalid Expression.\n");
                    invalid = true;
                    break;
                }
                num[numlen++] = exp[i];
                if (i+1 < len && !isdigit(exp[i+1])) {
                    num[numlen] = '\0';
                    nums[numscount++] = atoi(num);
                    numlen = 0;
                    is_now_op = true;
                }
            }
            else if (strchr(operators, exp[i])) {
                if (!is_now_op) {
                    printf("Error: Invalid Expression\n");
                    invalid = true;
                    break;
                }
                op[opcount++] = exp[i];
                is_now_op = false;
            }
            else {
                printf("Error: Invalid Expression\n");
                invalid = true;
                break;
            }
        }

        if (numlen > 0) {
            num[numlen] = '\0';
            nums[numscount++] = atoi(num);
            is_now_op = true;
        }

        if (!is_now_op) {
            printf("Error: Invalid Expression.\n");
            invalid = true;
        }
    }

    if (invalid) {
        numscount = 0;
        opcount = 0;
    }
}

int operation(int num1, int num2, char op) {
    switch (op) {
        case '+':
            return num1 + num2;
        case '-':
            return num1 - num2;
        case '*':
            return num1 * num2;
        case '/':
            if (num2 == 0) {
                printf("Error: Division by zero.\n");
                invalid = true;
                return 0;
            }
            else {
                return num1 / num2;
            }
    }
    return 0;
}

int precedence(char op) {
    if (op == '-' || op == '+') {
        return 1;
    }
    else if (op == '/' || op == '*') {
        return 2;
    }
    return 0;
}

int evaluation() {
    char stack[MAX_LEN];
    int stackpeek = -1;
    int postfix[MAX_LEN];
    int postfixpeek = -1;
    int i = 0, j = 0;

    while (i < numscount) {
        postfix[++postfixpeek] = nums[i];
        if (j < opcount) {
            if (stackpeek == -1) {
                stack[++stackpeek] = op[j];
            }
            else {
                while (stackpeek != -1 && precedence(stack[stackpeek]) >= precedence(op[j])) {
                    int num2 = postfix[postfixpeek--];
                    int num1 = postfix[postfixpeek--];
                    char operator = stack[stackpeek--];
                    int ans = operation(num1, num2, operator);
                    if (invalid) {
                        return 0;
                    }
                    postfix[++postfixpeek] = ans;
                }
                stack[++stackpeek] = op[j];
            }
            j++;
        }
        i++;
    }

    while (stackpeek != -1) {
        int num2 = postfix[postfixpeek--];
        int num1 = postfix[postfixpeek--];
        char operator = stack[stackpeek--];
        int ans = operation(num1, num2, operator);
        if (!invalid) {
            postfix[++postfixpeek] = ans;
        }
        else {
            return 0;
        }
    }
    return postfix[postfixpeek];
}

