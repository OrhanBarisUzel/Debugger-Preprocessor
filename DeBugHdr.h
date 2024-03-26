// Debugger.h
#include <stdio.h>
// Traces table

struct tr {
    char name;
    int value;
};

struct tr Traces[20];
int nTr;

void add_TT(char var) {
    if (nTr < 20) {
        Traces[nTr].name = var;
        Traces[nTr].value = 0;  // Initialize the value to 0
        nTr++;
    } else {

        // Handle the case where the Traces table is full
        printf("The table is full.");
    }
}


void display_TT(char var) {
    for (int i = 0; i < nTr; i++) {
        if (Traces[i].name == var) {
            printf("%c: %d\n", var, Traces[i].value);
            getchar();
            return;
        }
    }
    printf("Variable %c not found in Traces table\n", var);
}

// Function to update the value of a variable in the Traces table
void update_TT(char var, int val) {
    for (int i = 0; i < nTr; i++) {
        if (Traces[i].name == var) {
            Traces[i].value = val;
            return;
        }
    }
    printf("Variable %c not found in Traces table\n",var);
}