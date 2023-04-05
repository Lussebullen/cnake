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
    struct BodyNode* prev;
};

struct Snake {
    struct BodyNode* head;
    struct BodyNode* tail;
    int alive;
    int score;
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

void set_head(struct Snake* snake) {
    int x = snake->head->x;
    int y = snake->head->y;
    grid[y][x] = '@';
}

struct Snake* init_snake(int x, int y) {
    struct BodyNode* head = NULL;
    struct BodyNode* tail = NULL;
    struct Snake* snake = NULL;
    head = (struct BodyNode*)malloc(sizeof(struct BodyNode));
    tail = (struct BodyNode*)malloc(sizeof(struct BodyNode));
    snake = (struct Snake*)malloc(sizeof(struct Snake));
    
    head->x = x;
    head->y = y;
    head->next = tail;
    head->prev = NULL;
    snake->head = head;

    tail->x = x;
    tail->y = y;
    tail->next = NULL;
    tail->prev = head;
    snake->tail = tail;

    snake->alive = 1;

    snake->score = 0;

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
    int empty_space = (grid[yNew][xNew] == ' ');
    int fruit = (grid[yNew][xNew] == '*');
    return empty_space || fruit || tail_location;
}

int is_valid_direction(int ch) {
    return ch == 'w' || ch == 'a' || ch == 's' || ch == 'd';
}

void set_body(struct Snake* snake) {
    int x = snake->head->x;
    int y = snake->head->y;
    grid[y][x] = 'o';
}

void grow_snake(struct Snake* snake, int x, int y) {
    if (snake->score != 0) {
        struct BodyNode* new_head;
        new_head = (struct BodyNode*)malloc(sizeof(struct BodyNode));
        new_head->x = x;
        new_head->y = y;
        new_head->next = snake->head;
        new_head->prev = NULL;

        snake->head->prev = new_head;
        snake->head = new_head;
    } else {
        snake->head->x = x;
        snake->head->y = y;
    }
    snake->score = snake->score + 1;
}

void clear_tail(struct Snake* snake) {
    int x = snake->tail->x;
    int y = snake->tail->y;
    grid[y][x] = ' ';
}

void snake_wrap(struct Snake* snake, int x, int y) {
    
    if (snake->score > 0) {
        // struct BodyNode* temp;
        
        snake->tail->next = snake->head;
        snake->head->prev = snake->tail;
        
        // temp = snake->tail;
        snake->head = snake->tail;
        snake->tail = snake->head->prev;
        snake->tail->next = NULL;
        snake->head->prev = NULL;

        snake->head->x = x;
        snake->head->y = y;
    } else {
        snake->head->x = x;
        snake->head->y = y;
        snake->tail->x = x;
        snake->tail->y = y;
    }
}

void set_fruit(void) {
    int x = 0;
    int y = 0;

    while (grid[y][x] != ' ') {
        x = rand() % HEIGTH;
        y = rand() % WIDTH;
    }

    grid[y][x] = '*';
}

void move_snake(char move, struct Snake* snake) {

    set_body(snake);
    int x_new = snake->head->x;
    int y_new = snake->head->y;

    switch(move)
    {
        case 'w':
            y_new = y_new - 1;
            break;

        case 's':
            y_new = y_new + 1;
            break;

        case 'a':
            x_new = x_new - 1;
            break;

        case 'd':
            x_new = x_new + 1;
            break;
    }

    int ate_fruit = grid[y_new][x_new] == '*';

    if (ate_fruit) {
        grow_snake(snake, x_new, y_new);
        set_fruit();
    } else {
        clear_tail(snake);
        snake_wrap(snake, x_new, y_new);
    }
    set_head(snake);
}

void draw(struct Snake* snake) {
    for (i = 0; i < HEIGTH; i = i + 1) {
        printw("%s\n",grid[i]);
    }
    printw("\nScore: %d\n",snake->score);
    refresh();
}

int speed(struct Snake* snake) {
    int base_cycle = 500;
    int cycle = base_cycle;

    if (cycle > 99) {
        cycle = cycle - 10*snake->score;
    } else {
        cycle = 100;
    }
    return cycle;
}

int main(void)
{
    struct Snake* snake;
    char direction;
    int input = 0;

    set_bound();
    set_fruit();
    set_fruit();
    snake = init_snake(20, 10);
    
    initscr();
    noecho(); // do not print the pressed character
    draw(snake);

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
        draw(snake);
        timeout(speed(snake));
    }

    endwin();
    return 0;
}