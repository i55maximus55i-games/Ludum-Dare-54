#ifndef _DAPIZDEC_H
#define _DAPIZDEC_H

// Screens
#define TITLE_SCREEN 0
#define GAME_SCREEN 1
#define GAME_END 2
char current_screen = TITLE_SCREEN;

#define PLAYERS 2
int player_x[PLAYERS];
int player_y[PLAYERS];
bool player_jmp[PLAYERS];
int player_vel_x[PLAYERS];
int player_vel_y[PLAYERS];
int player_x[PLAYERS];
int player_y[PLAYERS];
int player_hp[PLAYERS];

char pad;
byte a, b, c;
int i = 0;
int j = 0;
int k = 0;
int l = 0;
int h, hl, hr;
int x, xl, xr;
int y;
int box_timer;
char str[32];

#endif