#ifndef QUICK_QUIZ_H
#define QUICK_QUIZ_H

#include <stdbool.h>

#define QUICK_QUIZ_OPTION_COUNT 3

typedef struct {
    char  title[64];
    char  subtitle[160];
    char  prompt[192];
    char  options[QUICK_QUIZ_OPTION_COUNT][96];
    int   correct_option;
    int   selected_option;
    float time_left;
    float time_limit;
    bool  active;
    bool  finished;
    bool  passed;
    bool  timed_out;
} QuickQuiz;

void quick_quiz_start(QuickQuiz *q);
void quick_quiz_start_custom(QuickQuiz *q,
                             const char *title,
                             const char *subtitle,
                             const char *prompt,
                             const char *const options[QUICK_QUIZ_OPTION_COUNT],
                             int correct_option,
                             float time_limit);
void quick_quiz_update(QuickQuiz *q, float dt);
void quick_quiz_draw(const QuickQuiz *q);

#endif /* QUICK_QUIZ_H */
