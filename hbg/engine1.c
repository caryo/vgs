#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "engine0.h"
#include "engine1.h"

int roll(void) {
   int z1, z2, z;

   z1 = (rand() % 6) + 1;
   z2 = (rand() % 6) + 1;

   z = z1 + z2;

#if DEBUG
   printf("z1=%d z2=%d z=%2d\n", z1, z2, z);
#endif

   return z;
}

int maprand(int p[], int n, int z0) {
   int i;

   for (i=0; i<n; i++) {
      if (z0 <= p[i]) {
         break;
      }
   }
   return i;
}

int genrand(int p[], int n) {
   int z;
#if !defined(USE_DICE)
   int z0;

   z0 = (rand() % 1000) + 1;

   z = maprand(p,n,z0);
   if (z < 10) {
      z += 2;
   }
   else {
      z = 12;
   }
#if DEBUG
   printf("genrand: z0: %4d, z: %2d\n", z0, z);
#endif
#else
   z = roll();
#endif

   return z;
}

/*
 * onbase
 *
 * allowed input conditions:
 * ob[0]==-1,ob[1]==-1,ob[2]==-1
 * ob[0]>=0,ob[1]==-1,ob[2]==-1
 * ob[0]>=0,ob[1]>=0,ob[2]==-1
 */
void onbase(int ob[], int li) {
   int j;
#if DEBUG
   for(j=2; j>=0; j--) {
      printf("onbase(begin)- j:%d, ob[%d]:%d\n", j, j, ob[j]);
   }
   printf("onbase(begin)- li:%d\n", li);
   printf("\n");
#endif
   assert(ob[2] == -1);
#if DEBUG
   printf("\n");
#endif
   for (j=2; j>0; j--) {
      if (ob[j-1] < 0) continue;
      ob[j] = ob[j-1];
   }
   ob[0] = li;
#if DEBUG
   for(j=2; j>=0; j--) {
      printf("onbase (end)- j:%d, ob[%d]:%d\n", j, j, ob[j]);
   }
#endif
}

int offbase(int ob[]) {
   int j, rc = -1;
#if DEBUG
   for(j=2; j>=0; j--) {
      printf("offbase(begin)- j:%d, ob[%d]:%d\n", j, j, ob[j]);
   }
#endif
   for (j=2;j>=0;j--) {
      if (ob[j] >= 0) {
         rc = ob[j];
         ob[j] = -1;
         break;
      }
   }
#if DEBUG
   for(j=2; j>=0; j--) {
      printf("offbase(end)- j:%d, ob[%d]:%d\n", j, j, ob[j]);
   }
   printf("offbase(end)- rc:%d\n", rc);
#endif
   return rc;
}

char * result_code(int z) {
   char *code;
   switch(z) {
      case 2:
         code = "WP";
         break;
      case 3:
         code = "T";
         break;
      case 4:
         code = "D";
         break;
      case 5:
         code = "W";
         break;
      case 6:
         code = "GO";
         break;
      case 7:
         code = "FO";
         break;
      case 8:
         code = "S";
         break;
      case 9:
         code = "SO";
         break;
      case 10:
         code = "FB";
         break;
      case 11:
         code = "DP";
         break;
      case 12:
         code = "HR";
         break;
      default:
         code = "X";
         break;
   }
   return code;
}

int result(int z, int a, int *o, int *h, int *r, int *c, int *gdp, int idx, int ob[], int rli[], int *rlo) {
   int x, f, w;
   int b, r1 = 0, r2 = 0;

   x = -1;
   f = 0;
   w = 0;

   *gdp = 0;

   switch(z) {
      case  2:
         x = 0;
         w = 0;
      break;

      case  3:
         x = 3;
         *h = *h + 1;
      break;

      case  4:
         x = 2;
         *h = *h + 1;
      break;

      case  5:
         x = 0;
         w = 1;
      break;

      case 6:
      case 7:
      case 9:
         break;

      case  8:
         x = 1;
         *h = *h + 1;
      break;

      case 10:
         f = 1;
      break;

      case 11:
         x = -2;
      break;

      case 12:
         x = 4;
         *h = *h + 1;
      break;

      default:
      break;
   }

   if (x < 0) {
      int j;
      for (j=0; j<3; j++) {
         if (ob[j] >= 0) {
            (*rlo)++;
#if DEBUG
            printf("z:%d, x:%d, j:%d, ob[%d]:%d, rlo:%d\n", z, x, j, j, ob[j], *rlo);
#endif
         }
      }
   }

   if (w == 1) {
      int j;
#if DEBUG
      printf("before advance- *r:%d, c:0x%1x, r1:0x%02x, r2:%d\n", *r, *c, r1, r2);
#endif
      advance(a, &b, c, &r1, &r2);
      *r = *r + r2;
#if DEBUG
      printf("after advance- *r:%d, c:0x%1x, r1:0x%02x, r2:%d\n", *r, *c, r1, r2);
#endif
      for (j=0; j<r2; j++) {
         int li = offbase(ob);
         if (li >= 0) {
            rli[j] = li;
#if DEBUG
            printf("j:%d, rli[j]:%d scored!\n", j, li);
#endif
         }
      }
      onbase(ob,idx);
   }
   else if (x >= 0) {
      int j;
#if DEBUG
      printf("before advance_x- x:%d,*r:%d, c:0x%1x, r1:0x%02x, r2:%d\n", x, *r, *c, r1, r2);
#endif
      advance_x(a, x, &b, c, &r1, &r2);
      *r = *r + r2;
#if DEBUG
      printf("after advance_x- x:%d,*r:%d, c:0x%1x, r1:0x%02x, r2:%d\n", x, *r, *c, r1, r2);
#endif
      if (x == 4) r2 = r2 - 1;
      for (j=0; j<r2; j++) {
         int li;
         li = offbase(ob);
         if (li >= 0) {
            rli[j] = li;
#if DEBUG
            printf("j:%d, rli[j]:%d scored!\n", j, li);
#endif
         }
      }
      if (x == 4) {
         rli[j] = idx;
      }
      else if (x > 0) {
         onbase(ob,idx);
      }
   }
   else {
      if (x == -2) {
         if (*c > 0) {
#if DEBUG
            int li = -1;
#endif
            if (*c & 0x04) {
               *c = *c & ~(0x04);
#if DEBUG
               li = offbase(ob);
               printf("c:%2x runner %d is out!\n", 0x04, li);
#else
               offbase(ob);
#endif
            }
            else if (*c & 0x02) {
               *c = *c & ~(0x02);
#if DEBUG
               li = offbase(ob);
               printf("c:%2x runner %d is out!\n", 0x02, li);
#else
               offbase(ob);
#endif
            }
            else if (*c & 0x01) {
               *c = *c & ~(0x01);
#if DEBUG
               li = offbase(ob);
               printf("c:%2x runner %d is out!\n", 0x01, li);
#else
               offbase(ob);
#endif
            }
            if (*rlo > 0) {
               (*rlo)--;
            }
            *o = *o + 1;
            if (*o < 3) {
               *o = *o + 1;
               *gdp = 1;
            }
         }
         else {
            *o = *o + 1;
         }
      }
      else if (x == -1) {
         if (f == 0) {
            *o = *o + 1;
         }
      }
   }

   return z;
}

void statrun(struct bstatdata stats[], int idx, int result) {
   switch (result) {
      case 2:
      case 3:
      case 4:
      case 5:
      case 8:
      case 12:
         (stats[idx].r)++;
      break;
      default:
      break;
   }
}

void stat(struct bstatdata stats[], int idx, int result, int gdp, int rbi, int lob) {
   switch (result) {
      case 2:
      break;
      case 3:
         (stats[idx].pa)++;
         (stats[idx].ab)++;
         (stats[idx].h)++;
         (stats[idx].t)++;
         stats[idx].rbi += rbi;
      break;
      case 4:
         (stats[idx].pa)++;
         (stats[idx].ab)++;
         (stats[idx].h)++;
         (stats[idx].d)++;
         stats[idx].rbi += rbi;
      break;
      case 5:
         (stats[idx].pa)++;
         (stats[idx].bb)++;
      break;
      case 6:
      case 7:
         (stats[idx].pa)++;
         (stats[idx].ab)++;
         stats[idx].lob += lob;
      break;
      case 8:
         (stats[idx].pa)++;
         (stats[idx].ab)++;
         (stats[idx].h)++;
         (stats[idx].s)++;
         stats[idx].rbi += rbi;
      break;
      case 9:
         (stats[idx].pa)++;
         (stats[idx].ab)++;
         (stats[idx].so)++;
         stats[idx].lob += lob;
      break;
      case 10:
      break;
      case 11:
         (stats[idx].pa)++;
         (stats[idx].ab)++;
         (stats[idx].gdp) += gdp;
         stats[idx].lob += lob;
      break;
      case 12:
         (stats[idx].pa)++;
         (stats[idx].ab)++;
         (stats[idx].h)++;
         (stats[idx].hr)++;
         stats[idx].rbi += rbi;
      break;
      default:
      break;
   }
}

void side(struct probdata p[], int n,
          int i, int b, int d, int *r, int *h, int *li,
          int li_base, struct bstatdata batstat[], struct bstatdata pitbstat[],
          struct pstatdata pitpstat[], int pitIdx, int *lob)
{
   int j;
   int z;
   int o;
   int a, c;
   int gdp;
   int idx;
   int ob[3];
   int rli[4];
   int rbi, rs;
   int rlo;

   o = 0;
   c = 0;

   for (j=0; j<3; j++) {
      ob[j] = rli[j] = -1;
   }
   rli[3] = -1;

   do {
      a =  c;

      idx = (*li - li_base) % 9;

#if DEBUG
      printf("\n");
#endif
      z = genrand(p[idx].p, n);

#if DEBUG
      printf("side: *li:%d, (*li - li_base):%d, idx:%d, z:%d, rc:%s\n", *li, (*li - li_base), idx, z,
        result_code(z));
      printf("a=0x%02x o=%d h=%2d r=%2d z=%2d\n", a, o, *h, *r, z);
#endif

      for (j=0; j<4; j++) {
         rli[j] = -1;
      }

      rlo = 0;
      result(z, a, &o, h, r, &c, &gdp, idx, ob, rli, &rlo);
      rbi = 0;
      rs = 0;
      for (j=0; j<4; j++) {
         if (rli[j] >= 0) {
#if DEBUG
            printf("rli[%d]:%d\n", j, rli[j]);
#endif
            if (z != 2 && z != 5) {
               rbi++;
            }
            else {
               rs++;
            }
            statrun(batstat,rli[j],z);
            statrun(pitbstat,pitIdx,z);
         }
      }

      printf("%02d (%d): i:%-2d o:%d gdp:%d rlo:%d rbi:%d rs:%d z:%-2d rc:%2s, ob[0]:%2d ob[1]:%2d ob[2]:%2d\n",
        *li, idx, i+1, o, gdp, rlo, rbi, rs, z, result_code(z), ob[0], ob[1], ob[2]);
      stat(batstat,idx,z,gdp,rbi,rlo);
      stat(pitbstat,pitIdx,z,gdp,rbi,rlo);

      if (z != 2 && z != 10) {
         *li = *li + 1;
      }

      if (i >= 8 && b == 1 && d + *r > 0) {
         printf("i:%d, b:%d, o:%d, d:%d, *r:%d\n", i, b, o, d, *r);
         printf("walk-off win!\n");
         break;
      }
   } while (o<3);

   if (o < 3) {
      pitpstat[pitIdx].ip_f = o;
   }
   else {
      pitpstat[pitIdx].ip += 1;
   }

   *lob = 0;
   for (j=0; j<3; j++) {
      if (ob[j] >= 0) {
         (*lob)++;
      }
   }

#if DEBUG
   printf("a=0x%02x o=%d h=%2d r=%2d, lob=%2d\n", a, o, *h, *r, *lob);
#endif
}
