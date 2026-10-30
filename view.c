#include "runner.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
uint32_t screen[HEIGHT][WIDTH];
typedef uint32_t Color;
typedef struct {int x,y;} Point;
static void rect(int x,int y,int w,int h,Color c){
 int x1=x<0?0:x,x2=x+w>WIDTH?WIDTH:x+w;
 for(int j=y<0?0:y;j<y+h&&j<HEIGHT;j++)for(int i=x1;i<x2;i++)screen[j][i]=c;
}
static void poly(Point *p,int n,Color c){
 int lo=HEIGHT,hi=0;
 for(int i=0;i<n;i++){if(p[i].y<lo)lo=p[i].y;if(p[i].y>hi)hi=p[i].y;}
 for(int y=lo<0?0:lo;y<=hi&&y<HEIGHT;y++){
  int xs[24],k=0;
  for(int i=0;i<n;i++){Point a=p[i],b=p[(i+1)%n];if((a.y<=y&&b.y>y)||(b.y<=y&&a.y>y))xs[k++]=a.x+(y-a.y)*(b.x-a.x)/(b.y-a.y);}
  for(int i=1;i<k;i++){int v=xs[i],j=i;while(j&&xs[j-1]>v){xs[j]=xs[j-1];j--;}xs[j]=v;}
  for(int i=0;i+1<k;i+=2)rect(xs[i],y,xs[i+1]-xs[i]+1,1,c);
 }
}
#define POLY(color,...) do { Point points[]={__VA_ARGS__};poly(points,sizeof points/sizeof points[0],color); } while(0)
static void oval(int x,int y,int a,int b,Color c){
 if(b<1||a<1)return;
 for(int i=-b;i<=b;i++){int w=a*sqrtf(fmaxf(0,1-(float)i*i/(b*b)));rect(x-w,y+i,w*2+1,1,c);}
}
static Color mix(Color a,Color b,float t){
 int r=(a>>16)*(1-t)+(b>>16)*t,g=((a>>8)&255)*(1-t)+((b>>8)&255)*t,bl=(a&255)*(1-t)+(b&255)*t;
 return (r<<16)|(g<<8)|bl;
}
/* Compact 3x5 pixel lettering. */
static void text(int x,int y,const char *s,int scale,Color color){
 static const char *chars="ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-";
 static const unsigned glyph[]={0x2bed,0x6bae,0x7927,0x6b6e,0x79a7,0x79a4,0x796f,0x5bed,0x7497,0x124e,0x5bad,0x4927,0x5fed,0x5ffd,0x7b6f,0x7be4,0x7b7b,0x6bad,0x79cf,0x7492,0x5b6f,0x5b6a,0x5bfd,0x5aad,0x5a92,0x72a7,0x7b6f,0x2c97,0x73e7,0x73cf,0x5bc9,0x79cf,0x79ef,0x7249,0x7bef,0x7bcf,0x01c0};
 for(;*s;s++,x+=4*scale){const char *p=strchr(chars,*s);if(!p)continue;unsigned bits=glyph[p-chars];for(int i=0;i<15;i++)if(bits&(1u<<(14-i)))rect(x+(i%3)*scale,y+(i/3)*scale,scale,scale,color);}
}
static float projection(float z){return 12.f/(12.f+z);}
static int center(float z,float d){return 192+(int)(25*sinf((d+z)*.013f)*(1-projection(z)));}
static int ground(float z){return 68+(int)(118*projection(z));}
static void palm(int x,int y,int h){
 int top=y-h,k=h/5;if(h<6)return;
 POLY(0x704934,{x-k/3,y},{x+k/3,y},{x+k/2,top},{x+k/6,top});
 int cx=x+k/3;for(int side=-1;side<=1;side+=2){
 POLY(0x174c41,{cx,top},{cx+side*k,top-k},{cx+side*k*3,top-k/2},{cx+side*k*4,top+k},{cx+side*k*2,top},{cx,top+k/4});
 POLY(0x38865a,{cx,top},{cx+side*k,top-k*2},{cx+side*k*3,top-k*2},{cx+side*k,top-k});
 POLY(0x22684c,{cx,top},{cx+side*k*2,top+k/2},{cx+side*k*2,top+2*k},{cx+side*k,top+k});}
}
static void scenery(const Game *g){
 float d=g->distance;int zone=((int)d/280)%3;
 Color sky[]={0x459eac,0x9b6289,0x477f9d},haze[]={0xffdc9c,0xffb17b,0xc1d9b2};
 for(int y=0;y<HEIGHT;y++)rect(0,y,WIDTH,1,mix(sky[zone],haze[zone],fminf(1,y/85.f)));
 oval(291,38,16,16,0xffe9ae);
 for(int layer=0;layer<2;layer++)for(int x=0;x<WIDTH;x++){
 float t=x+d*(layer+1)*.06f;int y=52+layer*11+8*sinf(t*.044f)+6*sinf(t*.083f);
 rect(x,y,1,90-y,layer?0x458578:0x78a79a);}
 /* Stepped sanctuary and its golden roof. */
 int tx=95+(int)(10*sinf(d*.007f));
 for(int i=0;i<4;i++){rect(tx-28+i*6,65-i*6,56-i*12,7,0x466c60);rect(tx-28+i*6,65-i*6,56-i*12,2,0xb5ad77);}
 POLY(0xc19253,{tx-11,46},{tx,32},{tx+11,46});
 rect(0,78,384,146,zone==1?0x458087:0x409998);
 for(int i=0;i<70;i++){int y=81+(i*19)%140,x=(i*59+(int)(d*1.4f))%400-12;rect(x,y,3+(y-78)/12,1,0x80c4ac);}
 /* Stone bridge in depth strips, with parapets and masonry seams. */
 for(int i=149;i>=-6;i--){
 float far=i+1-fmodf(d,1),near=i-fmodf(d,1);int a=ground(far),b=ground(near);
 if(a>=HEIGHT||b<=a)continue;
 int c=center(near,d),w=105*projection(near),edge=6*projection(near)+1;
 int row=(i+(int)d+100000)/3;Color road=(row%2)?0xc49a68:0xcfaa7b;
 if(zone==2)road=(row%2)?0x96a397:0xacb3a0;
 rect(c-w-edge,a,2*(w+edge),b-a,0x765d48);rect(c-w,a,2*w,b-a,road);
 for(int j=-1;j<=1;j++)rect(c+(j*2+(row%2))*w/3,a,1,b-a,0xac8b68);
 if((i+(int)d+100000)%3==0)rect(c-w,a,2*w,1,0xa38766);
 }
 for(int i=12;i>=0;i--){float z=i*11+6-fmodf(d,11);if(z < -4)continue;
 float s=projection(z);int y=ground(z),c=center(z,d);
 for(int side=-1;side<=1;side+=2){int x=c+side*(int)(135*s);
 if((i+(int)(d/11)+(side==1))%4) palm(x,y,69*s);
 else {int w=12*s+1,h=42*s;rect(x-w,y-h,w*2,h,0x486c63);rect(x-w,y-h,w/2,h,0x829781);rect(x-w-2,y-h-2,2*w+4,4*s+1,0xcead73);oval(x,y-h-5*s,4*s+1,6*s+1,0xffbd50);}}
 }
}
static void item(int type,int x,int y,float s,float time){
 int w=17*s+1,h=22*s+1;
 if(type==COIN){int cy=y-19*s+2*s*sinf(time*6),a=(4+3*fabsf(cosf(time*4)))*s+1;
 oval(x,cy,a+1,9*s+1,0x99551f);oval(x,cy,a,8*s+1,0xffb934);rect(x-a/2,cy-4*s,2*s+1,7*s+1,0xffed8a);}
 if(type==CRATE){rect(x-w,y-h,w*2,h,0x79432e);rect(x-w+2*s,y-h+2*s,w*2-4*s,h-4*s,0xb26736);
 POLY(0xebac59,{x-w,y-3*s},{x+w-3*s,y-h},{x+w,y-h+3*s},{x-w+3*s,y});
 rect(x-w,y-h,w*2,3*s+1,0xffcc79);}
 if(type==GAP){POLY(0x283c3c,{x-w,y-3*s},{x+w,y-3*s},{x+w+5*s,y+8*s},{x-w-5*s,y+8*s});rect(x-w,y-3*s,w*2,2*s+1,0x684f41);}
 if(type==ARCH){rect(x-w-2*s,y-43*s,4*s+1,43*s,0x556949);rect(x+w-2*s,y-43*s,4*s+1,43*s,0x556949);
 rect(x-w-3*s,y-43*s,w*2+6*s,23*s+1,0x397d56);rect(x-w-3*s,y-43*s,w*2+6*s,4*s+1,0x90ae62);
 for(int i=-1;i<=1;i++)rect(x+i*w/2,y-23*s,2*s+1,5*s+1,0x68a368);}
}
static void hero(const Game *g){
 int x=192+g->x*67,groundY=186,step=4*sinf(g->time*13);
 int y=groundY-(int)(37*sinf(3.14159f*g->jump/.86f));oval(x,groundY+1,10,3,0x866b50);
 if(g->slide>0){rect(x-9,y-7,18,6,0x243849);rect(x-8,y-14,13,8,0xd76639);oval(x+5,y-14,5,5,0x59392f);rect(x+1,y-17,9,2,0xefad4c);return;}
 rect(x-6,y-14,5,12+step,0x344951);rect(x+2,y-14,5,12-step,0x253847);
 rect(x-7,y-3+step,6,4,0x50382f);rect(x+2,y-3-step,6,4,0x50382f);
 rect(x-8,y-27,16,15,0xd96837);rect(x-9,y-27,17,3,0xf6ac53);
 rect(x-11,y-24+step/2,4,9,0x995b3d);rect(x+8,y-24-step/2,4,9,0xbf8051);
 rect(x-5,y-24,10,12,0x634b3a);rect(x-4,y-22,8,7,0x9c784b);rect(x-2,y-19,4,2,0xeac883);
 oval(x,y-32,6,7,0x533b33);rect(x-6,y-34,12,3,0xe8a646);rect(x+5,y-33,7,2,0xe58a38);
}
void render(const Game *g){
 scenery(g);
 /* Sort rows by depth after circular recycling. */
 int order[ROWS];for(int i=0;i<ROWS;i++){int j=i;while(j&&g->row[order[j-1]].z<g->row[i].z){order[j]=order[j-1];j--;}order[j]=i;}
 int drawn=0;
 for(int i=0;i<ROWS;i++){const Row *r=&g->row[order[i]];if(r->z<0&&!drawn){hero(g);drawn=1;}if(r->z < -4)continue;
 float s=projection(r->z);for(int j=0;j<3;j++)item(r->lane[j],center(r->z,g->distance)+(j-1)*67*s,ground(r->z),s,g->time);}
 if(!drawn)hero(g);
 char buf[64];rect(8,7,94,15,0x203e3b);snprintf(buf,sizeof buf,"%d M",(int)g->distance);text(14,12,buf,1,0xffe8a5);
 snprintf(buf,sizeof buf,"%d GOLD",g->coins);text(55,12,buf,1,0xffc450);
 const char *zones[]={"PALM RIVER","AMBER RUINS","JADE PASS"};text(330,11,zones[((int)g->distance/280)%3],1,0x193e3c);
 if(g->mode==RUN)return;
 rect(66,79,252,70,0x213a38);rect(66,79,252,2,0xeeb55c);
 const char *title=g->mode==START?"SUNSTONE RUN":g->mode==OVER?"RUN ENDED":"PAUSED";
 text(192-(int)strlen(title)*6,91,title,3,0xffcf77);
 if(g->mode==OVER)snprintf(buf,sizeof buf,"%d M   %d GOLD   BEST %d M",(int)g->distance,g->coins,g->best);
 else snprintf(buf,sizeof buf,"THREE LANES   ONE ANCIENT CAUSEWAY");
 text(192-(int)strlen(buf)*2,117,buf,1,0xc3d9bc);
 const char *prompt=g->mode==PAUSED?"P TO RESUME":g->mode==OVER?"ENTER TO RETRY":"ENTER TO RUN";
 text(192-(int)strlen(prompt)*2,133,prompt,1,0xffe5a3);
}
