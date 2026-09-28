#include <stdio.h>
#include <string.h>
#include <ctype.h> 

int main() {
    char expr[1000];

    printf("Enter expression: ");
    fgets(expr, sizeof(expr), stdin);
    expr[strcspn(expr, "\n")] = '\0';
    long numbers[100];
    char operators[100];
    int numCount = 0, opCount = 0;
    int i = 0;
    int isValid = 1;

    while (expr[i] != '\0'){
        if (expr[i] == ' '){
            i++;
            continue;
        }

        if (isdigit(expr[i])){
            long num = 0;
            while (isdigit(expr[i])){
                num = num * 10 + (expr[i] - '0');
                i++;
            }
            numbers[numCount] = num;
            numCount++;
        }
        else if (expr[i]=='+' || expr[i]=='-' || expr[i]=='*' || expr[i]=='/'){
            operators[opCount] = expr[i];
            opCount++;
            i++;
        }
        else {
            isValid = 0;
            break;
        }
    }
    if (numCount != opCount + 1 || numCount == 0){
        isValid = 0;
    }
    if (!isValid){
        printf("Error: Invalid expression.\n");
        return 0;
    }

    int divByZero = 0;
    for (int j = 0; j < opCount; j++){
        if (operators[j] == '*') {
            numbers[j+1] = numbers[j] * numbers[j+1];
            numbers[j] = -999999;
            operators[j] = '#';
        }
        else if (operators[j] == '/'){
            if (numbers[j+1] == 0) {
                divByZero = 1;
                break;
            }
            numbers[j+1] = numbers[j] / numbers[j+1];
            numbers[j] = -999999;
            operators[j] = '#';
        }
    }
    if (divByZero){
        printf("Error: Division by zero.\n");
        return 0;
    }
    // for + and -
    long result = 0;
    int started = 0;
    for (int j = 0; j < numCount; j++){
        if (numbers[j] == -999999) continue;
        if (!started){
            result = numbers[j];
            started = 1;
        } else{
            char op = operators[j-1];
            if (op == '+') result += numbers[j];
            else if (op == '-') result -= numbers[j];
        }
    }
    printf("%ld\n", result);
    return 0;
}