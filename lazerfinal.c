#include <stdio.h>
#include <stdlib.h>
#include <time.h>
// time.h library is used in this code to help placing walls randomly.

static const int ROW = 15;
static const int COL = 15;

void place_random_walls(int count);
int play_game(int level);
void print_board();
void interface();
char game_board[ROW][COL];
int board_i, board_j;

void board_fill(int level)
{
    for (board_i = 0; board_i < ROW; board_i++)
    {
        for (board_j = 0; board_j < COL; board_j++)
        {
            game_board[board_i][board_j] = '.';
        }
    }
    // (start/target point will be same at every run.)
    game_board[1][1] = '>';
    if (level < 3)
        game_board[ROW - 2][COL - 2] = 'H';
    else
    {
        game_board[ROW - 2][COL - 2] = 'H';
        game_board[2][COL - 2] = 'H';
    }
    return;
}
void place_random_walls(int count)
{
    int x, y;

    while (count > 0)
    {
        x = rand() % ROW;
        y = rand() % COL;

        if (x == 1 && y == 2)
            continue;
        if (x == ROW - 3 && y == COL - 2)
            continue;
        if (x == ROW - 2 && y == COL - 3)
            continue;
        if (x == ROW - 2 && y == COL - 2)
            continue;
        // (I placed these 4 if statements to avoid placing walls on or around start/target points)
        if (game_board[x][y] == '.')
        {
            game_board[x][y] = '#';
            count--;
        }
    }
}

int main()
{
    srand(time(NULL));
    interface();

    return 0;
}

void interface()
{
    int choice, level;

    while (1)
    {
        printf("\n---- LAZER GAME ----\n");
        printf("1. Start Game\n");
        printf("2. Exit\n");
        scanf("%d", &choice);

        if (choice == 2)
            return;

        if (choice != 1)
            continue;

        printf("Select Level (1-3): ");
        scanf("%d", &level);

        while (level >= 1 && level <= 3)
        {
            int result = play_game(level);

            int option;

            if (result)
            {
                if (level < 3)
                {
                    printf("\n1. Next Level\n");
                    printf("2. Main Menu\n");
                    scanf("%d", &option);

                    if (option == 1)
                        level++;
                    else
                        break;
                }
                else
                {
                    printf("\nYou finished all levels!\n");
                    printf("1. Main Menu\n");
                    scanf("%d", &option);
                    break;
                }
            }
            else
            {
                printf("\n1. Retry\n");
                printf("2. Main Menu\n");
                scanf("%d", &option);

                if (option == 1)
                    continue;
                else
                    break;
            }
        }
    }
}

void print_board()

{
    int i, j, k, m = ROW;
    char letter;
    for (i = 0; i < ROW; i++)
    {
        printf("\n");
        if (m > 0)
        {
            printf("%2d ", m);
        }
        for (j = 0; j < COL; j++)
        {
            printf("%c ", game_board[i][j]);
        }
        m -= 1;
    }
    printf("\n   ");
    for (k = 0; k < ROW; k++)
    {
        letter = 'a' + (k);
        printf("%c ", letter);
    }

    return;
}

int play_game(int level)
{
    board_fill(level);
    int wall_count;

    if (level == 1)
        wall_count = 7;
    else if (level == 2)
        wall_count = 20;
    else
        wall_count = 25;

    place_random_walls(wall_count);

    char command, mirror, col_char;
    int row_num, row, col;

    while (1)
    {
        print_board();
        printf("\n\nWrite 'm' to place mirrors, 'd' to delete mirrors and 'x' to start the game if you're ready.\n");
        scanf(" %c", &command);

        if (command == 'x')
            break;
        if (command == 'd')
        {
            printf("Enter location to delete mirror (e.g c6):\n");
            scanf(" %c%d", &col_char, &row_num);

            col = col_char - 'a';
            row = ROW - row_num;

            if (row < 0 || row >= ROW || col < 0 || col >= COL)
            {
                printf("Invalid location!\n");
                continue;
            }

            char c = game_board[row][col];
            if (c == '/' || c == '\\' || c == '+')
            {
                game_board[row][col] = '.';
                printf("Mirror deleted.\n");
            }
            else
                printf("You can't delete here.\n");
        }
        if (command == 'm')
        {
            printf("Enter location (e.g c6):\n");
            scanf(" %c%d", &col_char, &row_num);

            col = col_char - 'a';
            row = ROW - row_num;

            if (row < 0 || row >= ROW || col < 0 || col >= COL)
            {
                printf("Invalid location!\n");
                continue;
            }

            if (game_board[row][col] != '.')
            {
                printf("Cannot place a mirror here!\n");
                continue;
            }

            printf("Enter mirror type (/ or \\");
            if (level == 3)
                printf(" or +");
            printf("):\n");
            scanf(" %c", &mirror);

            if (mirror != '/' && mirror != '\\' && (level != 3 || mirror != '+'))
            {
                printf("Invalid mirror.\n");
                continue;
            }

            game_board[row][col] = mirror;
        }
    }

    int laser_x[20], laser_y[20], laser_dx[20], laser_dy[20], laser_active[20];
    int laser_count = 1;
    laser_x[0] = 1;
    laser_y[0] = 1;
    laser_dx[0] = 0;
    laser_dy[0] = 1;
    laser_active[0] = 1;
    // These variables are used for the calculations of the laser directions after hitting a mirror.

    int target_count;
    if (level == 3)
        target_count = 2;
    else
        target_count = 1;
    int reached_target = 0;

    while (1)
    {
        int active_lasers = 0;
        for (int i = 0; i < laser_count; i++)
        {
            if (!laser_active[i])
                continue;
            active_lasers++;

            laser_x[i] += laser_dx[i];
            laser_y[i] += laser_dy[i];

            int x = laser_x[i], y = laser_y[i];

            if (x < 0 || x >= ROW || y < 0 || y >= COL)
            {
                laser_active[i] = 0;
                continue;
            }

            char c = game_board[x][y];

            if (c == '.' || c == '*')
                game_board[x][y] = '*';
            else if (c == '\\')
            {
                int tmp = laser_dx[i];
                laser_dx[i] = laser_dy[i];
                laser_dy[i] = tmp;
            }
            else if (c == '/')
            {
                int tmp = laser_dx[i];
                laser_dx[i] = -laser_dy[i];
                laser_dy[i] = -tmp;
            }
            else if (c == '+')
            {
                if (laser_count + 2 < 20)
                {

                    laser_active[i] = 0;

                    int new_dx1 = 0, new_dy1 = 0;
                    int new_dx2 = 0, new_dy2 = 0;

                    if (laser_dx[i] == 0 && laser_dy[i] == 1)
                    {
                        new_dx1 = -1;
                        new_dy1 = 0;
                        new_dx2 = 1;
                        new_dy2 = 0;
                    }
                    else if (laser_dx[i] == 0 && laser_dy[i] == -1)
                    {
                        new_dx1 = -1;
                        new_dy1 = 0;
                        new_dx2 = 1;
                        new_dy2 = 0;
                    }
                    else if (laser_dx[i] == 1 && laser_dy[i] == 0)
                    {
                        new_dx1 = 0;
                        new_dy1 = 1;
                        new_dx2 = 0;
                        new_dy2 = -1;
                    }
                    else if (laser_dx[i] == -1 && laser_dy[i] == 0)
                    {
                        new_dx1 = 0;
                        new_dy1 = 1;
                        new_dx2 = 0;
                        new_dy2 = -1;
                    }
                    // Those if-blocks calculate the new directions of the laser beams after hitting '+')

                    // (Thats new laser 1 after hitting '+')
                    laser_x[laser_count] = x;
                    laser_y[laser_count] = y;
                    laser_dx[laser_count] = new_dx1;
                    laser_dy[laser_count] = new_dy1;
                    laser_active[laser_count] = 1;
                    laser_count++;

                    // (Laser 2)
                    laser_x[laser_count] = x;
                    laser_y[laser_count] = y;
                    laser_dx[laser_count] = new_dx2;
                    laser_dy[laser_count] = new_dy2;
                    laser_active[laser_count] = 1;
                    laser_count++;
                }
            }

            else if (c == 'H')
            {
                reached_target++;
                laser_active[i] = 0;
            }
            else if (c == '#')
                laser_active[i] = 0;
        }

        if (active_lasers == 0)
            break;
    }

    print_board();
    if (reached_target >= target_count)
    {
        printf("\nYOU WIN!\n");
        return 1;
    }
    else
    {
        printf("\nYOU LOST!\n");
        return 0;
    }
}
