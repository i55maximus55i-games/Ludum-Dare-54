#ifndef _DAGAME_H
#define _DAGAME_H

// attribute table in PRG ROM
const char GAME_ATTRIBUTE_TABLE[0x40] = {
  0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, // rows 0-3
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, // rows 4-7
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, // rows 8-11
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, // rows 12-15
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, // rows 16-19
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, // rows 20-23
  0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, // rows 24-27
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00  // rows 28-29
};



byte level[16];

int box[4*3]; // X, Y, VX, VY
bool box_en[3];
bool box_up[PLAYERS];

void start_game() {
  current_screen = GAME_SCREEN;
  
  // Clear Screen Buffer
  sprintf(str, "                              ");
  for (i = 1; i <= 24; ++i) {
    vrambuf_put(NTADR_A(1, i), str, 32);
  }
  
  for (i = 0; i < 32; ++i) {
    str[i] = 0x81;
  }
  vrambuf_put(NTADR_A(0, 1), str, 32);
  for (i = 0; i < 32; ++i) {
    str[i] = 0x80;
  }
  vrambuf_put(NTADR_A(0, 25), str, 32);
  
  memset(str, 0, sizeof(str));
  sprintf(str, "igPOK 1:%c%c%c      igPOK 2:%c%c%c", 0x12, 0x12, 0x12, 0x12, 0x12, 0x12);
  vrambuf_put(NTADR_A(2, 26), str, 32);
  
  // Sprite size
  oam_size(0);
  
  // Clear Level
  for (i = 0; i < 32; ++i) {
    level[i] = 0;
  }
  for (i = 0; i < 3; ++i) {
    box_en[i] = false;
  }
  box_timer = 0;
  // Init player positions;
  player_x[0] = 20;
  player_y[0] = 20;
  player_x[1] = 226;
  player_y[1] = 20;
  // Zero player velocity
  for (i = 0; i < PLAYERS; ++i) {
    player_vel_x[i] = 0;
    player_vel_y[i] = 0;
    player_jmp[i] = false;
    box_up[i] = false;
    player_hp[i] = 3;
  }
  
  memcpy(str, GAME_ATTRIBUTE_TABLE, 32);
  vrambuf_put(0x23C0, str, 32);
  memcpy(str, GAME_ATTRIBUTE_TABLE + 32, 32);
  vrambuf_put(0x23C0 + 32, str, 32);
}

void update_background(int q) {
    sprintf(str, "                              ");
    for (j = 0; j < 16; ++j) {
      if (level[j] > q) {
        str[j * 2] = 0xE2;
        str[j * 2 + 1] = 0xE3;
      }
    }
    vrambuf_put(NTADR_A(0, 23 - q * 2), str, 32);
    for (j = 0; j < 16; ++j) {
      if (level[j] > q) {
        str[j * 2]     = 0xF2;
        str[j * 2 + 1] = 0xF3;
      }
    }
    vrambuf_put(NTADR_A(0, 24 - q * 2), str, 32);
  
  if (q >= 10) {
    start_end();
  }
}

void render_game() {
  for (i = 0; i < PLAYERS; ++i) {
    oam_spr(player_x[i],     player_y[i],     0xE0, i * 2, 0 + i * 16);
    oam_spr(player_x[i] + 8, player_y[i],     0xE1, i * 2, 4 + i * 16);
    oam_spr(player_x[i],     player_y[i] + 8, 0xF0, 1,     8 + i * 16);
    oam_spr(player_x[i] + 8, player_y[i] + 8, 0xF1, 1,     12 + i * 16);
  }
  
  for (i = 0; i < 3; ++i) {
    if (box_en[i]) {
      oam_spr(box[4 * i],     box[4 * i + 1],     0xE2, 3, 32 + i * 16);
      oam_spr(box[4 * i] + 8, box[4 * i + 1],     0xE3, 3, 36 + i * 16);
      oam_spr(box[4 * i],     box[4 * i + 1] + 8, 0xF2, 3, 40 + i * 16);
      oam_spr(box[4 * i] + 8, box[4 * i + 1] + 8, 0xF3, 3, 44 + i * 16);
    } else {
      oam_spr(0, 0, 0x00, 3, 32 + i * 16);
      oam_spr(0, 0, 0x00, 3, 36 + i * 16);
      oam_spr(0, 0, 0x00, 3, 40 + i * 16);
      oam_spr(0, 0, 0x00, 3, 44 + i * 16);
    }
  }
}

void update_players() {
  for (i = 0; i < PLAYERS; ++i) {
    ++player_vel_y[i];              // Ускорение свободного падения
    player_y[i] += player_vel_y[i]; // Y = Y + Vy
    
    // Коллизия с полом
    xl = player_x[i] + 4;
    xr = player_x[i] + 11;
    hl = 184 - level[xl / 16] * 16;
    hr = 184 - level[xr / 16] * 16;
    h = hl < hr ? hl: hr;
    if (player_y[i] > h) {
      player_vel_y[i] = 0;
      player_y[i] = h;
    }
  
    // Jump
    pad = pad_poll(i);
    if (pad & PAD_A) {
      if (!player_jmp[i] && player_vel_y[i] == 0) {
        player_vel_y[i] = -10;
      };
      player_jmp[i] = true;
    } else {
      player_jmp[i] = false;
    }
    
    // Движение по горизонтали
    if (player_vel_y[i] == 0) { // Если Vy = 0, значит мы на земле
      if (pad & PAD_RIGHT) {
        player_vel_x[i] += 3;
      }
      if (pad & PAD_LEFT) {
        player_vel_x[i] -= 3;
      }
      if (player_vel_x[i] < 0) ++player_vel_x[i];
      if (player_vel_x[i] > 0) --player_vel_x[i];
      if (player_vel_x[i] < -5) player_vel_x[i] = -5;
      if (player_vel_x[i] > 5) player_vel_x[i] = 5;
    }
    x = player_x[i] + player_vel_x[i]; // Новое положение игрока, которое будем проверять
    xl = x + 4;
    xr = x + 11;
    hl = 184 - level[xl / 16] * 16;
    hr = 184 - level[xr / 16] * 16;
    h = hl < hr ? hl: hr;
    if (player_y[i] > h || x < 0 || x > 240) {
      player_vel_x[i] = 0;
    }
    player_x[i] += player_vel_x[i];
    
    // Хватание ящиков
    if (pad & PAD_B) {
      // Подобрать ящик, если он не летит, он есть близко, игрок не держит в данный момент ящик
      if (!box_en[i + 1] && !box_up[i]) {
        // Хватание ящика
        x = (player_x[i] + 8) / 16;
        // Возможность брать ящик слева
        if (x == 0) a = 0;
        else {
          if (level[x - 1] == 0) a = 0;
          else a = 1;
        }
        // Возможность брать ящик Под игроком
        if (level[x] == 0) b = 0;
        else b = 1;
        // Возможность брать ящик cправа
        if (x == 15) c = 0;
        else {
          if (level[x + 1] == 0) c = 0;
          else c = 1;
        }
        
        // Хватание
        if ((pad & PAD_LEFT) && a) {
          // Хватать слева
          j = level[x - 1] - 1;
          --level[x - 1];
          update_background(j);
          box_up[i] = true;
          box_en[i + 1] = true;
          
        } else if ((pad & PAD_RIGHT) && c) {
          // Хватать справа
          j = level[x + 1] - 1;
          --level[x + 1];
          update_background(j);
          box_up[i] = true;
          box_en[i + 1] = true;
        }
        if (!box_up[i] && b) {
          // Хватать снизу
          j = level[x] - 1;
          --level[x];
          update_background(j);
          box_up[i] = true;
          box_en[i + 1] = true;
        }
      }
      // Держать ящик
      if (box_up[i] && box_en[i + 1]) {
        // Держание ящика
        box[i * 4 + 0 + 4] = player_x[i];
        box[i * 4 + 1 + 4] = player_y[i] - 30;
        box[i * 4 + 2 + 4] = 0;
        box[i * 4 + 3 + 4] = 0;
      }
    } else {
      // Отпустить ящик
      if (box_up[i]) {
        // Дать импульс ящику
        box[i * 4 + 0 + 4] = player_x[i];
        box[i * 4 + 1 + 4] = player_y[i] - 30;
        box[i * 4 + 2 + 4] = player_vel_x[i] + ((pad & PAD_LEFT) ? -1 : 0) + ((pad & PAD_RIGHT) ? 1 : 0);
        box[i * 4 + 3 + 4] = player_vel_y[i] - 8;
        box_up[i] = false;
      }
    }
  }
}

void hit_player(byte player) {
  --player_hp[player];
  if (player == 0) {
    player_x[0] = 20;
    player_y[0] = 20;
  } else {
    player_x[1] = 226;
    player_y[1] = 20;
  }
  
  memset(str, 0, sizeof(str));
  sprintf(str, "igPOK 1:%c%c%c      igPOK 2:%c%c%c", player_hp[0] >= 1 ? 0x12 : ' ', player_hp[0] >= 2 ? 0x12 : ' ', player_hp[0] >= 3 ? 0x12 : ' ', player_hp[1] >= 1 ? 0x12 : 0x00, player_hp[1] >= 2 ? 0x12 : 0x00, player_hp[1] >= 3 ? 0x12 : 0x00);
  vrambuf_put(NTADR_A(2, 26), str, 32);
  
  if (player_hp[player] < 0) {
    start_end();
  }
}

void update_boxes() {
  // Спавн коробок, падающих сверху
  if (!box_en[0]) {
    ++box_timer;
    if (box_timer > 25) { // Падение ящика каждую секунду
      box_timer = 0;
      box[0] = rand8() % 16 * 16;
      //box[0] = 6 * 16;
      box[1] = 0;
      box[2] = 0;
      box[3] = 0;
      box_en[0] = true;
    }
  }
  // Обновление коробок
  for (i = 0; i < 3; ++i) {
    // Если коробка активна
    if (box_en[i]  && (i == 0 || !box_up[i - 1])) {
      ++box[3 + i * 4]; // Ускорение свободного падения
    
      box[0 + i * 4] += box[2 + i * 4]; // X = X + Vx
      box[1 + i * 4] += box[3 + i * 4]; // Y = Y + Vy
      
      // нормальная проверка коллизии (Спиздил у игрока)
      // Коллизия с полом
      xl = box[i * 4] + 4;
      xr = box[i * 4] + 12;
      x =  box[i * 4] + 8;
      hl = 184 - level[xl / 16] * 16;
      hr = 184 - level[xr / 16] * 16;
      h = hl < hr ? hl: hr;
      if (box[i * 4 + 1] > h) {
        box[i * 4 + 3] = 0;
        box[i * 4 + 1] = h;
        
        ++level[x / 16];
        box_en[i] = false;
        update_background(level[x / 16] - 1);
      }
      if (box[i * 4 + 1] > h || x < 0 || x > 240) {
        box[i * 4 + 2] = 0;
      }
      // Коллизия с игроком
      for (j = 0; j < PLAYERS; ++j) {
        if (player_x[j] + 4 < x + 8 && player_x[j] + 11 > x - 8 
            && player_y[j] < box[i * 4 + 1] + 16 && player_y[j] + 12 > box[i * 4 + 1]) {
          box_en[i] = false;
          hit_player(j);
        }
      }
    }
  }
}

void logic_game() {
  update_players(); // Логика игроков
  update_boxes();   // Логика у ящиков
}

void update_game() {
  logic_game();
  render_game();
}

#endif