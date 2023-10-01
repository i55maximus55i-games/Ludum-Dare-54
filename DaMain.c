#include <stdlib.h>
#include <string.h>

// include NESLIB header
#include "neslib.h"
// include CC65 NES Header (PPU)
#include <nes.h>
#include <string.h>
#include <stdio.h>

// link the pattern table into CHR ROM
//#link "chr_generic.s"

// BCD arithmetic support
#include "bcd.h"
//#link "bcd.c"

// VRAM update buffer
#include "vrambuf.h"
//#link "vrambuf.c"

/*{pal:"nes",layout:"nes"}*/
const char PALETTE[32] = { 
  0x00,			// screen color

  0x20,0x11,0x16,0x00,	// background palette 0
  0x1C,0x20,0x2C,0x00,	// background palette 1
  0x1C,0x13,0x25,0x00,	// background palette 2
  0x0D,0x17,0x27,0x00,   // background palette 3

  0x26,0x38,0x02,0x00,	// sprite palette 0
  0x02,0x37,0x07,0x00,	// sprite palette 1
  0x25,0x38,0x16,0x00,	// sprite palette 2
  0x0D,0x17,0x27	// sprite palette 3
};

// setup PPU and tables
void setup_graphics() {
  // clear vram buffer
  vrambuf_clear();
  // set NMI handler
  set_vram_update(updbuf);
  
  // clear sprites
  oam_clear();
  // set palette colors
  pal_all(PALETTE);
}

#include "DaPizdec.h"
void start_title() {
  for (i = 0; i < 64; ++i) {
    oam_spr(0, 0, 0, 0, 4 * i);
  }
  
  current_screen = TITLE_SCREEN;
  
  sprintf(str, "  %c dA %c %c%c       ", 0x12, 0x12, 0x10, 0x11);
  vrambuf_put(NTADR_A(10,9), str, 32);
  sprintf(str, "pOwEl(lA) Tq HAXYi");
  vrambuf_put(NTADR_A(7,11), str, 32);
  
  memset(str, 0, sizeof(str));
  str[0] = 0xAA;
  str[1] = 0xAA;
  str[2] = 0xAA;
  str[3] = 0xAA;
  str[8] = 0x55;
  str[9] = 0x55;
  str[10] = 0x55;
  str[11] = 0x55;
  vrambuf_put(0x23d0 + 2, str, 32);
}



void start_end() {
  current_screen = GAME_END;
  // Clear Screen Buffer
  sprintf(str, "                              ");
  for (i = 1; i <= 27; ++i) {
    vrambuf_put(NTADR_A(1, i), str, 32);
  }

  if (player_hp[0] == player_hp[1]) {
    sprintf(str, "Hi4'y");
    vrambuf_put(NTADR_A(10, 9), str, 32);
    
    oam_spr(64,     64,     0xE0, 0, 0);
    oam_spr(64 + 8, 64,     0xE1, 0, 4);
    oam_spr(64,     64 + 8, 0xF0, 1, 8);
    oam_spr(64 + 8, 64 + 8, 0xF1, 1, 12);
  
    oam_spr(120,     64,     0xE0, 2, 16);
    oam_spr(120 + 8, 64,     0xE1, 2, 20);
    oam_spr(120,     64 + 8, 0xF0, 1, 24);
    oam_spr(120 + 8, 64 + 8, 0xF1, 1, 28);
  } else if (player_hp[0] > player_hp[1]) {
    sprintf(str, "igPOK 1 pOdEbil");
    vrambuf_put(NTADR_A(10, 9), str, 32);
    
    oam_spr(64,     64,     0xE0, 0, 0);
    oam_spr(64 + 8, 64,     0xE1, 0, 4);
    oam_spr(64,     64 + 8, 0xF0, 1, 8);
    oam_spr(64 + 8, 64 + 8, 0xF1, 1, 12);
  
    oam_spr(120,     64,     0x00, 2, 16);
    oam_spr(120 + 8, 64,     0x00, 2, 20);
    oam_spr(120,     64 + 8, 0x00, 1, 24);
    oam_spr(120 + 8, 64 + 8, 0x00, 1, 28);
  } else {
    sprintf(str, "igPOK 2 pOdEbil");
    vrambuf_put(NTADR_A(10, 9), str, 32);
    
    oam_spr(64,     64,     0x00, 0, 0);
    oam_spr(64 + 8, 64,     0x00, 0, 4);
    oam_spr(64,     64 + 8, 0x00, 1, 8);
    oam_spr(64 + 8, 64 + 8, 0x00, 1, 12);
  
    oam_spr(64,     64,     0xE0, 2, 16);
    oam_spr(64 + 8, 64,     0xE1, 2, 20);
    oam_spr(64,     64 + 8, 0xF0, 1, 24);
    oam_spr(64 + 8, 64 + 8, 0xF1, 1, 28);
  }
  
  i = 0;
}

void update_end() {
  ++i;
  if (player_hp[0] == player_hp[1]) {
    oam_spr(64,     64,     0xE0, 0, 0);
    oam_spr(64 + 8, 64,     0xE1, 0, 4);
    oam_spr(64,     64 + 8, 0xF0, 1, 8);
    oam_spr(64 + 8, 64 + 8, 0xF1, 1, 12);
  
    oam_spr(120,     64,     0xE0, 2, 16);
    oam_spr(120 + 8, 64,     0xE1, 2, 20);
    oam_spr(120,     64 + 8, 0xF0, 1, 24);
    oam_spr(120 + 8, 64 + 8, 0xF1, 1, 28);
  } else if (player_hp[0] > player_hp[1]) {
    oam_spr(64,     64,     0xE0, 0, 0);
    oam_spr(64 + 8, 64,     0xE1, 0, 4);
    oam_spr(64,     64 + 8, 0xF0, 1, 8);
    oam_spr(64 + 8, 64 + 8, 0xF1, 1, 12);
  
    oam_spr(120,     64,     0x00, 2, 16);
    oam_spr(120 + 8, 64,     0x00, 2, 20);
    oam_spr(120,     64 + 8, 0x00, 1, 24);
    oam_spr(120 + 8, 64 + 8, 0x00, 1, 28);
  } else {
    oam_spr(64,     64,     0x00, 0, 0);
    oam_spr(64 + 8, 64,     0x00, 0, 4);
    oam_spr(64,     64 + 8, 0x00, 1, 8);
    oam_spr(64 + 8, 64 + 8, 0x00, 1, 12);
  
    oam_spr(64,     64,     0xE0, 2, 16);
    oam_spr(64 + 8, 64,     0xE1, 2, 20);
    oam_spr(64,     64 + 8, 0xF0, 1, 24);
    oam_spr(64 + 8, 64 + 8, 0xF1, 1, 28);
  }
  if (i > 120) {
    start_title();
  }
}

#include "DaTitleScreen.h"
#include "DaGameScreen.h"

void main(void)
{
  setup_graphics();
  memset(str, 0, sizeof(str));
  // enable rendering
  ppu_on_all();
  
  start_title();
  //start_game();
  //start_end();
  
  // infinite loop
  while(1) {
    switch (current_screen) {
      case GAME_SCREEN:
        update_game();
        break;
      case TITLE_SCREEN:
        update_title();
        break;
      case GAME_END:
        update_end();
        break;
    }
    ppu_wait_nmi();
  }
}
