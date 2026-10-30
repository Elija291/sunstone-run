#include "runner.h"
#include <math.h>
#include <string.h>
static unsigned random_number(Game *g){g->seed=1664525u*g->seed+1013904223u; return g->seed>>8;}
static void populate(Game *g,Row *row,float z){
 memset(row,0,sizeof *row);row->z=z;
 int safe=random_number(g)%3;
 row->lane[safe]=COIN;
 if(g->sequence++>0){
  row->lane[(safe+1)%3]=CRATE+random_number(g)%3;
  if(g->sequence>4 && random_number(g)%2)row->lane[(safe+2)%3]=CRATE+random_number(g)%3;
 }
}
void init(Game *g,unsigned seed,int best){
 memset(g,0,sizeof *g);g->seed=seed;g->best=best;
 for(int i=0;i<ROWS;i++)populate(g,&g->row[i],18+i*13);
}
void action(Game *g,int key){
 if(key==ACCEPT && (g->mode==START||g->mode==OVER)){init(g,g->seed,g->best);g->mode=RUN;return;}
 if(key==PAUSE && (g->mode==RUN||g->mode==PAUSED)){g->mode=g->mode==RUN?PAUSED:RUN;return;}
 if(g->mode!=RUN)return;
 if(key==LEFT && g->target>-1)g->target--;
 if(key==RIGHT && g->target<1)g->target++;
 if(key==JUMP && g->jump<=0 && g->slide<=0)g->jump=.86f;
 if(key==SLIDE && g->jump<=0)g->slide=.82f;
}
void update(Game *g,float dt){
 if(g->mode!=RUN)return;
 g->time+=dt;
 float dx=g->target-g->x;g->x+=copysignf(fminf(fabsf(dx),dt*7),dx);
 g->jump=fmaxf(0,g->jump-dt);g->slide=fmaxf(0,g->slide-dt);
 float travel=dt*(11+fminf(10,g->distance/110));g->distance+=travel;
 for(int i=0;i<ROWS;i++){
  Row *r=&g->row[i];float prev=r->z;r->z-=travel;
  if(prev>0 && r->z<=0){
   int slot=1+(int)roundf(g->x),kind=r->lane[slot];
   if(kind==COIN){g->coins++;r->lane[slot]=EMPTY;}
   int airborne=g->jump>.09f && g->jump<.77f;
   if(((kind==CRATE||kind==GAP)&&!airborne)||(kind==ARCH&&g->slide<=0)){
    g->mode=OVER;if((int)g->distance>g->best)g->best=(int)g->distance;return;
   }
  }
  if(r->z < -7)populate(g,r,r->z+ROWS*13);
 }
}
