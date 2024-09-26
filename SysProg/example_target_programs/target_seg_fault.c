#include <stdio.h>

int main() {
    int *ptr = NULL;
    int value;

    // Access an invalid memory location

    fprintf(stderr, "target executed\t");

    value = *ptr;

    printf("Enter a value: ");
    scanf("%d", &value);

    printf("You entered: %d\n", value);

    return 0;
}