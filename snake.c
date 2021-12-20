#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>
#include <unistd.h>
#include <string.h>
struct IntegerLinkedList
{ //A simple linked list to keep track of snake parts
    int index;
    struct IntegerLinkedList* next;
};

int board[40 * 40]; //The board is 40x40 pixels
enum Direction {RIGHT = 0, DOWN = 1, LEFT = 2, UP = 3} direction;
struct IntegerLinkedList* snake;
int snakeSize;
int points;
int fruitLocation;
int done = 0;

int randomIndex()
{
    return rand() % (40*40);
}

int snakeExistsHere(int index)
{
    struct IntegerLinkedList* curSnake = snake;
    while(curSnake != NULL)
    {
        if(curSnake->index == index)
        {
            return 1;
        }
        curSnake = curSnake->next;
    }
    return 0;
}

void newFruitLocation()
{
    do
    {
        //fruitLocation = snake->index + 5;
        fruitLocation = randomIndex();
    }
    while(snakeExistsHere(fruitLocation));
}

void start()
{
    snake = (struct IntegerLinkedList* ) malloc(sizeof(struct IntegerLinkedList));
    //snake->index = 0;
    snake->index = randomIndex();
    snake->next = NULL;
    newFruitLocation();
    snakeSize = 1;
    direction = 0;//rand() % 4;
}

void step()
{
    points++;
    int newIndex;
    switch(direction)
    {
        case RIGHT:
            newIndex = snake->index + 1;
            break;
        case DOWN:
            newIndex = snake->index + 40;
            break;
        case LEFT:
            newIndex = snake->index - 1;
            break;
        case UP:
            newIndex = snake->index - 40;
            break;
    }

    if(snakeExistsHere(newIndex))
    {
        done = 1;
    }

    if(snake->index / 40 == 0 && direction == UP ||
       snake->index / 40 == 39 && direction == DOWN ||
       snake->index % 40 == 0 && direction == LEFT ||
       snake->index % 40 == 39 && direction == RIGHT)
    {
        done = 1;
    }

    if(newIndex == fruitLocation)
    {
        //Grab the last snake in the linked list
        //TODO: Remove this lazy for loop
        for(int i = 0; i < 5 ; i++)
        {
            struct IntegerLinkedList* lastSnakePiece = snake;
            while(lastSnakePiece->next != NULL)
            {
                lastSnakePiece = lastSnakePiece->next;
            }
            struct IntegerLinkedList* newSnakePiecePointer = (struct IntegerLinkedList* ) malloc(sizeof(struct IntegerLinkedList));
            lastSnakePiece->next = newSnakePiecePointer;
            newSnakePiecePointer->next = NULL;
            snakeSize++;
        }
        newFruitLocation();
    }

    int oldIndex = newIndex;
    struct IntegerLinkedList* curSnake = snake;
    while(curSnake != NULL)
    {
        int temp = curSnake->index;
        curSnake->index = oldIndex;
        oldIndex = temp;
        curSnake = curSnake->next;
    }
    
}

void display()
{
    //Clear display
    erase();

    //Show your points, snake's length, and a horizontal bar.
    move(0, 0);
    printw("%d", points);

    char snakeSizeString[30];
    sprintf(snakeSizeString, "%d", snakeSize);
    move(0, 40 - strlen(snakeSizeString));
    printw("%s", snakeSizeString);

    move(1, 0);
    addstr("----------------------------------------");
    
    //Now, draw the snakes and the fruit
    //Snake is # symbols, Fruit is @
    move(1, 43); printw("%d", (fruitLocation / 40));
    move(1, 47); printw("%d", (fruitLocation % 40));

    move( 2 + (fruitLocation / 40), fruitLocation % 40);
    addch('@');

    int snakeCount = 0;
    struct IntegerLinkedList* curSnake = snake;
    while(curSnake != NULL)
    {
        snakeCount++;
        move( 2 + (curSnake->index / 40), curSnake->index % 40);
        addch('#');

        //move(2 + snakeCount, 43);
        //printw("%d", curSnake->index);

        curSnake = curSnake->next;

    }

    move(0,43);
    printw("%d", snakeCount);


    move(0,0);
    refresh();
}

int main()
{
    //Seting up the ncurses display.
    initscr();
    timeout(0);
    noecho();

    //Setting things up for the game.
    srand(time(NULL));
    start();

    while(!done)
    {
        step();
        display();

        enum Direction newDirection = -1;
        clock_t oldTime = clock();
        do
        {
            char input = getch();
            switch(input)
            {
                case 'd':
                    if(direction != LEFT)
                    {
                        newDirection = RIGHT;
                    }
                    break;
                case 's':
                    if(direction != UP)
                    {
                        newDirection = DOWN;
                    }
                    break;
                case 'a':
                    if(direction != RIGHT)
                    {
                        newDirection = LEFT;
                    }
                    break;
                case 'w':
                    if(direction != DOWN)
                    {
                        newDirection = UP;
                    }
                    break;
                default:
                    break;
            }
        }
        while(clock() * 100 < (100 * oldTime) + (6 * CLOCKS_PER_SEC));
        if(newDirection != -1)
        {
            direction = newDirection;
        }
        //To get fractions of a second, multiply the timer by 100,
        //so we tick once every 16/100ths of a second

        //usleep(250 * 1000);
    }

    //TODO:

    endwin();
    printf("Points: %d\nSnake Length: %d\n", points, snakeSize);
    return 0;
}