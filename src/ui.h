#ifndef UI_H
#define UI_H

#include "types.h"
#include "assets.h"
#include "player.h"
#include <stdbool.h>

/* Returns true if "Play" / "Continue" was clicked */
bool ui_draw_menu(void);
bool ui_draw_pause(void);
bool ui_draw_game_over(int score, int stage);
bool ui_draw_victory(int score);

void ui_draw_hud(const Player *p, int score, int stage, const Assets *a);

/* Returns chosen power index (0..2) or -1 if not chosen yet */
int  ui_draw_power_choice(void);

/* Narrative screens, returns true when user advances past last screen */
bool ui_draw_narrative(int *screen_idx, const Assets *a);

#endif /* UI_H */
