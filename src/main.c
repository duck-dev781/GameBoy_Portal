#include <gb/gb.h>
#include <gbdk/console.h>
#include <stdio.h>
#include <stdint.h>

/*
 * TEST SUBJECT 001
 * Stable text-mode prototype.
 * Every HUD string is kept within the Game Boy's 20-column console.
 */

#define MAP_W 20
#define MAP_H 14

static uint8_t px = 2, py = 11;
static uint8_t cube_x = 7, cube_y = 10;
static uint8_t cube_carried = 0;
static uint8_t cube_on_switch = 0;
static uint8_t door_open = 0;
static uint8_t side = 0;
static uint8_t room = 0;
static uint8_t boss_hp = 3;
static uint8_t boss_mode = 0;
static uint8_t frame = 0;
static uint8_t old_keys = 0;

static const char *front_map[MAP_H] = {
"####################",
"#                  #",
"#   A          B   #",
"#                  #",
"#       ####       #",
"#                  #",
"#   ###            #",
"#                  #",
"#            ###   #",
"#                  #",
"#       C          #",
"#                  #",
"#                D #",
"####################"
};

static const char *back_map[MAP_H] = {
"####################",
"#                  #",
"# B             A  #",
"#                  #",
"#       ####       #",
"#                  #",
"#            ###   #",
"#                  #",
"#   ###            #",
"#                  #",
"#          C       #",
"#                  #",
"# D                #",
"####################"
};

static uint8_t wall_at(uint8_t x, uint8_t y) {
    const char **map = side ? back_map : front_map;
    if (x >= MAP_W || y >= MAP_H) return 1;
    return map[y][x] == '#';
}

static void draw(void) {
    uint8_t x, y;
    const char **map = side ? back_map : front_map;

    cls();

    gotoxy(0, 0);
    printf("TEST %02u %s", room + 1, side ? "BACK" : "FRONT");

    gotoxy(0, 1);
    printf("D:MOVE A:CUBE B:PORT");

    for (y = 0; y < MAP_H; y++) {
        gotoxy(0, y + 2);
        for (x = 0; x < MAP_W; x++) {
            char c = map[y][x];
            if (x == px && y == py) c = '@';
            else if (!cube_carried && x == cube_x && y == cube_y) c = 'O';
            else if (c == 'C') c = cube_on_switch ? '=' : 'c';
            else if (c == 'D') c = door_open ? '/' : 'D';
            putchar(c);
        }
    }

    gotoxy(0, 16);
    if (boss_mode) printf("CORE HP:%u", boss_hp);
    else if (cube_carried) printf("CUBE: HELD");
    else if (cube_on_switch) printf("CUBE: SWITCH");
    else printf("CUBE: FREE");

    gotoxy(0, 17);
    printf("EXIT: %s", door_open ? "OPEN" : "LOCKED");
}

static void update_world(void) {
    if (!cube_carried && px == cube_x && py == cube_y) {
        cube_carried = 1;
    }

    if (cube_carried) {
        cube_x = px;
        cube_y = py;
    }

    if (!cube_carried && cube_x == 8 && cube_y == 10) {
        cube_on_switch = 1;
    } else if (!cube_carried) {
        cube_on_switch = 0;
    }

    if (cube_on_switch) door_open = 1;
}

static void next_room(void) {
    room++;
    px = 2;
    py = 11;
    cube_x = 7;
    cube_y = 10;
    cube_carried = 0;
    cube_on_switch = 0;
    door_open = 0;
    side = 0;
    if (room >= 2) boss_mode = 1;
}

void main(void) {
    uint8_t keys, pressed;

    DISPLAY_ON;
    SHOW_BKG;

    cls();
    gotoxy(0, 0); printf("PORTAL TEST 001");
    gotoxy(0, 2); printf("TWO SIDES. TWO");
    gotoxy(0, 3); printf("LINKED PORTALS.");
    gotoxy(0, 4); printf("ONE ODD CUBE.");
    gotoxy(0, 7); printf("D-PAD: MOVE");
    gotoxy(0, 8); printf("A: PICK UP / DROP");
    gotoxy(0, 9); printf("B: USE PORTAL");
    gotoxy(0, 10); printf("SELECT: SHIFT SIDE");
    gotoxy(0, 13); printf("PRESS START");

    while (!(joypad() & J_START)) wait_vbl_done();
    while (joypad() & J_START) wait_vbl_done();
    old_keys = joypad();

    while (1) {
        keys = joypad();
        pressed = keys & (uint8_t)~old_keys;
        old_keys = keys;

        if (pressed & J_SELECT) {
            side ^= 1;
            if (wall_at(px, py)) side ^= 1;
        }

        if (pressed & J_B) {
            if (!side) {
                if (!wall_at(15, 2)) { px = 15; py = 2; }
            } else {
                if (!wall_at(2, 2)) { px = 2; py = 2; }
            }
        }

        if (pressed & J_A) {
            if (cube_carried) {
                cube_carried = 0;
            } else {
                uint8_t near_cube =
                    ((px + 1 == cube_x || cube_x + 1 == px) && py == cube_y) ||
                    ((py + 1 == cube_y || cube_y + 1 == py) && px == cube_x);
                if (near_cube) cube_carried = 1;
            }
        }

        if ((pressed & J_LEFT) && px > 0 && !wall_at(px - 1, py)) px--;
        if ((pressed & J_RIGHT) && px < MAP_W - 1 && !wall_at(px + 1, py)) px++;
        if ((pressed & J_UP) && py > 0 && !wall_at(px, py - 1)) py--;
        if ((pressed & J_DOWN) && py < MAP_H - 1 && !wall_at(px, py + 1)) py++;

        update_world();

        if (door_open && px >= 17 && py == 12 && room < 2) {
            next_room();
        }

        if (boss_mode && px >= 16 && py <= 3 && frame % 18 == 0) {
            if (cube_carried) {
                if (boss_hp > 0) boss_hp--;
                cube_carried = 0;
            }
            if (boss_hp == 0) room = 3;
        }

        if (room == 3) {
            cls();
            gotoxy(1, 4); printf("TEST COMPLETE!");
            gotoxy(1, 7); printf("THE AI IS STILL");
            gotoxy(1, 8); printf("NOT IMPRESSED.");
            gotoxy(1, 12); printf("THANK YOU!");
            while (1) wait_vbl_done();
        }

        draw();
        frame++;
        wait_vbl_done();
    }
}
