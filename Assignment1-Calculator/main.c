#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>

int length(char *p);
int isValid(char *p, int n);
void removeSpace(char *p);
int isOperator(char ch);
int isInvalid(char ch);
int precedence(char ch);
int calculate(int left, int right, char op, int *result);
int evaluate(char *p, int n, int *result);

int main() {
    char expression[1000];

    printf("Enter the Expression: ");

    gets(expression);
    removeSpace(expression);
    int len = length(expression);

    if (!isValid(expression, len)) {
        printf("Error: Invalid expression.\n");
        return 0;
    }

    int result;

    if (evaluate(expression, len, &result)) {
        printf("Result: %d\n", result);
    }

return 0;
    return 0;
}

int length(char *p)
{
    int count = 0;
    int i = 0;

    while (*(p + i) != '\0') {
        count++;
        i++;
    }

    return count;
}

int isOperator(char ch)
{
    return ch == '+' || ch == '-' || ch == '*' || ch == '/';
}

int isInvalid(char ch)
{
    return !isdigit(ch) && !isOperator(ch);
}

void removeSpace(char *p)
{
    int i = 0;
    int j = 0;

    while (*(p + i) != '\0') {
        if (*(p + i) != ' '  &&
    *(p + i) != '\t') {
            *(p + j) = *(p + i);
            j++;
        }
        i++;
    }

    *(p + j) = '\0';
}

int isValid(char *p, int n)
{
    int expectNumber = 1;
    int i = 0;

    while (i < n) {
        char ch = *(p + i);

        if (isInvalid(ch)) {
            return 0;
        }

        if (expectNumber) {
            if (!isdigit(ch)) {
                return 0;
            }

            while (i < n && isdigit(*(p + i))) {
                i++;
            }

            expectNumber = 0;
        }
        else {
            if (!isOperator(ch)) {
                return 0;
            }

            i++;
            expectNumber = 1;
        }
    }

    return !expectNumber;
}

int evaluate(char *p, int n, int *result)
{
    int numbers[1000];
    char operators[1000];

    int numberTop = -1;
    int operatorTop = -1;
    int i = 0;

    if (n <= 0 || n >= 1000) {
        printf("Error: Invalid expression.\n");
        return 0;
    }

    while (i <= n) {
        char ch = *(p + i);

        if (ch >= '0' && ch <= '9') {
            int number = 0;

            while (i < n &&
                   *(p + i) >= '0' &&
                   *(p + i) <= '9') {

                int digit = *(p + i) - '0';

    
                // if (number > (INT_MAX - digit) / 10) {
                //     printf("Error: Integer overflow.\n");
                //     return 0;
                // }

                number = number * 10 + digit;
                i++;
            }

            numbers[++numberTop] = number;
        }
        else {
            while (operatorTop >= 0 &&
                   (ch == '\0' ||
                    precedence(operators[operatorTop]) >= precedence(ch))) {

                if (numberTop < 1) {
                    printf("Error: Invalid expression.\n");
                    return 0;
                }

                int right = numbers[numberTop--];
                int left = numbers[numberTop--];
                char op = operators[operatorTop--];

                int value;

                if (!calculate(left, right, op, &value)) {
                    return 0;
                }

                numbers[++numberTop] = value;
            }

            if (ch == '\0') {
                break;
            }

            operators[++operatorTop] = ch;
            i++;
        }
    }

    if (numberTop != 0) {
        printf("Error: Invalid expression.\n");
        return 0;
    }

    *result = numbers[numberTop];
    return 1;
}


int calculate(int left, int right, char op, int *result)
{
    if (op == '+') {
        *result = left + right;
    }
    else if (op == '-') {
        *result = left - right;
    }
    else if (op == '*') {
        *result = left * right;
    }
    else if (op == '/') {
        if (right == 0) {
            printf("Error: Division by zero.\n");
            return 0;
        }

        *result = left / right;
    }
    else {
        printf("Error: Invalid expression.\n");
        return 0;
    }

    return 1;
}

int precedence(char ch)
{
    if (ch == '*' || ch == '/') {
        return 2;
    }

    if (ch == '+' || ch == '-') {
        return 1;
    }

    return 0;
}