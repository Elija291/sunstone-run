#ifndef RUNNER_H
#define RUNNER_H
#include <stdint.h>
enum { WIDTH=384, HEIGHT=224, ROWS=10 };
enum { START, RUN, OVER, PAUSED };
enum { LEFT, RIGHT, JUMP, SLIDE, ACCEPT, PAUSE };
enum { EMPTY, COIN, CRATE, ARCH, GAP };
typedef struct {float z; int lane[3];} Row;
typedef struct {
 int mode,target,coins,best; unsigned seed,sequence;
 float distance,x,jump,slide,time; Row row[ROWS];
} Game;
extern uint32_t screen[HEIGHT][WIDTH];
void init(Game *g,unsigned seed,int best);
void action(Game *g,int key);
void update(Game *g,float dt);
void render(const Game *g);
#endif
