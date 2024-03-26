#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

struct buf {
    char line[80];
    int link;
};

struct buf Buffer[100];
int head = 0;
int bufferCount = 0;

char Tr_var[20]; // each variable name is 1 character
int nTrvar; // number of variables in the array

// Move the cursor up by one line
void move_cursor_up() {
    int row, col;
    getyx(stdscr, row, col);

    move(row - 1, 0);
}

// Move the cursor down by one line
void move_cursor_down() {
    int row, col, max_row, max_col;
    getyx(stdscr, row, col);
    getmaxyx(stdscr, max_row, max_col);

    move(row + 1, 0);
}

// Move the cursor to the last line on the screen
void move_cursor_to_last_line() {
    int max_row, max_col;
    getmaxyx(stdscr, max_row, max_col);
    move(max_row-1, 0);
}

void display_buffer() {
    clear();

    int current = head;
    int row_index = 0;

    while (current != -1) {
        mvprintw(row_index, 0, "%s", Buffer[current].line);
        current = Buffer[current].link;
        row_index++;
    }
}

void read_to_buffer(const char *filename) {
    FILE *file = fopen(filename, "r");

    int index = 0;

    while (index < 99 && fgets(Buffer[index].line, sizeof(Buffer[index].line), file) != NULL) {
        Buffer[index].link = index + 1;
        bufferCount++;
        index++;
    }

    Buffer[index - 1].link = -1;
    fclose(file);
}

void add_to_buffer(int row, char* input) {
    strcat(input,"\n");

    int rowIndex = 0;
    for(int i = 0; i < row && rowIndex != -1; i++) {
        rowIndex = Buffer[rowIndex].link;
    }

    strncpy(Buffer[bufferCount].line, input, 80);
    Buffer[bufferCount].link = Buffer[rowIndex].link;
    Buffer[rowIndex].link = bufferCount;

    bufferCount++;

    display_buffer();
}


void write_to_file(const char *filename) {
    FILE *file = fopen(filename, "w");

    int current = head;
    while (current != -1) {
        fprintf(file, "%s", Buffer[current].line);
        current = Buffer[current].link;
    }

    fclose(file);
}

int main() {
    initscr(); // Start NCURSES mode
    noecho();

    read_to_buffer("myprog.c");
    display_buffer();

    int ch;

    while ((ch = getch()) != 'x') {
        switch (ch) {
            case 'd':
                // Display buffer on the screen
                display_buffer();
                break;
            case 'w':
                // Move cursor up one line
                move_cursor_up();
                break;
            case 's':
                // Move cursor down one line
                move_cursor_down();
                break;
            case 'i':
                // Take input and add to buffer
                echo();

                int row, col; // Current position of cursor
                getyx(stdscr, row, col);

                move_cursor_to_last_line();

                // Allow the user to input a directive
                char input[80];
                getnstr(input, 79);

                add_to_buffer(row, input);
                noecho();
                break;
        }
    }

    endwin(); // End NCURSES mode

    // Write buffer to temp file
    write_to_file("temp.txt");

    read_to_buffer("temp.txt");

    int i = 0;
    int counter = 0;
    while (i < bufferCount) {

        if(strstr(Buffer[i].line,"@var") != NULL)
        {
            char temp[80];
            char *position;
            char *variable;
            int index;

            position = strstr(Buffer[i].line,"@var");
            variable = position + strlen("@var");
            variable++;
            Tr_var[nTrvar] = variable[0];
            ++nTrvar;
            index = position - Buffer[i].line;
            strncpy(temp, Buffer[i].line, index);
            temp[index] = '\0';

            strcat(temp,"add_TT('");
            strncat(temp,variable,1);
            strcat(temp,"');");
            strcat(temp,variable + 1);
            strcpy(Buffer[i].line,temp);
        }
        else if(strstr(Buffer[i].line,"@trace") != NULL)
        {
            char temp[80];
            char *position;
            char *variable;
            int index;

            position = strstr(Buffer[i].line,"@trace");
            variable = position + strlen("@trace");
            variable++;
            index = position - Buffer[i].line;
            strncpy(temp, Buffer[i].line, index);
            temp[index] = '\0';

            strcat(temp,"display_TT('");
            strncat(temp,variable,1);
            strcat(temp,"');");
            strcat(temp,variable + 1);
            strcpy(Buffer[i].line,temp);
        }
        else if ((strchr(Buffer[i].line, '=') != NULL))
        {
            // Find the variable name at the left handside of the '='
            char *position = strchr(Buffer[i].line, '=');
            position--;

            // check if the variable being assigned is a tracked variable
            for (int varIndex = 0; varIndex < nTrvar; varIndex++) {
                if (Tr_var[varIndex] == position[0])
                {
                    char updatedLine[80];
                    sprintf(updatedLine, "update_TT('%c', %c);", Tr_var[varIndex], Tr_var[varIndex]);

                    // insert the update_TT call after the assign statement line
                    add_to_buffer(i + counter, updatedLine);
                    counter++;
                    break;
                }
                else
                    continue;
            }
        }

        i++; // Increment the counter for the next line
    }

    write_to_file("expanded.c");

    return 0;
}
