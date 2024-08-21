#include <curses.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define WIDTH_CARD 20
#define HEIGHT_CARD 6
#define HEIGHT_LABEL_CARD 2

#define WIDTH_STATISTICS_CARD 25
#define HEIGHT_STATISTICS_CARD 12

#define NUM_CARD_IN_ROW (NUM_SPACES/4)

#define COL_LABEL_BROWN 1
#define COL_LABEL_SKYBLUE 2
#define COL_LABEL_PURPLE 3
#define COL_LABEL_ORANGE 4
#define COL_LABEL_RED 5
#define COL_LABEL_YELLOW 6
#define COL_LABEL_GREEN 7
#define COL_LABEL_BLUE 8
#define COL_LABEL_NOCOLOR 9

#define COL_NOPLAYER 10
#define COL_PLAYER1 11
#define COL_PLAYER2 12
#define COL_PLAYER3 13
#define COL_PLAYER4 14

#define COL_BROWN 17
#define COL_SKYBLUE 18
#define COL_PURPLE 19
#define COL_ORANGE 20
#define COL_RED 21
#define COL_YELLOW 22
#define COL_GREEN 23
#define COL_BLUE 24

#define NUM_PROPERTIES 16
#define NUM_SPACES 24
#define MAX_PLAYERS 4
#define NUM_SPACE_TYPES 6
#define NUM_COLORS 8
#define MAX_NAME 100
#define TOTAL_SPACES 24
#define INITIAL_CASH_MULTIPLIER 2

#ifdef _WIN32
#include <windows.h>
#else

#include <unistd.h>

#endif

/*
        * This is Sleep function which is universal for different OS
        * WARNING
        * IT HASN'T BEEN TESTED ON OTHER PLATFORMS WITH LINUX OR MACOS
        * CODE WAS TESTED ONLY ON WINDOWS
        * I WISH YOU'LL HAVE BEST EXPIRIENCE WITH THIS PROJECT
*/
void cross_sleep(int milliseconds) {
#ifdef _WIN32
    Sleep(milliseconds);
#else
    usleep(milliseconds * 1000);
#endif
}

/*
         * STRUCTURES FROM monopoly.h AND monopoly.c
*/

typedef enum space_type {
    Property, Start, Free_parking, In_jail, Go_to_jail, Jail_pass
} SPACE_TYPE;

typedef enum color {
    Brown, Skyblue, Purple, Orange, Red, Yellow, Green, Blue
} COLOR;

typedef struct property {
    char name[MAX_NAME + 1];
    int price;
    COLOR color;
} PROPERTY;

typedef struct space {
    SPACE_TYPE type;
    PROPERTY *property;
} SPACE;

typedef struct player {
    int number;
    int space_number;
    int cash;
    int num_jail_pass;
    int is_in_jail;
    PROPERTY *owned_properties[NUM_PROPERTIES];
    int num_properties;
} PLAYER;

const char *space_types[NUM_SPACE_TYPES] = {"PROPERTY", "START", "FREE PARKING", "IN JAIL", "GO TO JAIL", "JAIL PASS"};

const char *property_colors[NUM_COLORS] = {"Brown", "Skyblue", "Purple", "Orange", "Red", "Yellow", "Green", "Blue"};

PROPERTY properties[NUM_PROPERTIES] = {{.name = "FOOD TRUCK", .price = 1, .color = Brown},
                                       {.name = "PIZZA RESTAURANT", .price = 1, .color = Brown},
                                       {.name = "DOUGHNUT SHOP", .price = 1, .color = Skyblue},
                                       {.name = "ICE CREAM SHOP", .price = 1, .color = Skyblue},
                                       {.name = "MUSEUM", .price = 2, .color = Purple},
                                       {.name = "LIBRARY", .price = 2, .color = Purple},
                                       {.name = "THE PARK", .price = 2, .color = Orange},
                                       {.name = "THE BEACH", .price = 2, .color = Orange},
                                       {.name = "POST OFFICE", .price = 3, .color = Red},
                                       {.name = "TRAIN STATION", .price = 3, .color = Red},
                                       {.name = "COMMUNITY GARDEN", .price = 3, .color = Yellow},
                                       {.name = "PET RESCUE", .price = 3, .color = Yellow},
                                       {.name = "AQUARIUM", .price = 4, .color = Green},
                                       {.name = "THE ZOO", .price = 4, .color = Green},
                                       {.name = "WATER PARK", .price = 5, .color = Blue},
                                       {.name = "AMUSEMENT PARK", .price = 5, .color = Blue}};

SPACE game_board[NUM_SPACES] = {{.type = Start, .property = NULL},
                                {.type = Property, .property = &properties[0]},
                                {.type = Property, .property = &properties[1]},
                                {.type = Jail_pass, .property = NULL},
                                {.type = Property, .property =  &properties[2]},
                                {.type = Property, .property =  &properties[3]},
                                {.type = In_jail, .property =  NULL},
                                {.type = Property, .property =  &properties[4]},
                                {.type = Property, .property =  &properties[5]},
                                {.type = Jail_pass, .property =  NULL},
                                {.type = Property, .property =  &properties[6]},
                                {.type = Property, .property =  &properties[7]},
                                {.type = Free_parking, .property =  NULL},
                                {.type = Property, .property =  &properties[8]},
                                {.type = Property, .property =  &properties[9]},
                                {.type = Jail_pass, .property =  NULL},
                                {.type = Property, .property =  &properties[10]},
                                {.type = Property, .property =  &properties[11]},
                                {.type = Go_to_jail, .property =  NULL},
                                {.type = Property, .property =  &properties[12]},
                                {.type = Property, .property =  &properties[13]},
                                {.type = Jail_pass, .property =  NULL},
                                {.type = Property, .property =  &properties[14]},
                                {.type = Property, .property =  &properties[15]}};

/*
         * END STRUCTURES FROM monopoly.h and monopoly.c
*/


/*
         * FUNCTIONS FROM MY z4.c , SOME FUNCTIONS HAVE ADDITIONAL ARGUMENT win or stats_win , TO DO VIZUAL ACTIONS.
         *
         * WARNING, IT IS ONLY DEFENITIONS OF FUNCTIONS, REALISATIONS OF FUNCTIONS ARE AT THE BOTTOM.
         *
         * WARNING, THE VISUALISATION WAS BASED ON PROJECT monopoly_demo2.c, IT MEANS THAT IT HAS THE SAME BOARD.
         * THE IDEA OF VISUALISATION WAS COPIED FROM THIS PROJECT.
         * EVERYTHING ELSE IS MY HANDIWORK.
         *
         * WARNING, LOGIC OF GAME WAS FULLY COPIED FROM MY PROJECT z4.c, RULES HASN'T BEEN CHANGED
         *
         * WARNING FOR SUPERVISORS OF PROGRAMMING 2
         * I KNOW MY z4.c CODE IS USED IN OTHER PROJECTS
         * MONOPOLY CODE IS DONE BY ME
         * EVERYTHING WAS COPIED BY OTHER PERSONS WITH MY PERMISSION
         * PLAGIARISM WARNING
         *
         * WARNING MINIMUM CONSOLE SIZE TO CORRECT WORK IS
         * 38x38
         * CARD SIZES ARE STATIC  AND CARDS COUNT IS ALSO STATIC
         * IT'S STRICTLY RECOMMENDED TO RUN GAME IN FULLSCREEN MODE
         * PROJECT WAS TESTED WITH 148x38 CONSOLE SIZE ( FULLSCREEN MODE )
         *
         * BEFORE USING COMMANDS ENTER IN FULLSCREEN MODE
         * NCURSES SHOULD BE INSTALLED
         * USE THOSE COMMANDS TO BUILD PROJECT:
         * gcc -o monopoly z6.c -lncurses
         * monopoly
         *
*/

int getColorIndex(enum color color);

/*
         * this function helps convert color to string
         * no usages here, only in z4.c
         * here we don't need to print property types
*/

int isInJail();

/*
         * returns:
         *      1 if player is in jail
         *      0 if player isn't in jail
*/

int checkIfPLayerHasJailPass();

/*
         * returns:
         *      1 if player has more than 0 jail passes
         *      0 if player has less than 0 jail passes
*/

int isGoToJailSpace(int space_number);

/*
         * returns:
         *      1 if gameboard[space_number]  is    Go To Jail
         *      0 if gameboard[space_number]  isn't Go To Jail
*/

int isJailPassSpace(int space_number);

/*
         * returns:
         *      1 if gameboard[space_number]  is    Go To Jail
         *      0 if gameboard[space_number]  isn't Go To Jail
*/

int isPropertySpace(int space_number);

/*
         * returns:
         *      1 if gameboard[space_number]  is    Property
         *      0 if gameboard[space_number]  isn't Property
*/

int isPropertyOwned(int space_number);

/*
         * returns:
         *      1 if gameboard[space_number]  is Property and has    owner
         *      0 if gameboard[space_number]  is Property and has no owner
*/

int isEnoughCash(int property_price);

/*
         * returns:
         *      1 if player can   afford to buy this Property
         *      0 if player can't afford to buy this Property
*/

int isPropertyOwnedByEnemy(int space_number);

/*
         * returns:
         *      1 if player is on Property which is     owned by other player
         *      0 if player is on Property which isn't  owned by other player
*/

int isMonopoly(int space_number);

/*
         * returns:
         *      1 if other Property of the same color is    owned by the same player
         *      0 if other Property of the same color isn't owned by the same player
*/

int isEnoughCashToFee(int parking_fee);

/*
         * returns:
         *      1 if player can   afford to pay parking fee
         *      0 if player can't afford to pay parking fee
*/

void movePlayer(int number_on_dice, struct _win_st *win[]);

/*
         * This function move player in backend , and has additional argument win to move this player in frontend
         * Moving has a small animation it looks like piece is moving one cell at a time
*/

void payFee(int parking_fee, int space_number, struct _win_st *stats_win[]);

/*
         * This function transfers parking_fee to the owner of Property.
         * Also has stats_win as an argument, to animate it in the statistics cards.
*/

void players_init();

/*
         * This function initialize player.
         * It means that for each player sets: a certain start amount of money ,position, jail passes and other.
*/

void propertyOwnersInit();

/*
         * This function initialize property owners Array, at the start every Property has no owner.
*/

void monopoliesInit();

/*
         * This function initialize monopolies array, at the start no monopolies on the board.
*/

void checkIfNextLap(int number_on_dice, struct _win_st *stats_win[]);

/*
         * This function checks if player will end lap, and gives him 2 euros for finishing lap
         * Also has stats_win as an argument which provides animation on statistics card
*/

int hasPropertyOfSameColor(int space_number);

/*
         * this function helps to detect monopoly
         * when somebody is buying Property, this function checks if player has property of same color
*/

int checkIfInJailAndCanEscape(struct _win_st *stats_win[]);

/*
         * This function check if player has JAIL PASS or 1 cash to escape
         * Function also has stats_win as an additional argument,
         * it provides changing of cash and jail passes (with animations)
*/


void buyProperty(int space_number, struct _win_st *window[], struct _win_st *stats_win[]);

/*
         * This function buys property to player, then refreshes statistics of player such as
         * cash and properties count, then it draws on the property cell Owner: [Color]
         * to make visible owning of property
*/

void goToJail(struct _win_st **win, int locations[], int new_locations[], struct _win_st *stats_win[]);

/*
         * This function moves player from Go to Jail space to In Jail space ( 18 --> 6)
         * Then refresh statistics and provides animated moving
 */

void useJailPass(struct _win_st *stats_win[]);

/*
         * This function reduce number of jail passes ,when it is needed
         * and provides refresh of statistics of player
 */

int spaceAction(struct _win_st *win[], int locations[], int new_locations[], struct _win_st *stats_win[]);

/*
         * This is one of biggest and hard functions.
         * This function determine all kinds of actions, such as:
         * pay fee, go to jail,buy property,use jail pass, and other.
         * returns 1 if action is possible,
         * 0 if action is impossible and player loses
 */

void findWinner();

/* END OF MY z4.c FUNCTIONS */



void initNcurses(int *x, int *y);

/*
        * This function call other functions to initialize ncurses
        * such as noecho() , cbreak(); initscr(); and other
*/

void initialize();

/*
         * This function calls other initialize functions
*/

void clear_refresh();

/*
         * clear(); and refresh(); from ncurses in one function
*/

void exit_game(int maxx, int maxy);

/*
        * This function provides smooth exiting, after execution of exit button
*/

struct _win_st *create_property_card(int starty, int startx, SPACE game_space);

/*
         * This function creates Spaces for game board, cards consist of
         * name,color,price.
*/

void drawGameBoard(struct _win_st **win, SPACE *gameboard, int maxy, int maxx);

/*
        * This function determines position of property card ,then creates it and show on gameboard
*/

void drawPlayer(struct _win_st **win, int act_location, int new_location, int player_number);

/*
        * This function relocates player from act_location to new_location with smooth animation
*/

void init_custom_color_pair();

/*
         * This function initialize all colors which are used in monopoly.
*/

int getCubeNumber(int lower, int upper);

/*
         * Function from monopoly_demo2
*/

int genNumDigits(int number);

/*
         * Function from monopoly_demo2
         * This function is used to draw gameboard, counts digits in number
*/

int get_new_location(int actual_location, int dice_roll);

/*
         * Function from monopoly_demo2
         * Function computes new position.
*/

int menu(int x, int y);

/*
         * This function shows menu after executing program.
         * Then function returns entered option ( NEW GAME , SETTINGS, EXIT)
*/

int settings_menu(int x, int y);

/*
         * This function shows setting menu.
         * Then function returns entered option ( NEW GAME , SETTINGS, EXIT)
*/

void checkColors();

/*
         * Function from monopoly_demo2, check if terminal supports all used colors
*/

void init_locations(int locations[], int new_locations[]);

/*
        * This function set up locations and new locations for all players.
*/

struct _win_st *create_statistics_card(int starty, int startx, PLAYER player);

/*
         * This function creates a window.
         * This window will represent statistics of each player.
*/

void draw_statistics(struct _win_st **win, int maxx, int maxy);

/*
         * This function call creating of statistics cards and
         * determine the start coordinates of each card.
*/

void refresh_cash(struct _win_st *stats_win[]);

/*
        * This function provides smooth animation of CASH field in statistics card
*/

void refresh_spacenumber(struct _win_st *stats_win[]);

/*
        * This function provides smooth animation of POSITION field in statistics card
*/

void refresh_num_jail_passes(struct _win_st **stats_win);

/*
        * This function provides smooth animation of JAIL PASSES field in statistics card
*/

void refresh_num_properties(struct _win_st **stats_win);

/*
        * This function provides smooth animation of NUMBER OF PROPERTIES field in statistics card
*/

static int PLAYERS_COUNT = 2;

static int TURN = 0;

static PLAYER players[MAX_PLAYERS] = {0};

static int propertyOwners[NUM_SPACES];
// -1 No owner , 0,1,2,3  - owner

static int monopolies[NUM_COLORS];
//Brown, Skyblue, Purple, Orange, Red, Yellow, Green, Blue
// 0:no , 1:yes;
static int impossible_winner = 0;
static int winner = -1;
// -1 :no winner
// -2 :draw




int main(void) {

    struct _win_st *win[NUM_SPACES];

    int maxy, maxx;
    initNcurses(&maxx, &maxy);

    show_menu:
    int choice = menu(maxx / 2, maxy / 2);

    if (choice == 0) {

        int locations[PLAYERS_COUNT];
        int new_locations[PLAYERS_COUNT];

        struct _win_st *stats_win[PLAYERS_COUNT];

        clear_refresh();

        init_locations(locations, new_locations);
        initialize();

        drawGameBoard(win, game_board, maxy, maxx);
        draw_statistics(stats_win, maxx, maxy);

        int c = 0;
        int dice_roll = 0;

        for (int i = 0; i < PLAYERS_COUNT; i++) { drawPlayer(win, locations[TURN], new_locations[TURN], i); }
        mvprintw(0, 0, "Press Esc to return to the main menu");
        while (1) {
            if (TURN == 0) {
                nodelay(stdscr, TRUE);
                for (;;) {
                    mvprintw(maxy / 2, (maxx - strlen("Press C to roll the dice")) / 2, "Press C to roll the dice");
                    refresh();
                    cross_sleep(1000);

                    c = getch();
                    if (c == 'c') {
                        dice_roll = getCubeNumber(1, 6);
                        mvprintw(maxy / 2, (maxx - strlen("Press C to roll the dice")) / 2, "                        ");
                        refresh();
                        break;
                    } else if (c == 27) {
                        clear_refresh();
                        goto show_menu;
                    }
                    mvprintw(maxy / 2, (maxx - strlen("Press C to roll the dice")) / 2, "                        ");
                    refresh();
                    cross_sleep(700);
                    c = getch();
                    if (c == 'c') {
                        dice_roll = getCubeNumber(1, 6);
                        mvprintw(maxy / 2, (maxx - strlen("Press C to roll the dice")) / 2, "                        ");
                        refresh();
                        break;
                    } else if (c == 27) {
                        clear_refresh();
                        goto show_menu;
                    }

                }
            } else {
                dice_roll = getCubeNumber(1, 6);
                cross_sleep(1250);
            }
            const char *dice_roll_message = "Dice roll is ";
            char result_message[50];
            sprintf(result_message, "%s%d", dice_roll_message, dice_roll);
            for (int i = 0; i < strlen(result_message); i++) {
                mvaddch(maxy / 2 - 1, (maxx - strlen(result_message)) / 2 + i, ' ');
                refresh();
                cross_sleep(40);
            }
            for (int i = 0, counter = 0; i < strlen(result_message); i++) {
                if (i + 1 == strlen(result_message) && counter <= 8) {
                    mvaddch(maxy / 2 - 1, (maxx - strlen(result_message)) / 2 + i, getCubeNumber(1, 6) + '0');
                    refresh();
                    cross_sleep(150);
                    counter++;
                    i--;
                }
                mvaddch(maxy / 2 - 1, (maxx - strlen(result_message)) / 2 + i, result_message[i]);
                refresh();
                cross_sleep(40);
            }
            cross_sleep(700);
            nodelay(stdscr, FALSE);
            new_locations[TURN] = get_new_location(locations[TURN], dice_roll);
            drawPlayer(win, locations[TURN], new_locations[TURN], TURN);
            locations[TURN] = new_locations[TURN];
            if (checkIfInJailAndCanEscape(stats_win) == 0) {
                impossible_winner = TURN;
                break;
            }
            checkIfNextLap(dice_roll, stats_win);
            movePlayer(dice_roll, stats_win);
            if (!spaceAction(win, locations, new_locations, stats_win)) {
                impossible_winner = TURN;
                break;
            }
            TURN = (TURN + 1) % PLAYERS_COUNT;
            /*
                debug print
                mvprintw(maxy-1, 0,"old_loc: %2d  new_loc: %2d", locations[TURN], new_locations[TURN]);
            */
        }
        clear_refresh();
        draw_statistics(stats_win, maxx, maxy);
        refresh();
        findWinner();
        if (winner == -1 || winner == -2) {
            attron(COLOR_YELLOW);
            mvprintw(maxy / 2, (maxx - strlen("There is no winner in this game...")) / 2,
                     "There is no winner in this game...");
            attroff(COLOR_YELLOW);
        } else {
            attron(COLOR_YELLOW);
            mvprintw(maxy / 2, (maxx - strlen("Winner is PLAYER ")) / 2, "Winner is PLAYER%d", winner + 1);
            attroff(COLOR_YELLOW);
        }
        refresh();
        nodelay(stdscr, FALSE);
        cross_sleep(8000);
        clear_refresh();
        goto show_menu;
    } else if (choice == 1) {
        PLAYERS_COUNT = settings_menu(maxx / 2, maxy / 2);
        goto show_menu;
    } else if (choice == 2) {
        clear_refresh();
        exit_game(maxx, maxy);
    }
    endwin();
    return 0;
}

void drawPlayer(struct _win_st **win, int act_location, int new_location, int player_number) {
    if (act_location == new_location) {
        wattron(win[act_location], COLOR_PAIR(COL_NOPLAYER));
        mvwprintw(win[act_location], HEIGHT_CARD - 2, 2 + 3 * player_number, "  ");
        wattroff(win[act_location], COLOR_PAIR(COL_NOPLAYER));
        if (player_number == 0) {
            wattron(win[new_location], COLOR_PAIR(COL_PLAYER1));
            mvwprintw(win[new_location], HEIGHT_CARD - 2, 2 + 3 * player_number, ":)");
            wattroff(win[new_location], COLOR_PAIR(COL_PLAYER1));
        } else if (player_number == 1) {
            wattron(win[new_location], COLOR_PAIR(COL_PLAYER2));
            mvwprintw(win[new_location], HEIGHT_CARD - 2, 2 + 3 * player_number, ":)");
            wattroff(win[new_location], COLOR_PAIR(COL_PLAYER2));
        } else if (player_number == 2) {
            wattron(win[new_location], COLOR_PAIR(COL_PLAYER3));
            mvwprintw(win[new_location], HEIGHT_CARD - 2, 2 + 3 * player_number, ":)");
            wattroff(win[new_location], COLOR_PAIR(COL_PLAYER3));
        } else if (player_number == 3) {
            wattron(win[new_location], COLOR_PAIR(COL_PLAYER4));
            mvwprintw(win[new_location], HEIGHT_CARD - 2, 2 + 3 * player_number, ":)");
            wattroff(win[new_location], COLOR_PAIR(COL_PLAYER4));
        }
        wrefresh(win[act_location]);
        wrefresh(win[new_location]);
    } else if (act_location == 18 && new_location == 6) {
        cross_sleep(1200);
        for (int i = act_location; i > new_location; i--) {
            wattron(win[i], COLOR_PAIR(COL_NOPLAYER));
            mvwprintw(win[i], HEIGHT_CARD - 2, 2 + 3 * player_number, "  ");
            wattroff(win[i], COLOR_PAIR(COL_NOPLAYER));
            if (player_number == 0) {
                wattron(win[i - 1], COLOR_PAIR(COL_PLAYER1));
                mvwprintw(win[i - 1], HEIGHT_CARD - 2, 2 + 3 * player_number, ":)");
                wattroff(win[i - 1], COLOR_PAIR(COL_PLAYER1));
            } else if (player_number == 1) {
                wattron(win[i - 1], COLOR_PAIR(COL_PLAYER2));
                mvwprintw(win[i - 1], HEIGHT_CARD - 2, 2 + 3 * player_number, ":)");
                wattroff(win[i - 1], COLOR_PAIR(COL_PLAYER2));
            } else if (player_number == 2) {
                wattron(win[i - 1], COLOR_PAIR(COL_PLAYER3));
                mvwprintw(win[i - 1], HEIGHT_CARD - 2, 2 + 3 * player_number, ":)");
                wattroff(win[i - 1], COLOR_PAIR(COL_PLAYER3));
            } else if (player_number == 3) {
                wattron(win[i - 1], COLOR_PAIR(COL_PLAYER4));
                mvwprintw(win[i - 1], HEIGHT_CARD - 2, 2 + 3 * player_number, ":)");
                wattroff(win[i - 1], COLOR_PAIR(COL_PLAYER4));
            }
            wrefresh(win[i - 1]);
            wrefresh(win[i]);
            cross_sleep(125);
        }
    } else if (new_location < act_location) {
        int diff = (new_location + NUM_SPACES) - act_location;
        for (int i = (act_location + 1) % NUM_SPACES, counter = 0;
             counter < diff; i = (i + 1) % NUM_SPACES, counter++) {
            if (i == 0) {
                wattron(win[23], COLOR_PAIR(COL_NOPLAYER));
                mvwprintw(win[23], HEIGHT_CARD - 2, 2 + 3 * player_number, "  ");
                wattroff(win[23], COLOR_PAIR(COL_NOPLAYER));
            } else {
                wattron(win[i - 1], COLOR_PAIR(COL_NOPLAYER));
                mvwprintw(win[i - 1], HEIGHT_CARD - 2, 2 + 3 * player_number, "  ");
                wattroff(win[i - 1], COLOR_PAIR(COL_NOPLAYER));
            }

            if (player_number == 0) {
                wattron(win[i], COLOR_PAIR(COL_PLAYER1));
                mvwprintw(win[i], HEIGHT_CARD - 2, 2 + 3 * player_number, ":)");
                wattroff(win[i], COLOR_PAIR(COL_PLAYER1));
            } else if (player_number == 1) {
                wattron(win[i], COLOR_PAIR(COL_PLAYER2));
                mvwprintw(win[i], HEIGHT_CARD - 2, 2 + 3 * player_number, ":)");
                wattroff(win[i], COLOR_PAIR(COL_PLAYER2));
            } else if (player_number == 2) {
                wattron(win[i], COLOR_PAIR(COL_PLAYER3));
                mvwprintw(win[i], HEIGHT_CARD - 2, 2 + 3 * player_number, ":)");
                wattroff(win[i], COLOR_PAIR(COL_PLAYER3));
            } else if (player_number == 3) {
                wattron(win[i], COLOR_PAIR(COL_PLAYER4));
                mvwprintw(win[i], HEIGHT_CARD - 2, 2 + 3 * player_number, ":)");
                wattroff(win[i], COLOR_PAIR(COL_PLAYER4));
            }
            if (i == 0) {
                wrefresh(win[23]);
            } else {
                wrefresh(win[i - 1]);
            }
            wrefresh(win[i]);
            cross_sleep(250);
        }
    } else {
        for (int i = act_location + 1; i <= new_location; i++) {
            wattron(win[i - 1], COLOR_PAIR(COL_NOPLAYER));
            mvwprintw(win[i - 1], HEIGHT_CARD - 2, 2 + 3 * player_number, "  ");
            wattroff(win[i - 1], COLOR_PAIR(COL_NOPLAYER));
            if (player_number == 0) {
                wattron(win[i], COLOR_PAIR(COL_PLAYER1));
                mvwprintw(win[i], HEIGHT_CARD - 2, 2 + 3 * player_number, ":)");
                wattroff(win[i], COLOR_PAIR(COL_PLAYER1));
            } else if (player_number == 1) {
                wattron(win[i], COLOR_PAIR(COL_PLAYER2));
                mvwprintw(win[i], HEIGHT_CARD - 2, 2 + 3 * player_number, ":)");
                wattroff(win[i], COLOR_PAIR(COL_PLAYER2));
            } else if (player_number == 2) {
                wattron(win[i], COLOR_PAIR(COL_PLAYER3));
                mvwprintw(win[i], HEIGHT_CARD - 2, 2 + 3 * player_number, ":)");
                wattroff(win[i], COLOR_PAIR(COL_PLAYER3));
            } else if (player_number == 3) {
                wattron(win[i], COLOR_PAIR(COL_PLAYER4));
                mvwprintw(win[i], HEIGHT_CARD - 2, 2 + 3 * player_number, ":)");
                wattroff(win[i], COLOR_PAIR(COL_PLAYER4));
            }
            wrefresh(win[i - 1]);
            wrefresh(win[i]);
            cross_sleep(250);
        }
    }

}

struct _win_st *create_property_card(int starty, int startx, SPACE game_space) {

    struct _win_st *local_win;

    local_win = newwin(HEIGHT_CARD, WIDTH_CARD, starty, startx);

    box(local_win, ACS_VLINE, ACS_HLINE);

    mvwhline(local_win, HEIGHT_LABEL_CARD, 1, ACS_HLINE, WIDTH_CARD - 1);
    mvwaddch(local_win, HEIGHT_LABEL_CARD, 0, ACS_LTEE);
    mvwaddch(local_win, HEIGHT_LABEL_CARD, WIDTH_CARD - 1, ACS_RTEE);

    if (game_space.type == Property) {
        wattron(local_win, COLOR_PAIR(game_space.property->color + 1));
        for (int y = 1; y < HEIGHT_LABEL_CARD; y++) {
            for (int x = 1; x < WIDTH_CARD - 1; x++) {
                mvwprintw(local_win, y, x, " ");
            }
        }
    } else {
        wattron(local_win, COLOR_PAIR(COL_LABEL_NOCOLOR));
        for (int y = 1; y < HEIGHT_LABEL_CARD; y++) {
            for (int x = 1; x < WIDTH_CARD - 1; x++) {
                mvwprintw(local_win, y, x, " ");
            }
        }
    }

    if (game_space.type == Property) {
        mvwprintw(local_win, (HEIGHT_LABEL_CARD / 2),
                  (WIDTH_CARD - strlen(property_colors[game_space.property->color])) / 2, "%s",
                  property_colors[game_space.property->color]);

        wattroff(local_win, COLOR_PAIR(game_space.property->color + 16));
        mvwprintw(local_win, (HEIGHT_LABEL_CARD + 1), (WIDTH_CARD - strlen(game_space.property->name)) / 2, "%s",
                  game_space.property->name);
        mvwprintw(local_win, (HEIGHT_LABEL_CARD + 3), (WIDTH_CARD - genNumDigits(game_space.property->price)) / 2, "%d",
                  game_space.property->price);
    } else {
        mvwprintw(local_win, (HEIGHT_LABEL_CARD / 2), (WIDTH_CARD - strlen(space_types[game_space.type])) / 2, "%s",
                  space_types[game_space.type]);
        wattroff(local_win, COLOR_PAIR(COL_LABEL_NOCOLOR));
    }
    wrefresh(local_win);

    return local_win;
}

void drawGameBoard(struct _win_st **win, SPACE *gameboard, int maxy, int maxx) {
    char cost[3] = {'\0'};
    int n = 0;
    for (int i = 0; i < NUM_SPACES; i++) {
        sprintf(cost, "%d", i);
        if (i >= 0 && i < NUM_CARD_IN_ROW) {
            win[i] = create_property_card(maxy - HEIGHT_CARD, maxx - WIDTH_CARD * (n + 1), gameboard[i]);
            n++;
            if (n >= NUM_CARD_IN_ROW) {
                n = 0;
            }
        }
        if (i >= 1 * NUM_CARD_IN_ROW && i < 2 * NUM_CARD_IN_ROW) {
            win[i] = create_property_card(maxy - HEIGHT_CARD * (n + 1), maxx - WIDTH_CARD * (NUM_CARD_IN_ROW + 1),
                                          gameboard[i]);
            n++;
            if (n >= NUM_CARD_IN_ROW) {
                n = NUM_CARD_IN_ROW;
            }
        }

        if (i >= 2 * NUM_CARD_IN_ROW && i < 3 * NUM_CARD_IN_ROW) {
            win[i] = create_property_card(maxy - HEIGHT_CARD * (NUM_CARD_IN_ROW + 1), maxx - WIDTH_CARD * (n + 1),
                                          gameboard[i]);
            n--;
            if (n <= 0) {
                n = NUM_CARD_IN_ROW;
            }
        }
        if (i >= 3 * NUM_CARD_IN_ROW && i < 4 * NUM_CARD_IN_ROW) {
            win[i] = create_property_card(maxy - HEIGHT_CARD * (n + 1), maxx - WIDTH_CARD, gameboard[i]);
            n--;
            if (n <= 0) {
                n = NUM_CARD_IN_ROW;
            }
        }
        wrefresh(win[i]);
        refresh();
        cross_sleep(10);
    }
}

void init_custom_color_pair() {
    int r = 4;
    start_color();

    init_color(COL_BROWN, 139 * r, 69 * r, 19 * r);
    init_color(COL_SKYBLUE, 65 * r, 105 * r, 225 * r);
    init_color(COL_PURPLE, 128 * r, 128 * r, 0 * r);
    init_color(COL_ORANGE, 250 * r, 128 * r, 0 * r);
    init_color(COL_RED, 249 * r, 0 * r, 0 * r);
    init_color(COL_YELLOW, 249 * r, 249 * r, 0 * r);
    init_color(COL_GREEN, 0 * r, 100 * r, 0 * r);
    init_color(COL_BLUE, 0 * r, 0 * r, 249 * r);

    init_pair(COL_LABEL_BROWN, COLOR_WHITE, COL_BROWN);
    init_pair(COL_LABEL_SKYBLUE, COLOR_WHITE, COL_SKYBLUE);
    init_pair(COL_LABEL_PURPLE, COLOR_WHITE, COL_PURPLE);
    init_pair(COL_LABEL_ORANGE, COLOR_WHITE, COL_ORANGE);
    init_pair(COL_LABEL_RED, COLOR_WHITE, COL_RED);
    init_pair(COL_LABEL_YELLOW, COLOR_BLACK, COL_YELLOW);
    init_pair(COL_LABEL_GREEN, COLOR_WHITE, COL_GREEN);
    init_pair(COL_LABEL_BLUE, COLOR_WHITE, COL_BLUE);

    init_pair(COL_LABEL_NOCOLOR, COLOR_WHITE, COLOR_BLACK);
    init_pair(COL_PLAYER1, COLOR_BLACK, COLOR_MAGENTA);
    init_pair(COL_PLAYER2, COLOR_BLACK, COLOR_YELLOW);
    init_pair(COL_PLAYER3, COLOR_BLACK, COLOR_CYAN);
    init_pair(COL_PLAYER4, COLOR_BLACK, COLOR_RED);
    init_pair(COL_NOPLAYER, COLOR_BLACK, COLOR_BLACK);
}

int get_new_location(int actual_location, int dice_roll) {
    if (actual_location + dice_roll >= NUM_SPACES) {
        return actual_location + dice_roll - NUM_SPACES;
    }
    return actual_location + dice_roll;
}

int genNumDigits(int number) {
    int count = 0;

    while (number != 0) {
        number = number / 10;
        count++;
    }
    return count;
}

int getCubeNumber(int lower, int upper) {
    int num = (rand() % (upper - lower + 1)) + lower;
    return num;
}

int menu(int x, int y) {

    char *choices[] = {"New Game", "Settings", "Exit"};
    int numChoices = sizeof(choices) / sizeof(char *);
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLUE);
    init_pair(2, COLOR_BLACK, COLOR_WHITE);
    int width = 20;
    int height = numChoices + 2;

    int startX = x - width / 2;
    int startY = y - height / 2;

    WINDOW *menuWin = newwin(height, width, startY, startX);
    box(menuWin, 0, 0);
    wrefresh(menuWin);
    refresh();

    int choice;
    int highlight = 0;

    while (1) {
        for (int i = 0; i < numChoices; i++) {
            if (i == highlight) {
                wattron(menuWin, COLOR_PAIR(2));
                mvwprintw(menuWin, i + 1, 1, "%s", choices[i]);
                wattroff(menuWin, COLOR_PAIR(2));
            } else {
                wattron(menuWin, COLOR_PAIR(1));
                mvwprintw(menuWin, i + 1, 1, "%s", choices[i]);
                wattroff(menuWin, COLOR_PAIR(1));
            }
        }
        wrefresh(menuWin);

        choice = getch();

        switch (choice) {
            case KEY_UP:
                highlight--;
                if (highlight < 0)
                    highlight = numChoices - 1;
                break;
            case KEY_DOWN:
                highlight++;
                if (highlight >= numChoices)
                    highlight = 0;
                break;
            case 10:
                delwin(menuWin);
                endwin();
                goto exit;
            default:
                break;
        }
    }
    exit:
    delwin(menuWin);
    endwin();
    return highlight;
}

int getColorIndex(enum color color) { return color; }

int isInJail() { return players[TURN].is_in_jail == 1; }

int checkIfPLayerHasJailPass() { return players[TURN].num_jail_pass > 0; }

int isGoToJailSpace(int space_number) { return game_board[space_number].type == Go_to_jail; }

int isJailPassSpace(int space_number) { return game_board[space_number].type == Jail_pass; }

int isPropertySpace(int space_number) { return game_board[space_number].type == Property; }

int isPropertyOwned(int space_number) { return propertyOwners[space_number] == -1; }

int isEnoughCash(int property_price) { return players[TURN].cash - property_price >= 0; }

int isPropertyOwnedByEnemy(int space_number) {
    return propertyOwners[space_number] != -1 && propertyOwners[space_number] != TURN;
}

int isMonopoly(int space_number) {
    return monopolies[getColorIndex(game_board[space_number].property->color)] == 1;
}

int isEnoughCashToFee(int parking_fee) {
    return players[TURN].cash - parking_fee >= 0;
}

void movePlayer(int number_on_dice, struct _win_st *win[]) {
    players[TURN].space_number = (players[TURN].space_number + number_on_dice) % 24;
    refresh_spacenumber(win);
}

void initialize() {
    players_init();
    propertyOwnersInit();
    monopoliesInit();
}

void findWinner() {
    int sum_prices[PLAYERS_COUNT];
    for (int i = 0; i < PLAYERS_COUNT; i++) {
        sum_prices[i] = 0;
    }
    for (int i = 0; i < PLAYERS_COUNT; i++) {
        if (i == impossible_winner) { continue; }
        sum_prices[i] = players[i].cash;
    }
    int max = sum_prices[0];
    int max_count = 1;
    int max_index = 0;
    for (int i = 1; i < PLAYERS_COUNT; i++) {
        if (i == impossible_winner) { continue; }
        if (sum_prices[i] > max) {
            max = sum_prices[i];
            max_count = 1;
            max_index = i;
        } else if (sum_prices[i] == max) {
            max_count++;
        }
    }
    if (max_count == 1) {
        winner = max_index;
        return;
    }
    for (int i = 0; i < PLAYERS_COUNT; i++) {
        if (i == impossible_winner) { continue; }
        if (sum_prices[i] == max) {
            for (int j = 0; j < players[i].num_properties; j++) {
                sum_prices[i] += players[i].owned_properties[j]->price;
            }
        }
    }
    max = sum_prices[0];
    max_count = 1;
    max_index = 0;
    for (int i = 1; i < PLAYERS_COUNT; i++) {
        if (sum_prices[i] > max) {
            max = sum_prices[i];
            max_count = 1;
            max_index = i;
        } else if (sum_prices[i] == max) {
            max_count++;
        }
    }
    if (max_count != 2) {
        winner = max_index;
        return;
    }

    winner = -2;
}

int spaceAction(struct _win_st *win[], int locations[], int new_locations[], struct _win_st *stats_win[]) {
    int space_number = players[TURN].space_number;
    if (isGoToJailSpace(space_number)) {
        if (checkIfPLayerHasJailPass()) {
            useJailPass(stats_win);
        } else {
            goToJail(win, locations, new_locations, stats_win);
        }
    }
    if (isJailPassSpace(space_number)) {
        players[TURN].num_jail_pass++;
        refresh_num_jail_passes(stats_win);
    }
    if (isPropertySpace(space_number)) {
        if (isPropertyOwnedByEnemy(space_number)) {
            int parking_fee = (game_board[space_number].property->price) * (isMonopoly(space_number) ? 2 : 1);
            if (isEnoughCashToFee(parking_fee)) {
                payFee(parking_fee, space_number, stats_win);
            } else {
                players[TURN].cash -= parking_fee;
                refresh_cash(stats_win);
                return 0;
            }
        } else if (isPropertyOwned(space_number)) {
            if (isEnoughCash(game_board[space_number].property->price)) {
                buyProperty(space_number, win, stats_win);
            } else {
                players[TURN].cash -= game_board[space_number].property->price;
                refresh_cash(stats_win);
                return 0;
            }
        }
    }
    return 1;
}

void useJailPass(struct _win_st *stats_win[]) {
    players[TURN].num_jail_pass--;
    refresh_num_jail_passes(stats_win);
}

void goToJail(struct _win_st **win, int locations[], int new_locations[], struct _win_st *stats_win[]) {
    new_locations[TURN] = 6;
    drawPlayer(win, locations[TURN], new_locations[TURN], TURN);
    locations[TURN] = new_locations[TURN];
    players[TURN].space_number = 6;
    refresh_spacenumber(stats_win);
    players[TURN].is_in_jail = 1;
}

void buyProperty(int space_number, struct _win_st *window[], struct _win_st *stats_win[]) {
    players[TURN].owned_properties[players[TURN].num_properties] = game_board[space_number].property;
    players[TURN].cash -= game_board[space_number].property->price;
    refresh_cash(stats_win);
    propertyOwners[space_number] = TURN;
    players[TURN].num_properties++;
    refresh_num_properties(stats_win);
    char string[] = " Owner: ";
    for (int i = 0; i < strlen(string); i++) {
        mvwaddch(window[space_number], HEIGHT_LABEL_CARD, 5 + i, string[i]);
        wrefresh(window[space_number]);
        cross_sleep(40);

    }
    if (TURN == 0) {
        wattron(window[space_number], COLOR_PAIR(COL_PLAYER1));
        mvwprintw(window[space_number], HEIGHT_LABEL_CARD, 13, "  ");
        wattroff(window[space_number], COLOR_PAIR(COL_PLAYER1));
    } else if (TURN == 1) {
        wattron(window[space_number], COLOR_PAIR(COL_PLAYER2));
        mvwprintw(window[space_number], HEIGHT_LABEL_CARD, 13, "  ");
        wattroff(window[space_number], COLOR_PAIR(COL_PLAYER2));
    } else if (TURN == 2) {
        wattron(window[space_number], COLOR_PAIR(COL_PLAYER3));
        mvwprintw(window[space_number], HEIGHT_LABEL_CARD, 13, "  ");
        wattroff(window[space_number], COLOR_PAIR(COL_PLAYER3));
    } else if (TURN == 3) {
        wattron(window[space_number], COLOR_PAIR(COL_PLAYER4));
        mvwprintw(window[space_number], HEIGHT_LABEL_CARD, 13, "  ");
        wattroff(window[space_number], COLOR_PAIR(COL_PLAYER4));
    }
    mvwprintw(window[space_number], HEIGHT_LABEL_CARD, 15, " ");
    wrefresh(window[space_number]);
    if (hasPropertyOfSameColor(space_number)) {
        monopolies[getColorIndex(game_board[space_number].property->color)] = 1;
    }

}

int checkIfInJailAndCanEscape(struct _win_st *stats_win[]) {
    if (isInJail()) {
        if (players[TURN].cash - 1 < 0) {
            players[TURN].cash -= 1;
            refresh_cash(stats_win);
            return 0;
        }
        players[TURN].cash -= 1;
        refresh_cash(stats_win);
        players[TURN].is_in_jail = 0;
    }
    return 1;
}

void payFee(int parking_fee, int space_number, struct _win_st *stats_win[]) {
    players[TURN].cash -= parking_fee;
    refresh_cash(stats_win);
    int temp = TURN;
    players[propertyOwners[space_number]].cash += parking_fee;
    TURN = propertyOwners[space_number];
    refresh_cash(stats_win);
    TURN = temp;

}

void players_init() {
    int cash = TOTAL_SPACES - (INITIAL_CASH_MULTIPLIER * PLAYERS_COUNT);
    for (int i = 0; i < PLAYERS_COUNT; i++) {
        struct player player;
        player.number = i + 1;
        player.space_number = 0;
        player.cash = cash;
        player.num_jail_pass = 0;
        player.is_in_jail = 0;
        player.num_properties = 0;
        for (int j = 0; j < NUM_PROPERTIES; j++) {
            player.owned_properties[j] = NULL;
        }
        players[i] = player;
    }
}

void propertyOwnersInit() {
    for (int i = 0; i < NUM_SPACES; i++) {
        propertyOwners[i] = -1;
    }
}

void monopoliesInit() {
    for (int i = 0; i < NUM_COLORS; i++) {
        monopolies[i] = 0;
    }
}

int hasPropertyOfSameColor(int space_number) {
    int colorIndex = getColorIndex(game_board[space_number].property->color);
    for (int i = 0; i < players[TURN].num_properties - 1; i++) {
        if (getColorIndex(players[TURN].owned_properties[i]->color) == colorIndex) {
            return 1;
        }
    }
    return 0;

}

void checkIfNextLap(int number_on_dice, struct _win_st *stats_win[]) {
    if (players[TURN].space_number + number_on_dice >= 24) {
        players[TURN].cash += 2;
        refresh_cash(stats_win);
    }
}

void checkColors() {
    if (has_colors() == FALSE) {
        endwin();
        printf("Your terminal does not support color\n");
        exit(1);
    }
}

void init_locations(int locations[], int new_locations[]) {
    for (int i = 0; i < PLAYERS_COUNT; i++) {
        locations[i] = 0;
    }
    for (int i = 0; i < PLAYERS_COUNT; i++) {
        new_locations[i] = 0;
    }
}

int settings_menu(int x, int y) {
    // Define settings options
    char *choices[] = {"2 Players", "3 Players", "4 Players"};
    int numChoices = sizeof(choices) / sizeof(char *);

    // Initialize ncurses and color pairs
    initscr();
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLUE);
    init_pair(2, COLOR_BLACK, COLOR_WHITE);

    // Calculate menu dimensions and position
    int width = 30;
    int height = numChoices + 2;
    int startX = x - width / 2;
    int startY = y - height / 2;

    // Create window for the menu
    WINDOW *menuWin = newwin(height, width, startY, startX);
    box(menuWin, 0, 0); // Draw box around menu window
    wrefresh(menuWin); // Refresh the menu window
    refresh();

    int choice;
    int highlight = 0;

    while (1) {
        // Display menu options with styling
        for (int i = 0; i < numChoices; i++) {
            if (i == PLAYERS_COUNT - 2) {
                wattron(menuWin, COLOR_PAIR(3)); // Activate default color
                mvwprintw(menuWin, i + 1, 1, "%s", choices[i]); // Print default option
                wattroff(menuWin, COLOR_PAIR(3)); // Deactivate default color
            } else if (i == highlight) {
                wattron(menuWin, COLOR_PAIR(2)); // Activate highlight color
                mvwprintw(menuWin, i + 1, 1, "%s", choices[i]); // Print highlighted option
                wattroff(menuWin, COLOR_PAIR(2)); // Deactivate highlight color
            } else {
                wattron(menuWin, COLOR_PAIR(1)); // Activate default color
                mvwprintw(menuWin, i + 1, 1, "%s", choices[i]); // Print default option
                wattroff(menuWin, COLOR_PAIR(1)); // Deactivate default color
            }

        }
        wrefresh(menuWin); // Refresh the menu window

        choice = getch(); // Get user input

        switch (choice) {
            case KEY_UP:
                highlight--;
                if (highlight == PLAYERS_COUNT - 2) {
                    highlight--;
                    highlight %= numChoices;
                }
                if (highlight < 0) {
                    highlight = numChoices - 1;

                }
                if (highlight == PLAYERS_COUNT - 2) {
                    highlight--;
                    highlight %= numChoices;
                }
                break;
            case KEY_DOWN:
                highlight++;
                if (highlight == PLAYERS_COUNT - 2) {
                    highlight++;
                    highlight %= numChoices;

                }
                if (highlight >= numChoices) {
                    highlight = 0;

                }
                if (highlight == PLAYERS_COUNT - 2) {
                    highlight++;
                    highlight %= numChoices;

                }
                break;
            case 10: // Enter key pressed
                clear();
                delwin(menuWin); // Delete menu window
                refresh();
                endwin(); // End ncurses mode
                return highlight + 2; // Translate menu index to number of players (2, 3, or 4)
            default:
                break;
        }
    }
}

void initNcurses(int *x, int *y) {
    srand(time(NULL));
    initscr();
    noecho();
    cbreak();
    curs_set(0);
    keypad(stdscr, TRUE);
    getmaxyx(stdscr, *y, *x);
    checkColors();
    init_custom_color_pair();
    refresh();
}


void clear_refresh() {
    clear();
    refresh();
}

void exit_game(int maxx, int maxy) {
    for (int i = 3; i > 0; i--) {
        mvprintw(maxy / 2, maxx / 2, "Exiting %d", i);
        refresh();
        cross_sleep(1000);
    }
}

struct _win_st *create_statistics_card(int starty, int startx, PLAYER player) {
    struct _win_st *local_win;

    local_win = newwin(HEIGHT_STATISTICS_CARD, WIDTH_STATISTICS_CARD, starty, startx);
    int player_number = player.number - 1;
    box(local_win, ACS_VLINE, ACS_HLINE);
    mvwhline(local_win, HEIGHT_LABEL_CARD, 1, ACS_HLINE, WIDTH_STATISTICS_CARD - 1);
    mvwaddch(local_win, HEIGHT_LABEL_CARD, 0, ACS_LTEE);
    mvwaddch(local_win, HEIGHT_LABEL_CARD, WIDTH_STATISTICS_CARD - 1, ACS_RTEE);
    if (player_number == 0) {
        wattron(local_win, COLOR_PAIR(COL_PLAYER1));
        mvwprintw(local_win, HEIGHT_LABEL_CARD - 1, (WIDTH_STATISTICS_CARD - 8) / 2, "PLAYER %d", player.number);
        wattroff(local_win, COLOR_PAIR(COL_PLAYER1));
    } else if (player_number == 1) {
        wattron(local_win, COLOR_PAIR(COL_PLAYER2));
        mvwprintw(local_win, HEIGHT_LABEL_CARD - 1, (WIDTH_STATISTICS_CARD - 8) / 2, "PLAYER %d", player.number);
        wattroff(local_win, COLOR_PAIR(COL_PLAYER2));
    } else if (player_number == 2) {
        wattron(local_win, COLOR_PAIR(COL_PLAYER3));
        mvwprintw(local_win, HEIGHT_LABEL_CARD - 1, (WIDTH_STATISTICS_CARD - 8) / 2, "PLAYER %d", player.number);
        wattroff(local_win, COLOR_PAIR(COL_PLAYER3));
    } else if (player_number == 3) {
        wattron(local_win, COLOR_PAIR(COL_PLAYER4));
        mvwprintw(local_win, HEIGHT_LABEL_CARD - 1, (WIDTH_STATISTICS_CARD - 8) / 2, "PLAYER %d", player.number);
        wattroff(local_win, COLOR_PAIR(COL_PLAYER4));
    }
    wrefresh(local_win);

    mvwprintw(local_win, HEIGHT_LABEL_CARD + 1, 2, "CASH: %d", player.cash);
    mvwprintw(local_win, HEIGHT_LABEL_CARD + 3, 2, "JAIL PASSES: %d", player.num_jail_pass);
    mvwprintw(local_win, HEIGHT_LABEL_CARD + 5, 2, "OWNED PROPERTIES: %d", player.num_properties);
    mvwprintw(local_win, HEIGHT_LABEL_CARD + 7, 2, "POSITION: %d", player.space_number + 1);
    wrefresh(local_win);
    return local_win;
}

void draw_statistics(struct _win_st **win, int maxx, int maxy) {
    for (int i = 0; i < PLAYERS_COUNT; i++) {
        int start_x = (maxx - (WIDTH_CARD * 6) + ((WIDTH_STATISTICS_CARD) * (i)));
        int start_y = maxy - ((HEIGHT_CARD + 2) + HEIGHT_STATISTICS_CARD);
        win[i] = create_statistics_card(start_y, start_x, players[i]);
    }
}

void refresh_spacenumber(struct _win_st *stats_win[]) {
    const char string[] = "POSITION: ";
    for (int i = 0; i < WIDTH_STATISTICS_CARD - 3; i++) {
        mvwaddch(stats_win[TURN], HEIGHT_LABEL_CARD + 7, 2 + i, ' ');
        wrefresh(stats_win[TURN]);
        cross_sleep(15);
    }
    cross_sleep(100);
    for (int i = 0; i < strlen(string); i++) {
        mvwaddch(stats_win[TURN], HEIGHT_LABEL_CARD + 7, 2 + i, string[i]);
        wrefresh(stats_win[TURN]);

        cross_sleep(15);
    }
    mvwprintw(stats_win[TURN], HEIGHT_LABEL_CARD + 7, 2 + strlen(string), "%d", players[TURN].space_number + 1);
    wrefresh(stats_win[TURN]);
}

void refresh_cash(struct _win_st **stats_win) {
    const char string[] = "CASH: ";
    for (int i = 0; i < WIDTH_STATISTICS_CARD - 3; i++) {
        mvwaddch(stats_win[TURN], HEIGHT_LABEL_CARD + 1, 2 + i, ' ');
        wrefresh(stats_win[TURN]);
        cross_sleep(15);
    }
    cross_sleep(100);
    for (int i = 0; i < strlen(string); i++) {
        mvwaddch(stats_win[TURN], HEIGHT_LABEL_CARD + 1, 2 + i, string[i]);
        wrefresh(stats_win[TURN]);

        cross_sleep(15);
    }
    mvwprintw(stats_win[TURN], HEIGHT_LABEL_CARD + 1, 2 + strlen(string), "%d", players[TURN].cash);
    wrefresh(stats_win[TURN]);
}

void refresh_num_jail_passes(struct _win_st **stats_win) {
    const char string[] = "JAIL PASSES: ";
    for (int i = 0; i < WIDTH_STATISTICS_CARD - 3; i++) {
        mvwaddch(stats_win[TURN], HEIGHT_LABEL_CARD + 3, 2 + i, ' ');
        wrefresh(stats_win[TURN]);
        cross_sleep(15);
    }
    cross_sleep(100);
    for (int i = 0; i < strlen(string); i++) {
        mvwaddch(stats_win[TURN], HEIGHT_LABEL_CARD + 3, 2 + i, string[i]);
        wrefresh(stats_win[TURN]);

        cross_sleep(15);
    }
    mvwprintw(stats_win[TURN], HEIGHT_LABEL_CARD + 3, 2 + strlen(string), "%d", players[TURN].num_jail_pass);
    wrefresh(stats_win[TURN]);
}

void refresh_num_properties(struct _win_st **stats_win) {
    const char string[] = "OWNED PROPERTIES: ";
    for (int i = 0; i < WIDTH_STATISTICS_CARD - 3; i++) {
        mvwaddch(stats_win[TURN], HEIGHT_LABEL_CARD + 5, 2 + i, ' ');
        wrefresh(stats_win[TURN]);
        cross_sleep(15);
    }
    cross_sleep(100);
    for (int i = 0; i < strlen(string); i++) {
        mvwaddch(stats_win[TURN], HEIGHT_LABEL_CARD + 5, 2 + i, string[i]);
        wrefresh(stats_win[TURN]);

        cross_sleep(15);
    }
    mvwprintw(stats_win[TURN], HEIGHT_LABEL_CARD + 5, 2 + strlen(string), "%d", players[TURN].num_properties);
    wrefresh(stats_win[TURN]);
}