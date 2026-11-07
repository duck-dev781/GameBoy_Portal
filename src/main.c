#include <gb/gb.h>
#include <stdio.h>
#include <stdint.h>

/*
 * TEST SUBJECT 001
 * Original Game Boy puzzle-platformer prototype.
 * All visuals are original placeholder tile glyphs rendered in the
 * Game Boy text console; replaceable with custom tile art in later passes.
 */

#define MAP_W 20
#define MAP_H 14

static uint8_t px = 2, py = 11;
static uint8_t cube_x = 7, cube_y = 10;
static uint8_t cube_carried = 0;
static uint8_t cube_on_switch = 0;
static uint8_t door_open = 0;
static uint8_t side = 0;       /* 0 = FRONT, 1 = BACK */
static uint8_t portal_a = 0, portal_b = 1;
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

static void draw(void) {
    uint8_t x, y;
    const char **map = side ? back_map : front_map;
    cls();
    gotoxy(0,0); printf("TEST %02u  %s", room + 1, side ? "BACK " : "FRONT");
    gotoxy(0,1); printf("A:PORTAL  B:CUBE  SEL:SIDE");
    for (y=0; y<MAP_H; y++) {
        gotoxy(0,y+2);
        for (x=0; x<MAP_W; x++) {
            char c = map[y][x];
            if (x==px && y==py) c='@';
            else if (!cube_carried && x==cube_x && y==cube_y) c='O';
            else if (c=='C') c = cube_on_switch ? '=' : 'c';
            else if (c=='D') c = door_open ? '/' : 'D';
            putchar(c);
        }
    }
    gotoxy(0,16);
    if (boss_mode) printf("CORE AI HP: %u  HIT WITH CUBE", boss_hp);
    else if (cube_carried) printf("CUBE: HELD  A=DROP");
    else printf("CUBE: %s  EXIT:%s", cube_on_switch?"POWERED":"FREE",door_open?"OPEN":"LOCKED");
    gotoxy(0,17);
    printf("PORTALS %c <-> %c   ROOM %u", portal_a?'B':'A',portal_b?'B':'A',room+1);
}

static uint8_t wall_at(uint8_t x, uint8_t y) {
    const char **map = side ? back_map : front_map;
    if (x>=MAP_W || y>=MAP_H) return 1;
    return map[y][x]=='#';
}

static void update_world(void) {
    if (!cube_carried && px==cube_x && py==cube_y) {
        /* Touching the cube picks it up. */
        cube_carried=1;
    }
    if (cube_carried) {
        cube_x=px; cube_y=py;
    }
    if (!cube_carried && cube_x==8 && cube_y==10) cube_on_switch=1;
    else if (!cube_carried) cube_on_switch=0;
    if (cube_on_switch) door_open=1;
}

static void next_room(void) {
    room++;
    px=2; py=11; cube_x=7; cube_y=10;
    cube_carried=0; cube_on_switch=0; door_open=0;
    side=0;
    if (room>=2) boss_mode=1;
}

void main(void) {
    uint8_t keys, pressed;
    DISPLAY_ON;
    SHOW_BKG;
    printf("PORTAL: TEST SUBJECT 001");
    wait_vbl_done(); wait_vbl_done();
    cls();
    printf("WELCOME, TEST SUBJECT.");
    gotoxy(0,2); printf("Two sides. Two portals.");
    gotoxy(0,3); printf("One very suspicious cube.");
    gotoxy(0,5); printf("D-PAD: MOVE");
    gotoxy(0,6); printf("A: PICK UP / DROP CUBE");
    gotoxy(0,7); printf("B: USE PORTAL");
    gotoxy(0,8); printf("SELECT: SHIFT SIDE");
    gotoxy(0,10); printf("START: BEGIN TEST");
    while (!(joypad() & J_START)) wait_vbl_done();

    while (1) {
        keys=joypad();
        pressed=keys & (uint8_t)~old_keys;
        old_keys=keys;

        if (pressed & J_SELECT) side ^= 1;
        if (pressed & J_B) {
            /* Linked portals: step onto the paired portal position. */
            if (!side) { px=15; py=2; }
            else { px=2; py=2; }
        }
        if (pressed & J_A) {
            if (cube_carried) cube_carried=0;
            else if ((px+1==cube_x || px==cube_x+1 || py+1==cube_y || py==cube_y+1) &&
                     (px==cube_x || py==cube_y || px+1==cube_x || py+1==cube_y)) cube_carried=1;
        }

        if (pressed & J_LEFT  && !wall_at(px-1,py)) px--;
        if (pressed & J_RIGHT && !wall_at(px+1,py)) px++;
        if (pressed & J_UP    && !wall_at(px,py-1)) py--;
        if (pressed & J_DOWN  && !wall_at(px,py+1)) py++;

        update_world();

        if (door_open && px>=17 && py==12 && room<2) next_room();
        if (boss_mode && px>=16 && py<=3 && frame%18==0) {
            if (cube_carried) { boss_hp--; cube_carried=0; }
            if (boss_hp==0) room=3;
        }
        if (room==3) {
            cls();
            gotoxy(1,3); printf("TEST SEQUENCE");
            gotoxy(4,5); printf("COMPLETE!");
            gotoxy(1,8); printf("THE AI IS STILL");
            gotoxy(1,9); printf("NOT IMPRESSED.");
            gotoxy(1,12); printf("THANK YOU FOR");
            gotoxy(1,13); printf("PARTICIPATING.");
            while (1) wait_vbl_done();
        }
        draw();
        frame++;
        wait_vbl_done();
    }
}
