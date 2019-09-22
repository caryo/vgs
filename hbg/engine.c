#include <assert.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

struct statdata {
   int pa;
   int ab;
   int r;
   int h;
   int rbi;
   int bb;
   int so;
   int lob;
   int s;
   int d;
   int t;
   int hr;
   int gdp;
};

struct probdata {
   int p[10];
};

void   onbase(int ob[], int li);
int    offbase(int ob[]);
int    advance(int a, int *b, int *c, int *r1, int *r2);
int    advance_x(int a, int x, int *b, int *c, int *r1, int *r2);
void   initialize(int seed);
void   test_advance(int argc, char *argv[]);
char * result_code(int z);
int    result(int z, int a, int *o, int *h, int *r, int *c, int *gdp, int idx, int ob[], int rli[], int *rlo);
void   statrun(struct statdata stats[], int idx, int result);
void   stat(struct statdata stats[], int idx, int result, int gdp, int rbi, int lob);
int    roll(void);
long   myround(double x);
void   initrand(int p[], int n);
int    maprand(int p[], int n, int z0);
int    genrand(int p[], int n);
void   side(struct probdata p[], int n, int i, int b, int d, int *r, int *h, int *li,
            int li_base, struct statdata statp[], int *lob);
void   initmem(int c, int *ahiP[], int *ariP[], int *hhiP[], int *hriP[]);
void   linescore(int i, int ahiP[], int ariP[], int ar, int alo, int hhiP[], int hriP[], int hr, int hlo);
void   boxscore(struct statdata astat[], struct statdata hstat[]);
void   match(void);

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

int advance(int a, int *b, int *c, int *r1, int *r2) {
   int x = 0, r;

   /* special case when x is 0 and a is 7 - treat x as a 1 */
   if (a == 7 && x == 0) {
      x = 1;
#if DEBUG
         printf("x =%d\n", x);
#endif
      r = advance_x(a, x, b, c, r1, r2);
   }
   else {
      int q0, q1, i;

      r  = 0;
      q0 = a;
      q1 = a;
      *b = 0;
      *c = 0;
      *r1 = 0;
      *r2 = 0;

      for (i=0; q0 && i<3; i++) {
#if DEBUG
         printf("i=%d, q0=0x%02x, q1=0x%02x, b=0x%02x\n", i, q0, q1, *b);
#endif
         if ((q0 % 2) == 0) {
            break;
         }
         q1 = q1 & ~(1 << i);
         *b |= 1;
         *b = *b << 1;
         q0 = q0 >> 1;
      }

      *b |= 1;

      *c = q1 | *b;
#if DEBUG
      printf("q0=0x%02x, q1=0x%02x, b=0x%02x, c=0x%02x\n", q0, q1, *b, *c);
#endif
   }

   return r;
}

int advance_x(int a, int x, int *b, int *c, int *r1, int *r2) {
   int r, q, i;

   if (x > 0) {
      *b  = (a << x) + (1 << (x-1));
   }
   else {
      *b  = (a << 1);
   }

   *r1 = (*b & 0xf8);
   r   = *r1 >> 3;
   *c  = *b & 0x07;

   q = r;

   *r2 = 0;

   for (i=0; q && i<4; i++) {
#if DEBUG
      printf("i=%d, q=0x%02x, r2=0x%02x\n", i, q, *r2);
#endif
      if ((q % 2) == 1) {
         *r2 += 1;
      }
      q = q >> 1;
   }

   return r;
}

void initialize(int seed) {
   if (seed < 0) {
      int seed0 = time(NULL);
      printf("seed0: %d\n", seed0);
      seed = seed0;
   }
   else {
      printf("seed: %d\n", seed);
   }
   srand(seed);
}

void test_advance(int argc, char *argv[]) {
#if DEBUG
   int i;
   int r;
#endif
   int a;
   int x;
   int b;
   int c;
   int r1;
   int r2;

#if DEBUG
   for (i=0; i<argc; i++) {
      printf("argv[%d]=%s\n", i, argv[i]);
   }
   printf("\n");
#endif

   if (argc <= 3) {
      printf("error: argc=%d, expected at least 3 integer values as input - exiting\n", argc);
      exit(1);
   }

   a = atoi(argv[2]);
   x = atoi(argv[3]);

   if (a < 0) {
      a = 0;
   }
   else if (a > 7) {
      a = 7;
   }

   if (x < -1) {
      x = -1;
   }
   else if (x > 4) {
      x = 4;
   }

#if DEBUG
   printf("a =%d (0x%02x)\n", a, a);
   printf("x =%d\n", x);
#endif

   if (x <= 0) {
      if (x == 0) {
#if DEBUG
         r = advance(a, &b, &c, &r1, &r2);
#else
         advance(a, &b, &c, &r1, &r2);
#endif
      }
      else if (x == -1) {
#if DEBUG
         r = advance_x(a, 0, &b, &c, &r1, &r2);
#else
         advance_x(a, 0, &b, &c, &r1, &r2);
#endif
      }
   }
   else {
#if DEBUG
      r = advance_x(a, x, &b, &c, &r1, &r2);
#else
      advance_x(a, x, &b, &c, &r1, &r2);
#endif
   }

#if DEBUG
   printf("r1=%d (0x%02x)\n", r1, r1);
   printf("r =%d (0x%02x)\n", r, r);
   printf("b =%d (0x%02x)\n", b, b);
   printf("c =%d (0x%02x)\n", c, c);
   printf("r2=%d (0x%02x)\n", r2, r2);
#endif
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
      for (j=0; j<2; j++) {
         if (ob[j] >= 0) {
            (*rlo)++;
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

long myround(double x) {
   assert(x >= LONG_MIN-0.5);
   assert(x <= LONG_MAX+0.5);
   if (x >= 0)
      return (long) (x+0.5);
   return (long) (x-0.5);
}

void initrand(int p[], int n) {
   int i;
   float p0, t0 = 0, t = 0;

   for (i=0; i<6; i++) {
      p0 = (((float) ((i+1)*2) / 36) * 1000) / 2.;
      t += p0;
      p[i] = myround(t);
      if (i < 4) {
         t0 += p0;
         p[9-i] = myround(1000. - t0);
#if DEBUG
         printf("i:%2d, p0: %7.3f, p[%d]: %4d, 1000-t0: %7.3f, p[%d]: %4d\n", i, p0, i, p[i], 1000. - t0, 9-i, p[9-i]);
#endif
      }
      else {
#if DEBUG
         printf("i:%2d, p0: %7.3f, p[%d]: %4d\n", i, p0, i, p[i]);
#endif
      }
   }

#if DEBUG
   for (i=0; i<n; i++) {
      printf("i:%2d, p[%d]: %4d\n", i, i, p[i]);
   }
#endif
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
   z += 2;
#if DEBUG
   printf("genrand: z0: %4d, z: %2d\n", z0, z);
#endif
#else
   z = roll();
#endif

   return z;
}

void statrun(struct statdata stats[], int idx, int result) {
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

void stat(struct statdata stats[], int idx, int result, int gdp, int rbi, int lob) {
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

void side(struct probdata p[], int n, int i, int b, int d, int *r, int *h, int *li,
          int li_base, struct statdata statp[], int *lob)
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

      z = genrand(p[idx].p, n);

#if DEBUG
      printf("\n");
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
            statrun(statp,rli[j],z);
         }
      }

      printf("%02d (%d): i:%-2d o:%d gdp:%d rlo:%d rbi:%d rs:%d z:%-2d rc:%s\n",
        *li, idx, i+1, o, gdp, rlo, rbi, rs, z, result_code(z));
      stat(statp,idx,z,gdp,rbi,rlo);

      if (z != 2 && z != 10) {
         *li = *li + 1;
      }

      if (i >= 8 && b == 1 && d + *r > 0) {
         printf("i:%d, b:%d, o:%d, d:%d, *r:%d\n", i, b, o, d, *r);
         printf("walk-off win!\n");
         break;
      }
   } while (o<3);

   *lob = 0;
   for (j=0; j<2; j++) {
      if (ob[j] >= 0) {
         (*lob)++;
      }
   }

#if DEBUG
   printf("a=0x%02x o=%d h=%2d r=%2d, lob=%2d\n", a, o, *h, *r, *lob);
#endif
}

void initmem(int c, int *ahiP[], int *ariP[], int *hhiP[], int *hriP[]) {
   if (*ahiP != NULL) {
      *ahiP = realloc(*ahiP,sizeof(int)*c);
   }
   else {
      *ahiP = malloc(sizeof(int)*c);
      memset(*ahiP,0,sizeof(int)*c);
   }
   if (*ariP != NULL) {
      *ariP = realloc(*ariP,sizeof(int)*c);
   }
   else {
      *ariP = malloc(sizeof(int)*c);
      memset(*ariP,0,sizeof(int)*c);
   }
   if (*hhiP != NULL) {
      *hhiP = realloc(*hhiP,sizeof(int)*c);
   }
   else {
      *hhiP = malloc(sizeof(int)*c);
      memset(*hhiP,0,sizeof(int)*c);
   }
   if (*hriP != NULL) {
      *hriP = realloc(*hriP,sizeof(int)*c);
   }
   else {
      *hriP = malloc(sizeof(int)*c);
      memset(*hriP,0,sizeof(int)*c);
   }
}

void linescore(int i, int ahiP[], int ariP[], int ar, int alo,
               int hhiP[], int hriP[], int hr, int hlo)
{
   int ah = 0, hh = 0;
   int in = i+1;

   printf("T: ");
   for (i=0; i<in; i++) {
      printf("%2d ", i+1);
      if (((i+1) % 3) == 0) {
         printf(" ");
      }
   }
   printf("%5c %2c %2c %5s\n", 'R', 'H', 'E', "LOB");

   printf("A: ");
   for (i=0; i<in; i++) {
      printf("%2d ", ariP[i]);
      if (((i+1) % 3) == 0) {
         printf(" ");
      }
      ah = ah + ahiP[i];
   }
   printf("%5d %2d %2d %5d\n", ar, ah, 0, alo);
   printf("H: ");
   for (i=0; i<in; i++) {
      if (i>=8 && hriP[i] == -1) {
         printf("%2c ", 'x');
      }
      else {
         printf("%2d ", hriP[i]);
         hh = hh + hhiP[i];
      }
      if (((i+1) % 3) == 0) {
         printf(" ");
      }
   }
   printf("%5d %2d %2d %5d\n", hr, hh, 0, hlo);
}

void boxscore(struct statdata astat[], struct statdata hstat[]) {
   int i;
   int s_pa, s_ab, s_r, s_h, s_rbi, s_bb, s_so, s_lob, s_s, s_d, s_t, s_hr, s_gdp;

   s_pa=0; s_ab=0; s_r=0; s_h=0; s_rbi=0; s_bb=0; s_so=0; s_lob=0; s_s=0; s_d=0; s_t=0; s_hr=0; s_gdp=0;
   printf("Away Team:\n");
   printf("%3s %3s %3s %3s %3s %3s %3s %3s %3s %3s %3s %3s %3s %3s\n",
     "#", "PA", "AB", "R", "H", "RBI", "BB", "SO", "LOB", "S", "D", "T", "HR", "GDP");
   for(i=0; i<9; i++) {
      printf("%3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d\n",
         i+1, astat[i].pa, astat[i].ab, astat[i].r, astat[i].h, astat[i].rbi,
         astat[i].bb, astat[i].so, astat[i].lob, astat[i].s, astat[i].d,
         astat[i].t, astat[i].hr, astat[i].gdp);
      s_pa += astat[i].pa;
      s_ab += astat[i].ab;
      s_r += astat[i].r;
      s_h += astat[i].h;
      s_rbi += astat[i].rbi;
      s_bb += astat[i].bb;
      s_so += astat[i].so;
      s_lob += astat[i].lob;
      s_s += astat[i].s;
      s_d += astat[i].d;
      s_t += astat[i].t;
      s_hr += astat[i].hr;
      s_gdp += astat[i].gdp;
   }
   printf("TOT %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d\n",
      s_pa, s_ab, s_r, s_h, s_rbi, s_bb, s_so, s_lob, s_s, s_d, s_t, s_hr, s_gdp);

   s_pa=0; s_ab=0; s_r=0; s_rbi=0; s_bb=0; s_so=0; s_lob=0; s_h=0; s_s=0; s_d=0; s_t=0; s_hr=0; s_gdp=0;
   printf("Home Team:\n");
   printf("%3s %3s %3s %3s %3s %3s %3s %3s %3s %3s %3s %3s %3s %3s\n",
     "#", "PA", "AB", "R", "H", "RBI", "BB", "SO", "LOB", "S", "D", "T", "HR", "GDP");
   for(i=0; i<9; i++) {
      printf("%3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d\n",
         i+1, hstat[i].pa, hstat[i].ab, hstat[i].r, hstat[i].h, hstat[i].rbi,
         hstat[i].bb, hstat[i].so, hstat[i].lob, hstat[i].s, hstat[i].d,
         hstat[i].t, hstat[i].hr, hstat[i].gdp);
      s_pa += hstat[i].pa;
      s_ab += hstat[i].ab;
      s_r += hstat[i].r;
      s_h += hstat[i].h;
      s_rbi += hstat[i].rbi;
      s_bb += hstat[i].bb;
      s_so += hstat[i].so;
      s_lob += hstat[i].lob;
      s_s += hstat[i].s;
      s_d += hstat[i].d;
      s_t += hstat[i].t;
      s_hr += hstat[i].hr;
      s_gdp += hstat[i].gdp;
   }
   printf("TOT %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d\n",
      s_pa, s_ab, s_r, s_h, s_rbi, s_bb, s_so, s_lob, s_s, s_d, s_t, s_hr, s_gdp);
}

void match(void) {
   int i = 0;
   int ar = 0, alo = 0, ali_base = 100, ali = 100, *ahiP = NULL, *ariP = NULL;
   int hr = 0, hlo = 0, hli_base = 200, hli = 200, *hhiP = NULL, *hriP = NULL;
   struct statdata astat[9] = { 0 };
   struct statdata hstat[9] = { 0 };
   struct probdata ap[9];
   struct probdata hp[9];
   int n = sizeof(ap[0].p)/sizeof(int);
   int c = 2;
   int alob, hlob;

   initmem(c, &ahiP, &ariP, &hhiP, &hriP);
   for (i=0; i<9; i++) {
      initrand(ap[i].p, n);
      initrand(hp[i].p, n);
   }

   i = 0;
   do {
      if (i >= c) {
         printf("initmem: i:%d, c:%d\n", i, c);
         c += c/2;
         initmem(c, &ahiP, &ariP, &hhiP, &hriP);
      }

      side(ap, n, i,0,0,&ariP[i], &ahiP[i], &ali, ali_base, astat, &alob);
      printf("i:%d, ariP[i]:%d, ahiP[i]:%d, alob:%d\n", i, ariP[i], ahiP[i], alob);
      alo = alo + alob;
      ar = ar + ariP[i];
      if (i>=8 && hr > ar) {
         hriP[i] = -1;
         break;
      }
      printf("end of side: top, i:%d, ar:%d, hr:%d\n", i, ar, hr);

      side(hp, n, i,1,hr-ar,&hriP[i], &hhiP[i], &hli, hli_base, hstat, &hlob);
      printf("i:%d, hriP[i]:%d, hhiP[i]:%d, hlob:%d\n", i, hriP[i], hhiP[i], hlob);
      hlo = hlo + hlob;
      hr = hr + hriP[i];
      printf("end of side: bottom, i:%d, ar:%d, hr:%d\n", i, ar, hr);

      if (i>=8) {
         if (hr == ar) {
            printf("extra: i:%d, ar:%d, hr:%d\n", i, ar, hr);
         }
         else {
            printf("end of game: i:%d, ar:%d, hr:%d\n", i, ar, hr);
            break;
         }
      }
      i++;
   }
   while (1);

   linescore(i,ahiP,ariP,ar,alo,hhiP,hriP,hr,hlo);
   boxscore(astat,hstat);
}

int main(int argc, char *argv[]) {

   if (argc > 1) {
      int v = atoi(argv[1]);
      if (v == -1) {
         test_advance(argc, argv);
      }
      else {
         initialize(v);
         match();
      }
   }
   else {
      initialize(-1);
      match();
   }

   exit(0);
}
