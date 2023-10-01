#include <string.h>
#include <stdio.h>

#include "DaPizdec.h"
#include "DaGameScreen.h"

void update_title() {
  ++i;
  if (i > 20) {
    i = 0;
    j = j == 0 ? 1 : 0;
  }
  
  if (j == 0) sprintf(str, "PRESS START");
  else sprintf(str, "           ");
  vrambuf_put(NTADR_A(10, 15), str, 32);
  
  pad = pad_poll(0);
  if ((pad_poll(0) | pad_poll(1)) & PAD_START) {
    start_game();
  }
}