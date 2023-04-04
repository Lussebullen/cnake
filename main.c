#include <curses.h> // Linux replacement for conio.h
#include <stdlib.h>
#include <unistd.h>

#define HEIGTH 22
#define WIDTH 42

char grid[HEIGTH][WIDTH+1];
int i, j;

struct BodyNode {
    int x, y;
    struct BodyNode* next;
};

struct Snake {
    struct BodyNode* head;
    struct BodyNode* tail;
    int alive;
};

void set_bound(void) {
    for (i = 0; i < HEIGTH; i = i + 1) {
        for (j = 0; j < WIDTH; j = j + 1) {
            if (i == 0 || j == 0 || i == HEIGTH - 1 || j == WIDTH - 1) {
                grid[i][j] = '#';
            } else {
                grid[i][j] = ' ';
            }
        }
    }
}

void draw(void) {
    for (i = 0; i < HEIGTH; i = i + 1) {
        printw("%s\n",grid[i]);
        refresh();
    }
}

void set_head(struct Snake* snake) {
    int x = snake->head->x;
    int y = snake->head->y;
    grid[y][x] = '@';
}

struct Snake* init_snake(int x, int y) {
    struct BodyNode* body = NULL;
    struct Snake* snake = NULL;
    body = (struct BodyNode*)malloc(sizeof(struct BodyNode));
    snake = (struct Snake*)malloc(sizeof(struct Snake));
    body->x = x;
    body->y = y;
    body->next = NULL;
    snake->head = body;
    snake->tail = body;
    snake->alive = 1;

    set_head(snake);

    return snake;
}

int is_valid_move(char move, struct Snake* snake) {
    int xNew = snake->head->x;
    int yNew = snake->head->y;
    switch(move)
    {
        case 'w':
            yNew = yNew - 1;
            break;

        case 's':
            yNew = yNew + 1;
            break;

        case 'a':
            xNew = xNew - 1;
            break;

        case 'd':
            xNew = xNew + 1;
            break;
    }

    // if headed where tail currently is, won't hit.
    int tail_location = (xNew == snake->tail->x) && (yNew == snake->tail->y);
    // FIXME: Add handling of fruit
    return (grid[yNew][xNew] == ' ') || tail_location;
}

int is_valid_direction(int ch) {
    return ch == 'w' || ch == 'a' || ch == 's' || ch == 'd';
}

void move_snake(char move, struct Snake* snake) {
    
    // Temp, before full linked list implementation //
    int x = snake->head->x;
    int y = snake->head->y;
    grid[y][x] = ' ';
    //////////////////////////////////////////////////

    switch(move)
    {
        case 'w':
            snake->head->y = snake->head->y - 1;
            break;

        case 's':
            snake->head->y = snake->head->y + 1;
            break;

        case 'a':
            snake->head->x = snake->head->x - 1;
            break;

        case 'd':
            snake->head->x = snake->head->x + 1;
            break;
    }

    x = snake->head->x;
    y = snake->head->y;
    int ate_fruit = grid[y][x] == '*';

    set_head(snake);
    // FIXME: handle tail and growing
    
    
}

int main(void)
{
    struct Snake* snake;
    char direction;
    int input = 0;

    set_bound();
    snake = init_snake(20, 10);
    
    initscr();
    noecho(); // do not print the pressed character
    draw();

    // initialize direction //
    while (is_valid_direction(input) == 0) {
        input = getch();
    }
    direction = (char) input;
    //////////////////////////
    
    while (input != 'q' && snake->alive) { // modify to alive
        input = getch();
        if (is_valid_direction(input)) {
            direction = (char) input;
        }

        if (is_valid_move(direction, snake)) {
            move_snake(direction, snake);
        } else {
            snake->alive = 0;
        }


        clear();
        draw();
        
        // for development //
        // refresh();
        // printw("%c\n",direction);
        /////////////////////

        timeout(500);
    }
    // sleep(2);
    endwin();
    return 0;
}