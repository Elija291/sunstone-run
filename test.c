#include "runner.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
static void setup(Game *g,int kind){
 init(g,42,0);action(g,ACCEPT);g->row[0]=(Row){.z=.12f,.lane={EMPTY,kind,EMPTY}};
}
int main(void){
 Game g;
 for(int hazard=CRATE;hazard<=GAP;hazard++){
  setup(&g,hazard);update(&g,.02f);assert(g.mode==OVER);action(&g,ACCEPT);assert(g.mode==RUN && g.distance==0);
  setup(&g,hazard);g.row[0].z=3;action(&g,hazard==ARCH?SLIDE:JUMP);
  for(int i=0;i<20;i++)update(&g,1.f/60);
  assert(g.mode==RUN);
 }
 setup(&g,COIN);update(&g,.02f);assert(g.coins==1);action(&g,PAUSE);float d=g.distance;update(&g,.1f);assert(g.distance==d);action(&g,PAUSE);
 setup(&g,CRATE);g.row[0].z=3;action(&g,LEFT);
 for(int i=0;i<20;i++)update(&g,1.f/60);
 assert(g.mode==RUN && g.x==-1);
 for(int i=0;i<12000;i++){
  for(int j=0;j<ROWS;j++){int safe=0;for(int k=0;k<3;k++){if(g.row[j].lane[k]<=COIN)safe++;g.row[j].lane[k]=EMPTY;}assert(safe);}
  update(&g,1.f/60);
 }
 assert(g.distance>1000);render(&g);
 puts("Passed: hazards, jump, slide, dodge, coins, pause, restart, row recycling and render.");
 return 0;
}
