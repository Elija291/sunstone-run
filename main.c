#include "runner.h"
#include <SDL2/SDL.h>
#include <time.h>
#include <stdio.h>
int main(void){
 if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER)!=0){fprintf(stderr,"%s\n",SDL_GetError());return 1;}
 SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY,"nearest");
 SDL_Window *window=SDL_CreateWindow("Sunstone Run",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,960,560,SDL_WINDOW_RESIZABLE);
 SDL_Renderer *renderer=window?SDL_CreateRenderer(window,-1,0):NULL;
 SDL_Texture *texture=renderer?SDL_CreateTexture(renderer,SDL_PIXELFORMAT_RGB888,SDL_TEXTUREACCESS_STREAMING,WIDTH,HEIGHT):NULL;
 if(!texture){fprintf(stderr,"%s\n",SDL_GetError());SDL_Quit();return 1;}
 SDL_RenderSetLogicalSize(renderer,WIDTH,HEIGHT);
 Game game;init(&game,(unsigned)time(NULL),0);
 int running=1;Uint64 last=SDL_GetPerformanceCounter();double accumulator=0;
 while(running){
  SDL_Event event;
  while(SDL_PollEvent(&event)){
   if(event.type==SDL_QUIT)running=0;
   if(event.type==SDL_WINDOWEVENT && event.window.event==SDL_WINDOWEVENT_FOCUS_LOST && game.mode==RUN)action(&game,PAUSE);
   if(event.type!=SDL_KEYDOWN || event.key.repeat)continue;
   switch(event.key.keysym.sym){
   case SDLK_LEFT:case SDLK_a:action(&game,LEFT);break;
   case SDLK_RIGHT:case SDLK_d:action(&game,RIGHT);break;
   case SDLK_UP:case SDLK_w:case SDLK_SPACE:action(&game,JUMP);break;
   case SDLK_DOWN:case SDLK_s:action(&game,SLIDE);break;
   case SDLK_RETURN:action(&game,ACCEPT);break;
   case SDLK_p:action(&game,PAUSE);break;
   case SDLK_ESCAPE:running=0;break;}
  }
  Uint64 now=SDL_GetPerformanceCounter();accumulator+=(double)(now-last)/SDL_GetPerformanceFrequency();last=now;
  if(accumulator>.1)accumulator=.1;
  while(accumulator>=1./60){update(&game,1.f/60);accumulator-=1./60;}
  render(&game);SDL_UpdateTexture(texture,NULL,screen,WIDTH*sizeof(uint32_t));
  SDL_RenderClear(renderer);SDL_RenderCopy(renderer,texture,NULL,NULL);SDL_RenderPresent(renderer);SDL_Delay(8);
 }
 SDL_DestroyTexture(texture);SDL_DestroyRenderer(renderer);SDL_DestroyWindow(window);SDL_Quit();return 0;
}
