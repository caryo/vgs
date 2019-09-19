#include <assert.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

struct statdata {
   int pa;
   int ab;
   int h;
   int s;
   int d;
   int t;
   int hr;
   int bb;
   int so;
   int gdp;
};

int    advance(int a, int *b, int *c, int *r1, int *r2);
int    advance_x(int a, int x, int *b, int *c, int *r1, int *r2);
void   initialize(int seed);
void   test_advance(int argc, char *argv[]);
char * result_code(int z);
int    result(int z, int a, int *o, int *h, int *r, int *c, int *gdp);
void   stat(struct statdata stats[], int li, int result, int gdp);
#if defined(USE_DICE)
int    roll(void);
void   side(int i, int b, int d, int *r, int *h, int *li,
            struct statdata statp[]);
#else
long   myround(double x);
void   initrand(int p[], int n);
int    maprand(int p[], int n, int z0);
int    genrand(int p[], int n);
void   side(int p[], int n, int i, int b, int d, int *r, int *h, int *li,
            struct statdata statp[]);
#endif
void   initmem(int c, int *ahiP[], int *ariP[], int *hhiP[], int *hriP[]);
void   match(void);

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

int result(int z, int a, int *o, int *h, int *r, int *c, int *gdp) {
   int x, f, w;
   int b, r1, r2;

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

   if (w == 1) {
      advance(a, &b, c, &r1, &r2);
      *r = *r + r2;
   }
   else if (x >= 0) {
      advance_x(a, x, &b, c, &r1, &r2);
      *r = *r + r2;
   }
   else {
      if (x == -2) {
         if (*c > 0) {
            if (*c & 0x01) {
               *c = *c & ~(0x01);
            }
            else if (*c & 0x02) {
               *c = *c & ~(0x02);
            }
            else if (*c & 0x04) {
               *c = *c & ~(0x04);
            }
            *o = *o + 1;
            if (*o < 3) {
               *o = *o + 1;
               *gdp = 1;
            }
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

#if defined(USE_DICE)
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
#endif

#if !defined(USE_DICE)
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
   int z, z0;

   z0 = (rand() % 1000) + 1;

   z = maprand(p,n,z0);
   z += 2;
#if DEBUG
   printf("genrand: z0: %4d, z: %2d\n", z0, z);
#endif

   return z;
}
#endif

void stat(struct statdata stats[], int li, int result, int gdp) {
   int idx = li % 9;
   switch (result) {
      case 2:
      break;
      case 3:
         (stats[idx].pa)++;
         (stats[idx].ab)++;
         (stats[idx].h)++;
         (stats[idx].t)++;
      break;
      case 4:
         (stats[idx].pa)++;
         (stats[idx].ab)++;
         (stats[idx].h)++;
         (stats[idx].d)++;
      break;
      case 5:
         (stats[idx].pa)++;
         (stats[idx].bb)++;
      break;
      case 6:
      case 7:
         (stats[idx].pa)++;
         (stats[idx].ab)++;
      break;
      case 8:
         (stats[idx].pa)++;
         (stats[idx].ab)++;
         (stats[idx].h)++;
         (stats[idx].s)++;
      break;
      case 9:
         (stats[idx].pa)++;
         (stats[idx].ab)++;
         (stats[idx].so)++;
      break;
      case 10:
      break;
      case 11:
         (stats[idx].pa)++;
         (stats[idx].ab)++;
         (stats[idx].gdp) += gdp;
      break;
      case 12:
         (stats[idx].pa)++;
         (stats[idx].ab)++;
         (stats[idx].hr)++;
      break;
      default:
      break;
   }
}

#if !defined(USE_DICE)
void side(int p[], int n, int i, int b, int d, int *r, int *h, int *li,
          struct statdata statp[])
#else
void side(int i, int b, int d, int *r, int *h, int *li,
          struct statdata statp[])
#endif
{
   int z;
   int o;
   int a, c;
   int gdp;

   o = 0;
   c = 0;

   do {
      a =  c;

#if !defined(USE_DICE)
      z = genrand(p, n);
#else
      z = roll();
#endif

#if DEBUG
      printf("a=0x%02x o=%d h=%2d r=%2d z=%2d\n", a, o, *h, *r, z);
#endif

      result(z, a, &o, h, r, &c, &gdp);

      printf("%02d: z:%d rc:%s\n", *li, z, result_code(z));
      stat(statp,*li,z,gdp);

      if (z != 2 && z != 10) {
         *li = *li + 1;
      }

      if (i >= 8 && b == 1 && d + *r > 0) {
         printf("i:%d, b:%d, o:%d, d:%d, *r:%d\n", i, b, o, d, *r);
         printf("walk-off win!\n");
         break;
      }
   } while (o<3);

#if DEBUG
   printf("a=0x%02x o=%d h=%2d r=%2d\n", a, o, *h, *r);
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

void match(void) {
   int i = 0;
   int ar = 0, ah = 0, ali = 0, *ahiP = NULL, *ariP = NULL;
   int hr = 0, hh = 0, hli = 0, *hhiP = NULL, *hriP = NULL;
   struct statdata astat[9] = { 0 };
   struct statdata hstat[9] = { 0 };
#if !defined(USE_DICE)
   int p[10];
   int n = sizeof(p)/sizeof(int);
#endif
   int in = 9;
   int c = 2;
   int s_pa, s_ab, s_h, s_s, s_d, s_t, s_hr, s_bb, s_so, s_gdp;

   initmem(c, &ahiP, &ariP, &hhiP, &hriP);
#if !defined(USE_DICE)
   initrand(p, n);
#endif

   do {
      if (i >= c) {
         printf("initmem: i:%d, c:%d\n", i, c);
         c += c/2;
         initmem(c, &ahiP, &ariP, &hhiP, &hriP);
      }

#if !defined(USE_DICE)
      side(p, n, i,0,0,&ariP[i], &ahiP[i], &ali, astat);
#else
      side(i,0,0,&ariP[i], &ahiP[i], &ali, astat);
#endif
      printf("i:%d, ariP[i]:%d, ahiP[i]:%d\n", i, ariP[i], ahiP[i]);
      ar = ar + ariP[i];
      if (i>=8 && hr > ar) {
         hriP[i] = -1;
         break;
      }
      printf("end of side: top, i:%d, ar:%d, hr:%d\n", i, ar, hr);

#if !defined(USE_DICE)
      side(p, n, i,1,hr-ar,&hriP[i], &hhiP[i], &hli, hstat);
#else
      side(i,1,hr-ar,&hriP[i], &hhiP[i], &hli, hstat);
#endif
      printf("i:%d, hriP[i]:%d, hhiP[i]:%d\n", i, hriP[i], hhiP[i]);
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

   in = i+1;

   printf("T: ");
   for (i=0; i<in; i++) {
      printf("%2d ", i+1);
      if (((i+1) % 3) == 0) {
         printf(" ");
      }
   }
   printf("%5c %2c %2c\n", 'R', 'H', 'E');

   printf("A: ");
   for (i=0; i<in; i++) {
      printf("%2d ", ariP[i]);
      if (((i+1) % 3) == 0) {
         printf(" ");
      }
      ah = ah + ahiP[i];
   }
   printf("%5d %2d %2d\n", ar, ah, 0);
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
   printf("%5d %2d %2d\n", hr, hh, 0);

   s_pa=0; s_ab=0; s_h=0; s_s=0; s_d=0; s_t=0; s_hr=0; s_bb=0; s_so=0; s_gdp=0;
   printf("Away Team:\n");
   printf("%3s %3s %3s %3s %3s %3s %3s %3s %3s %3s %3s\n",
     "#", "PA", "AB", "H", "S", "D", "T", "HR", "BB", "SO", "GDP");
   for(i=0; i<9; i++) {
      printf("%3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d\n",
         i, astat[i].pa, astat[i].ab, astat[i].h, astat[i].s,
         astat[i].d, astat[i].t, astat[i].hr, astat[i].bb,
         astat[i].so, astat[i].gdp);
      s_pa += astat[i].pa;
      s_ab += astat[i].ab;
      s_h += astat[i].h;
      s_s += astat[i].s;
      s_d += astat[i].d;
      s_t += astat[i].t;
      s_hr += astat[i].hr;
      s_bb += astat[i].bb;
      s_so += astat[i].so;
      s_gdp += astat[i].gdp;
   }
   printf("TOT %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d\n",
      s_pa, s_ab, s_h, s_s, s_d, s_t, s_hr, s_bb, s_so, s_gdp);

   s_pa=0; s_ab=0; s_h=0; s_s=0; s_d=0; s_t=0; s_hr=0; s_bb=0; s_so=0; s_gdp=0;
   printf("Home Team:\n");
   printf("%3s %3s %3s %3s %3s %3s %3s %3s %3s %3s %3s\n",
     "#", "PA", "AB", "H", "S", "D", "T", "HR", "BB", "SO", "GDP");
   for(i=0; i<9; i++) {
      printf("%3d %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d\n",
         i, hstat[i].pa, hstat[i].ab, hstat[i].h, hstat[i].s,
         hstat[i].d, hstat[i].t, hstat[i].hr, hstat[i].bb,
         hstat[i].so, hstat[i].gdp);
      s_pa += hstat[i].pa;
      s_ab += hstat[i].ab;
      s_h += hstat[i].h;
      s_s += hstat[i].s;
      s_d += hstat[i].d;
      s_t += hstat[i].t;
      s_hr += hstat[i].hr;
      s_bb += hstat[i].bb;
      s_so += hstat[i].so;
      s_gdp += hstat[i].gdp;
   }
   printf("TOT %3d %3d %3d %3d %3d %3d %3d %3d %3d %3d\n",
      s_pa, s_ab, s_h, s_s, s_d, s_t, s_hr, s_bb, s_so, s_gdp);
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
