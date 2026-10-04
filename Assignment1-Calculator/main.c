#include <stdio.h>
#include <ctype.h>

int length(char *expression);
int isValid(char *expression, int expressionLength);
void removeSpace(char *expression);
int isOperator(char character);
int isInvalid(char character);
int precedence(char operator);
int calculate(int leftOperand, int rightOperand, char operator, int *result);
int evaluate(char *expression, int expressionLength, int *result);

int main()
{
    char expression[1000];
    int expressionLength;
    int result;

    printf("Enter the Expression: ");

    gets(expression);

    removeSpace(expression);
    expressionLength = length(expression);

    if (!isValid(expression, expressionLength))
    {
        printf("Error: Invalid expression.\n");
    }
    else
    {
        if (evaluate(expression, expressionLength, &result))
        {
            printf("Result: %d\n", result);
        }
    }

    return 0;
}

int length(char *expression)
{
    int count = 0;
    int index = 0;

    while (*(expression + index) != '\0')
    {
        count++;
        index++;
    }

    return count;
}

int isOperator(char character)
{
    int result;

    result = character == '+' ||
             character == '-' ||
             character == '*' ||
             character == '/';

    return result;
}

int isInvalid(char character)
{
    int result;

    result = !isdigit(character) && !isOperator(character);

    return result;
}

void removeSpace(char *expression)
{
    int readIndex = 0;
    int writeIndex = 0;

    while (*(expression + readIndex) != '\0')
    {
        if (*(expression + readIndex) != ' ' &&
            *(expression + readIndex) != '\t')
        {
            *(expression + writeIndex) = *(expression + readIndex);
            writeIndex++;
        }

        readIndex++;
    }

    *(expression + writeIndex) = '\0';
}

int isValid(char *expression, int expressionLength)
{
    int expectNumber = 1;
    int currentIndex = 0;
    int valid = 1;

    while (currentIndex < expressionLength && valid)
    {
        char currentCharacter = *(expression + currentIndex);

        if (isInvalid(currentCharacter))
        {
            valid = 0;
        }
        else if (expectNumber)
        {
            if (!isdigit(currentCharacter))
            {
                valid = 0;
            }
            else
            {
                while (currentIndex < expressionLength &&
                       isdigit(*(expression + currentIndex)))
                {
                    currentIndex++;
                }

                expectNumber = 0;
            }
        }
        else
        {
            if (!isOperator(currentCharacter))
            {
                valid = 0;
            }
            else
            {
                currentIndex++;
                expectNumber = 1;
            }
        }
    }

    if (expectNumber)
    {
        valid = 0;
    }

    return valid;
}

int evaluate(char *expression, int expressionLength, int *result)
{
    int numbers[1000];
    char operators[1000];

    int numberTop = -1;
    int operatorTop = -1;
    int currentIndex = 0;
    int evaluationSuccessful = 1;

    if (expressionLength <= 0 || expressionLength >= 1000)
    {
        printf("Error: Invalid expression.\n");
        evaluationSuccessful = 0;
    }

    while (currentIndex <= expressionLength && evaluationSuccessful)
    {
        char currentCharacter = *(expression + currentIndex);

        if (currentCharacter >= '0' && currentCharacter <= '9')
        {
            int number = 0;

            while (currentIndex < expressionLength &&
                   *(expression + currentIndex) >= '0' &&
                   *(expression + currentIndex) <= '9')
            {
                int digit = *(expression + currentIndex) - '0';

                number = number * 10 + digit;
                currentIndex++;
            }

            numbers[++numberTop] = number;
        }
        else
        {
            while (operatorTop >= 0 &&
                   evaluationSuccessful &&
                   (currentCharacter == '\0' ||
                    precedence(operators[operatorTop]) >=
                    precedence(currentCharacter)))
            {
                if (numberTop < 1)
                {
                    printf("Error: Invalid expression.\n");
                    evaluationSuccessful = 0;
                }
                else
                {
                    int rightOperand = numbers[numberTop--];
                    int leftOperand = numbers[numberTop--];
                    char currentOperator = operators[operatorTop--];
                    int calculatedValue;

                    if (calculate(leftOperand,
                                  rightOperand,
                                  currentOperator,
                                  &calculatedValue))
                    {
                        numbers[++numberTop] = calculatedValue;
                    }
                    else
                    {
                        evaluationSuccessful = 0;
                    }
                }
            }

            if (evaluationSuccessful)
            {
                if (currentCharacter == '\0')
                {
                    currentIndex = expressionLength + 1;
                }
                else
                {
                    operators[++operatorTop] = currentCharacter;
                    currentIndex++;
                }
            }
        }
    }

    if (evaluationSuccessful)
    {
        if (numberTop != 0)
        {
            printf("Error: Invalid expression.\n");
            evaluationSuccessful = 0;
        }
        else
        {
            *result = numbers[numberTop];
        }
    }

    return evaluationSuccessful;
}

int calculate(int leftOperand,
              int rightOperand,
              char operator,
              int *result)
{
    int calculationSuccessful = 1;

    if (operator == '+')
    {
        *result = leftOperand + rightOperand;
    }
    else if (operator == '-')
    {
        *result = leftOperand - rightOperand;
    }
    else if (operator == '*')
    {
        *result = leftOperand * rightOperand;
    }
    else if (operator == '/')
    {
        if (rightOperand == 0)
        {
            printf("Error: Division by zero.\n");
            calculationSuccessful = 0;
        }
        else
        {
            *result = leftOperand / rightOperand;
        }
    }
    else
    {
        printf("Error: Invalid expression.\n");
        calculationSuccessful = 0;
    }

    return calculationSuccessful;
}

int precedence(char operator)
{
    int operatorPrecedence = 0;

    if (operator == '*' || operator == '/')
    {
        operatorPrecedence = 2;
    }
    else if (operator == '+' || operator == '-')
    {
        operatorPrecedence = 1;
    }

    return operatorPrecedence;
}