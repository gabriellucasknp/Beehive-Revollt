#ifndef QUIZ_H
#define QUIZ_H

#include <stdbool.h>

#define QUIZ_MAX_BLOCKS 6

typedef struct {
    char  text[48];
    float x, y;
    bool  dragging;
} QuizBlock;

typedef struct {
    int        stage;           /* which stage this quiz unlocks */
    char       prompt[256];
    QuizBlock  blocks[QUIZ_MAX_BLOCKS];
    int        block_count;
    int        correct_order[QUIZ_MAX_BLOCKS];
    int        current_order[QUIZ_MAX_BLOCKS];
    int        drag_idx;        /* -1 = none */
    bool       verified;
    bool       passed;
    char       feedback[64];
} Quiz;

void quiz_init(Quiz *q, int stage_number);
void quiz_update(Quiz *q);
void quiz_draw(const Quiz *q);

#endif /* QUIZ_H */
