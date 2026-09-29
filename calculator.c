#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>

void validate(char *exp);
int operation(int num1, int num2, char op);
int precedence(char op);
int evaluation();

char operators[] = "+-*/";
int nums[1000];
int numscount = 0;
char op[1000];
int opcount = 0;
bool invalid = false;

int main() {
    char exp[1000];
    fgets(exp, sizeof(exp), stdin);
    int len = strlen(exp);
    if (len > 0 && exp[len-1] == '\n') {
        exp[len-1] = '\0';
    }

    validate(exp);
    if (numscount && opcount) {
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
        printf("Error: Empty Expression\n");
        invalid = true;
    }
    else if (!isdigit(exp[0]) || !isdigit(exp[len-1])) {
        printf("Error: Invalid Expression\n");
        invalid = true;
    }
    else {
        char num[1000] = "";
        int numlen = 0;
        for (int i = 0; i < len; i++) {
            if (strchr(operators, exp[i])) {
                if (numlen > 0) {
                    num[numlen] = '\0';
                    nums[numscount++] = atoi(num);
                    numlen = 0;
                    num[0] = '\0';
                }
                if (i > 0 && i < len) {
                    if (strchr(operators, exp[i-1]) || strchr(operators, exp[i+1])) {
                        printf("Error: Invalid Expression\n");
                        invalid = true;
                        break;
                    }
                    else {
                        op[opcount++] = exp[i];
                    }
                }
            }
            else if (isdigit(exp[i])) {
                num[numlen++] = exp[i];
            }
            else if (exp[i] == ' ') {
                continue;
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
                printf("Error: Division ny Zero\n");
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
    char stack[1000];
    int stackpeek = -1;
    int postfix[1000];
    int postfixpeek = -1;
    int i = 0, j = 0;

    while (i < numscount) {
        postfix[++postfixpeek] = nums[i];
        if (j < opcount) {
            if (stackpeek == -1) {
                stack[++stackpeek] = op[j];
            }
            else {
                if (precedence(op[j]) <= precedence(stack[stackpeek])) {
                    int num2 = postfix[postfixpeek--];
                    int num1 = postfix[postfixpeek--];
                    char operator = stack[stackpeek--];
                    int ans = operation(num1, num2, operator);
                    if (!invalid) {
                        postfix[++postfixpeek] = ans;
                        stack[++stackpeek] = op[j];
                    }
                    else {
                        return 0;
                    }
                }
                else {
                    stack[++stackpeek] = op[j];
                }
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

