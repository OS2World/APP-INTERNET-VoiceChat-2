/*   Multiply-free Arithmetic Coding Implementation  -  Expansion  version 0.0.0
 
 
     Copyright Feb. 1993
 
     Gordon V. Cormack  Feb. 1993
     University of Waterloo
     cormack@uwaterloo.ca
 
 
     All rights reserved.
 
     This code and the algorithms herein are the property of Gordon V. Cormack.
 
     Neither the code nor any algorithm herein may be included in any software,
     device, or process which is sold, exchanged for profit, or for which a
     licence or royalty fee is charged.
 
     Permission is granted to use this code for educational, research, or
     commercial purposes, provided this notice is included, and provided this
     code is not used as described in the above paragraph.
 
*/

extern unsigned char *temp,*dumpin;

#define THRESH 2
#include <stdio.h>


int diff[0x10000];

int inc0tab [0x10000];
int inc1tab [0x10000];

int cnt[0x100];


#define INC0(x) inc0tab[(unsigned int)(x)]
#define INC1(x) inc1tab[(unsigned int)(x)]
#define DIFF(x) diff[(unsigned int)(x)]
#define CNT1(x) ((x&0xff)+1)
#define CNT0(x) ((x>>8)+1)


void init() {
   int i,j;
   for (i=0;i<256;i++) for (j=0;j<256;j++) {
     if (i != 255)
        inc0tab[(i<<8)+j] = ((i+1)<<8) + j;
     else
        inc0tab[(i<<8)+j] = (127<<8) + (j>>1);
     if (j != 255)
        inc1tab[(i<<8)+j] = (i<<8) + j + 1;
     else
        inc1tab[(i<<8)+j] = ((i>>1)<<8) + 127;
     if (i < j) {
        diff[(i<<8)+j] = 175.0 * (i+1) / (i+j+2);
        if (!diff[(i<<8)+j]) diff[(i<<8)+j]++;
     }else{
        diff[(i<<8)+j] = -175.0 * (j+1) / (i+j+2);
        if (!diff[(i<<8)+j]) diff[(i<<8)+j]--;
     }
   }
}


unsigned long int dedec(unsigned long int length)
{
    register count = -8;
    unsigned register mask;
    register int space = 0xff, min = 0, index, i;
    unsigned register char c=0,gc = 0;
    unsigned register last = 0;
    unsigned register val;
    unsigned long int curIN=0,curOUT=0;

    for(curIN=0;curIN<0x10000;curIN++)
        {
            inc0tab[curIN] = inc1tab[curIN] = diff[curIN] = 0;
        }

    for(curIN=0;curIN<0x100;curIN++) cnt[curIN]=0;

    curIN = 0;

    init();

    val = dumpin[curIN++];
    gc = dumpin[curIN++];

do{
   c = 0;
   index = 1;
   for(mask=0x80;mask;mask>>=1){
         register l = last  + index;
         register a = cnt[l];
         register x = DIFF(a);
         if (x>0) 
            if (val < min+space-x) { 
               c |= mask;
               space -=x;
               cnt[l] = INC1(a);
               index += index + 1;
            }else { 
               min += space-x;
               space = x;
               cnt[l] = INC0(a);
               index += index;
            }
         else 
            if (val < min+space+x) {
               space +=x; 
               cnt[l] = INC0(a);
               index += index;
            } else { 
               c |= mask;
              min += space+x; 
              space = -x;
              cnt[l] = INC1(a);
              index += index+1;
            }
      while ((space) < 128) {
         space <<= 1;
         min <<= 1;
         val = (val<<1) | (gc>>(7-(count&7)));
         if (!++count) {
            count = -8;
            gc = dumpin[curIN++];
            min &= 0xffffff;
            val &= 0xffffff;
            if (0xffffff - min < space){
                space = 0xffffff - min;
                }
            }
         }
      }
   if ( curIN == length ) break;
   temp[curOUT++] = c;
   }
while (1);
return ( curOUT );
}
