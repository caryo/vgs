#include <assert.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define NUM_BATTERS  9
#define NUM_PITCHERS 6

#define NUM_TEAMS    2
#define NUM_LEAGUES  1

struct batdata {
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
   int hbp;
   int sh;
   int sf;
   int out;
};

struct bstatdata {
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
   int hbp;
   int sf;
};

struct pstatdata {
   int ip;
   int ip_f;
   int w;
   int l;
};

struct probdata {
   int p[12];
};

struct team_data {
   struct batdata   bat[NUM_BATTERS];
   struct batdata   pit[NUM_PITCHERS];
   struct probdata  batp[NUM_BATTERS];
   struct bstatdata b_stat[NUM_BATTERS];
   struct bstatdata p_bstat[NUM_PITCHERS];
   struct pstatdata p_pstat[NUM_PITCHERS];
};

struct league_data {
   struct batdata   bat;
   struct batdata   pit;
};

struct game_data {
   struct team_data   away;
   struct team_data   home;
   struct league_data league;
};

void   onbase(int ob[], int li);
int    offbase(int ob[]);
int    advance(int a, int *b, int *c, int *r1, int *r2);
int    advance_x(int a, int x, int *b, int *c, int *r1, int *r2);
void   initialize(int seed);
void   test_advance(int argc, char *argv[]);
char * result_code(int z);
int    result(int z, int a, int *o, int *h, int *r, int *c, int *gdp, int idx, int ob[], int rli[], int *rlo);
void   statrun(struct bstatdata stats[], int idx, int result);
void   stat(struct bstatdata stats[], int idx, int result, int gdp, int rbi, int lob);
int    roll(void);
long   myround(double x);
void   defaultprob(int p[]);
void   setprob(
               struct team_data team[], struct league_data league[],
               int aTeamIdx, int aLeagueIdx, int batIdx,
               int bTeamIdx, int bLeagueIdx, int pitIdx);
void   initrand(
                struct team_data team[], struct league_data league[],
                int aTeamIdx, int aLeagueIdx, int batIdx,
                int bTeamIdx, int bLeagueIdx, int pitIdx);
int    maprand(int p[], int n, int z0);
int    genrand(int p[], int n);
void   side(
            struct probdata p[],
            int n,
            int i, int b, int d, int *r, int *h, int *li,
            int li_base, struct bstatdata batstat[], struct bstatdata pitbstat[],
            struct pstatdata pitpstat[], int pitIdx, int *lob);
void   initmem(int c, int *ahiP[], int *ariP[], int *hhiP[], int *hriP[]);
void   clearmem(int idx, int c, int ahiP[], int ariP[], int hhiP[], int hriP[]);
void   linescore(int i, char *aName, char *hName, int ahiP[], int ariP[],
                 int ar, int alo, int hhiP[],
                 int hriP[], int hr, int hlo, int g, int *awP, int *hwP);
void   boxscore(char *name, struct bstatdata batstat[], struct bstatdata pitbstat[],
                struct pstatdata pitpstat[],
                struct team_data team[], int teamIdx);
void   addstat(struct bstatdata g_stat[], struct bstatdata s_stat[], int n);
void   addpstat(struct pstatdata g_stat[], struct pstatdata s_stat[], int n);
void   match(int g, char *aName, char *hName,
             struct team_data team[], struct league_data league[],
             int aTeamIdx, int aLeagueIdx, int hTeamIdx, int hLeagueIdx,
             struct bstatdata abatstat[], struct bstatdata apitbstat[],
             struct pstatdata apitpstat[],
             struct bstatdata hstat[], struct bstatdata hpitbstat[],
             struct pstatdata hpitpstat[],
             int *awP, int *hwP);
void   matchset(int n, struct team_data team[], struct league_data league[]);
void   readvals(struct batdata *dataP);

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

void defaultprob(int p[]) {
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
   p[10] = p[9];
   p[11] = p[9];
}

int log5calc(double b, double bl, double p, double pl) {
   double p0, pl0 = (bl + pl) / 2;
#if DEBUG
   printf("b:%f, bl:%f, p:%f, pl:%f, pl0:%f\n", b, bl, p, pl, pl0);
#endif
   if (b == 0 && p == 0) {
      p0 = 0;
   }
   else if (b > 0 && p == 0) {
      p0 = b;
   }
   else if (b == 0 && p > 0) {
      p0 = p;
   }
   else {
      p0 = ((b*p)/pl0) / (((b*p)/pl0) + (1-b) * (1-p) / (1-pl0));
   }
   return p0 * 1000;
}

void setprob(
             struct team_data team[], struct league_data league[],
             int aTeamIdx, int aLeagueIdx, int batIdx,
             int bTeamIdx, int bLeagueIdx, int pitIdx)
{
   int t = 0, adji;
   double adj;
   int p0[10], p_hr, out;
   struct batdata *bat = &(team[aTeamIdx].bat[batIdx]);
   struct batdata *pitP = &(team[bTeamIdx].pit[pitIdx]);
   struct batdata *lbat = &(league[aLeagueIdx].bat);
   struct batdata *lpit = &(league[bLeagueIdx].pit);
   int            *p = team[aTeamIdx].batp[batIdx].p;

   p0[0] = 0;
   p0[1] = log5calc((double)bat->t/bat->pa, (double)lbat->t/lbat->pa, (double)pitP->t/pitP->pa, (double)lpit->t/lpit->pa);
   t += p0[1];
   p0[2] = log5calc((double)bat->d/bat->pa, (double)lbat->d/lbat->pa, (double)pitP->d/pitP->pa, (double)lpit->d/lpit->pa);
   t += p0[2];
   p0[3] = log5calc((double)bat->bb/bat->pa, (double)lbat->bb/lbat->pa, (double)pitP->bb/pitP->pa, (double)lpit->bb/lpit->pa);
   t += p0[3];
   out = log5calc((double)bat->out/bat->pa, (double)lbat->out/lbat->pa, (double)pitP->out/pitP->pa, (double)lpit->out/lpit->pa);
   p0[4] = out/2;
   t += p0[4];
   p0[5] = out/2;
   t += p0[5];
   p0[6] = log5calc((double)bat->s/bat->pa, (double)lbat->s/lbat->pa, (double)pitP->s/pitP->pa, (double)lpit->s/lpit->pa);
   t += p0[6];
   p0[7] = log5calc((double)bat->so/bat->pa, (double)lbat->so/lbat->pa, (double)pitP->so/pitP->pa, (double)lpit->so/lpit->pa);
   t += p0[7];
   p0[8] = p0[7];
   p0[9] = log5calc((double)bat->gdp/bat->pa, (double)lbat->gdp/lbat->pa, (double)pitP->gdp/pitP->pa, (double)lpit->gdp/lpit->pa);
   t += p0[9];
   p_hr = log5calc((double)bat->hr/bat->pa, (double)lbat->hr/lbat->pa, (double)pitP->hr/pitP->pa, (double)lpit->hr/lpit->pa);
   t += p_hr;

   adj = (double) t / 1000;

#if DEBUG
   printf("before: p0[0]:%3d p0[1]:%3d p0[2]:%3d p0[3]:%3d p0[4]:%3d p0[5]:%3d "
          "p0[6]:%3d p0[7]:%3d p0[8]:%3d p0[9]:%3d p_hr:%3d t:%4d adj:%4.3f out:%3d out/2:%3d\n",
     p0[0], p0[1], p0[2], p0[3], p0[4], p0[5],
     p0[6], p0[7], p0[8], p0[9], p_hr, t, adj, out, out/2);
#endif

   t = 0;

   p0[0] = 0;
   t += p0[0];
   p0[1] = myround((double) p0[1] / adj);
   t += p0[1];
   p0[2] = myround((double) p0[2] / adj);
   t += p0[2];
   p0[3] = myround((double) p0[3] / adj);
   t += p0[3];
   p0[4] = myround((double) p0[4] / adj);
   t += p0[4];
   p0[5] = myround((double) p0[5] / adj);
   t += p0[5];
   p0[6] = myround((double) p0[6] / adj);
   t += p0[6];
   p0[7] = myround((double) p0[7] / adj);
   t += p0[7];
   p0[8] = p0[7];
   p0[9] = myround((double) p0[9] / adj);
   t += p0[9];
   p_hr  = myround((double) p_hr  / adj);
   t += p_hr;
   out   = myround((double) out   / adj);

#if DEBUG
   printf("after0: p0[0]:%3d p0[1]:%3d p0[2]:%3d p0[3]:%3d p0[4]:%3d p0[5]:%3d "
          "p0[6]:%3d p0[7]:%3d p0[8]:%3d p0[9]:%3d p_hr:%3d t:%4d, out:%3d out/2:%3d\n",
     p0[0], p0[1], p0[2], p0[3], p0[4], p0[5],
     p0[6], p0[7], p0[8], p0[9], p_hr, t, out, out/2);
#endif

   adji = 1000-t;
   if (adji != 0) {
      out += adji;
      p0[4] += adji / 2;
      p0[5] += adji / 2;
      p0[5] += adji % 2;
   }
   t += adji;

#if DEBUG
   printf("after1: p0[0]:%3d p0[1]:%3d p0[2]:%3d p0[3]:%3d p0[4]:%3d p0[5]:%3d "
          "p0[6]:%3d p0[7]:%3d p0[8]:%3d p0[9]:%3d p_hr:%3d t:%4d, out:%3d out/2:%3d\n",
     p0[0], p0[1], p0[2], p0[3], p0[4], p0[5],
     p0[6], p0[7], p0[8], p0[9], p_hr, t, out, out/2);
#endif

   t = 0;
   p[0] = p0[0];
   t += p[0];
   p[1] = t + p0[1];
   t += p0[1];
   p[2] = t + p0[2];
   t += p0[2];
   p[3] = t + p0[3];
   t += p0[3];
   p[4] = t + p0[4];
   t += p0[4];
   p[5] = t + p0[5];
   t += p0[5];
   p[6] = t + p0[6];
   t += p0[6];
   p[7] = t + p0[7];
   t += p0[7];
   p[8] = p[7];
   p[9] = t + p0[9];
   t += p0[9];

   p[10] = t;
   p[11] = t;
}

void initrand(
              struct team_data team[], struct league_data league[],
              int aTeamIdx, int aLeagueIdx, int batIdx,
              int bTeamIdx, int bLeagueIdx, int pitIdx)
{
#if DEBUG
   int i;
#endif
   if (team == NULL) {
   }
   else {
      setprob(
              team, league,
              aTeamIdx, aLeagueIdx, batIdx,
              bTeamIdx, bLeagueIdx, pitIdx);
   }

#if DEBUG
   for (i=0; i<n; i++) {
      printf("i:%2d, p[%d]: %4d, pitIdx:%d\n", i, i, p[i], pitIdx);
   }
   printf("\n");
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

void side(
          struct probdata p[],
          int n,
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

      printf("%02d (%d): i:%-2d o:%d gdp:%d rlo:%d rbi:%d rs:%d z:%-2d rc:%s\n",
        *li, idx, i+1, o, gdp, rlo, rbi, rs, z, result_code(z));
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

void clearmem(int idx, int c, int ahiP[], int ariP[], int hhiP[], int hriP[]) {
   printf("idx:%d, c:%d\n", idx, c);
   memset(ahiP+idx,0,sizeof(int)*c);
   memset(ariP+idx,0,sizeof(int)*c);
   memset(hhiP+idx,0,sizeof(int)*c);
   memset(hriP+idx,0,sizeof(int)*c);
}

void linescore(int i, char *aName, char *hName, int ahiP[], int ariP[], int ar, int alo,
               int hhiP[], int hriP[], int hr, int hlo,
               int g, int *awP, int *hwP)
{
   int ah = 0, hh = 0;
   int in = i+1;

   if (ar > hr) {
      (*awP)++;
   }
   else if (hr > ar) {
      (*hwP)++;
   }

   printf("%-20s","Team:");
   for (i=0; i<in; i++) {
      printf("%2d ", i+1);
      if (((i+1) % 3) == 0) {
         printf(" ");
      }
   }
   printf("%5c %2c %2c %5s\n", 'R', 'H', 'E', "LOB");

   printf("%-8s (%3d-%3d): ", aName, *awP, ((g+1)-*awP));
   for (i=0; i<in; i++) {
      printf("%2d ", ariP[i]);
      if (((i+1) % 3) == 0) {
         printf(" ");
      }
      ah = ah + ahiP[i];
   }
   printf("%5d %2d %2d %5d\n", ar, ah, 0, alo);
   printf("%-8s (%3d-%3d): ", hName, *hwP, ((g+1)-*hwP));
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

void boxscore(char *name, struct bstatdata batstat[], struct bstatdata pitbstat[],
              struct pstatdata pitpstat[],
              struct team_data team[], int teamIdx)
{
   int i;
   int pa, ab, r, h, rbi, bb, so, lob, s, d, t, hr, gdp, hbp, sf;
   int ab0, h0, bb0, hbp0, sf0;
   int t_ab0 = 0, t_h0 = 0, t_bb0 = 0, t_hbp0 = 0, t_sf0 = 0;
   double gavg = 0., savg = 0., gobp = 0., sobp = 0., era = 0., ipd;
   int w, l, ip, ip_f;

   pa=0; ab=0; r=0; h=0; rbi=0; bb=0; so=0; lob=0; s=0; d=0; t=0; hr=0; gdp=0; hbp=0; sf=0;
   printf("%s Team:\n", name);
   printf("%4s %4s %4s %4s %4s %4s %4s %4s %4s %4s %4s %4s %4s %4s %6s %6s %6s %6s\n",
     "#", "PA", "AB", "R", "H", "RBI", "BB", "SO", "LOB", "S", "D", "T", "HR",
     "GDP", "AVG", "SAVG", "OBP", "SOBP");
   for(i=0; i<NUM_BATTERS; i++) {
      if (batstat[i].ab > 0) {
         gavg = (double) batstat[i].h / batstat[i].ab;
      }
      else {
         gavg = 0.;
      }
      if (team)
      {
         ab0 = team[teamIdx].b_stat[i].ab;
         h0 = team[teamIdx].b_stat[i].h;
         bb0 = team[teamIdx].b_stat[i].bb;
         hbp0 = team[teamIdx].b_stat[i].hbp;
         sf0 = team[teamIdx].b_stat[i].sf;
         t_ab0 += ab0;
         t_h0 += h0;
         t_bb0 += bb0;
         t_hbp0 += hbp0;
         t_sf0 += sf0;
      }
      else {
         ab0 = 0;
         h0 = 0;
         bb0 = 0;
         hbp0 = 0;
         sf0 = 0;
         t_ab0 = 0;
         t_h0 = 0;
         t_bb0 = 0;
         t_hbp0 = 0;
         t_sf0 = 0;
      }
      ab0 += batstat[i].ab;
      h0 += batstat[i].h;
      bb0 += batstat[i].bb;
      hbp0 += batstat[i].hbp;
      sf0 += batstat[i].sf;
      if (ab0 > 0) {
         savg = (double) h0 / ab0;
      }
      else {
         savg = 0.;
      }
      if ((batstat[i].ab + batstat[i].bb + batstat[i].hbp + batstat[i].sf) > 0) {
         gobp = (double) (batstat[i].h + batstat[i].bb + batstat[i].hbp) /
           (batstat[i].ab + batstat[i].bb + batstat[i].hbp + batstat[i].sf);
      }
      else {
         gobp = 0.;
      }
      if ((ab0 + bb0 + hbp0 + sf0) > 0) {
         sobp = (double) (h0 + bb0 + hbp0) / (ab0 + bb0 + hbp0 + sf0);
      }
      else {
         sobp = 0.;
      }
      printf("%4d %4d %4d %4d %4d %4d %4d %4d %4d %4d %4d %4d %4d %4d %6.3f %6.3f %6.3f %6.3f\n",
         i+1, batstat[i].pa, batstat[i].ab, batstat[i].r, batstat[i].h, batstat[i].rbi,
         batstat[i].bb, batstat[i].so, batstat[i].lob, batstat[i].s, batstat[i].d,
         batstat[i].t, batstat[i].hr, batstat[i].gdp, gavg, savg, gobp, sobp);
      pa  += batstat[i].pa;
      ab  += batstat[i].ab;
      r   += batstat[i].r;
      h   += batstat[i].h;
      rbi += batstat[i].rbi;
      bb  += batstat[i].bb;
      so  += batstat[i].so;
      lob += batstat[i].lob;
      s   += batstat[i].s;
      d   += batstat[i].d;
      t   += batstat[i].t;
      hr  += batstat[i].hr;
      gdp += batstat[i].gdp;
   }

   if (ab > 0) {
      gavg = (double) h / ab;
   }
   else {
      gavg = 0.;
   }
   if ((ab+bb+hbp+sf) > 0) {
      gobp = (double) (h + bb + hbp) / (ab + bb + hbp + sf);
   }
   else {
      gobp = 0.;
   }
   ab0 = t_ab0 + ab;
   h0 = t_h0 + h;
   bb0 = t_bb0 + bb;
   hbp0 = t_hbp0 + hbp;
   sf0 = t_sf0 + sf;
   if (ab0 > 0) {
      savg = (double) h0 / ab0;
   }
   else {
      savg = 0.;
   }
   if ((ab0 + bb0 + hbp0 + sf0) > 0) {
      sobp = (double) (h0 + bb0 + hbp0) / (ab0 + bb0 + hbp0 + sf0);
   }
   else {
      sobp = 0.;
   }

   printf("%-4s %4d %4d %4d %4d %4d %4d %4d %4d %4d %4d %4d %4d %4d %6.3f %6.3f %6.3f %6.3f\n",
      "TOT", pa, ab, r, h, rbi, bb, so, lob, s, d, t, hr, gdp, gavg, savg, gobp, sobp);

   pa=0; ab=0; r=0; h=0; rbi=0; bb=0; so=0; lob=0; s=0; d=0; t=0; hr=0; gdp=0; hbp=0; sf=0;
   gavg = 0.; savg = 0.; w = 0; l = 0; ip = 0; ip_f = 0; era = 0.;

   printf("%4s %4s %4s %4s %4s %4s %4s %4s %4s %4s %4s %4s %4s %4s %6s %6s %6s %7s %6s\n",
     "#", "PA", "AB", "R", "H", "RBI", "BB", "SO", "LOB", "S", "D", "T", "HR", "GDP",
     "AVG", "SAVG", "IP", "W-L", "ERA");
   for (i=0; i<NUM_PITCHERS; i++) {
      if (pitbstat[i].ab > 0) {
         gavg = (double) pitbstat[i].h / pitbstat[i].ab;
      }
      else {
         gavg = 0.;
      }
      if (team)
      {
         ab0 = team[teamIdx].p_bstat[i].ab;
         h0 = team[teamIdx].p_bstat[i].h;
         bb0 = team[teamIdx].p_bstat[i].bb;
         hbp0 = team[teamIdx].p_bstat[i].hbp;
         sf0 = team[teamIdx].p_bstat[i].sf;
         t_ab0 += ab0;
         t_h0 += h0;
         t_bb0 += bb0;
         t_hbp0 += hbp0;
         t_sf0 += sf0;
      }
      else {
         ab0 = 0;
         h0 = 0;
         bb0 = 0;
         hbp0 = 0;
         sf0 = 0;
         t_ab0 = 0;
         t_h0 = 0;
         t_bb0 = 0;
         t_hbp0 = 0;
         t_sf0 = 0;
      }
      ab0 += pitbstat[i].ab;
      h0 += pitbstat[i].h;
      bb0 += pitbstat[i].bb;
      hbp0 += pitbstat[i].hbp;
      sf0 += pitbstat[i].sf;
      if (ab0 > 0) {
         savg = (double) h0 / ab0;
      }
      else {
         savg = 0.;
      }
      ipd = (double) pitpstat[i].ip + (double) pitpstat[i].ip_f / 3;
      if (ipd > 0.) {
         era = 9.0 * (pitbstat[i].r / ipd);
      }
      else {
         era = 0.;
      }
      printf("%4d %4d %4d %4d %4d %4d %4d %4d %4d %4d %4d %4d %4d %4d %6.3f %6.3f %4d.%d %3d-%3d %6.2f\n",
         i+1, pitbstat[i].pa, pitbstat[i].ab, pitbstat[i].r, pitbstat[i].h, pitbstat[i].rbi,
         pitbstat[i].bb, pitbstat[i].so, pitbstat[i].lob, pitbstat[i].s, pitbstat[i].d,
         pitbstat[i].t, pitbstat[i].hr, pitbstat[i].gdp, gavg, savg,
         pitpstat[i].ip, pitpstat[i].ip_f, pitpstat[i].w, pitpstat[i].l, era);

      pa  += pitbstat[i].pa;
      ab  += pitbstat[i].ab;
      r   += pitbstat[i].r;
      h   += pitbstat[i].h;
      rbi += pitbstat[i].rbi;
      bb  += pitbstat[i].bb;
      so  += pitbstat[i].so;
      lob += pitbstat[i].lob;
      s   += pitbstat[i].s;
      d   += pitbstat[i].d;
      t   += pitbstat[i].t;
      hr  += pitbstat[i].hr;
      gdp += pitbstat[i].gdp;

      w += pitpstat[i].w;
      l += pitpstat[i].l;
      ip += pitpstat[i].ip;
      ip_f += pitpstat[i].ip_f;
      ip += ip_f / 3;
      ip_f = ip_f % 3;
   }

   if (ab > 0) {
      gavg = (double) h / ab;
   }
   else {
      gavg = 0.;
   }
   if ((ab+bb+hbp+sf) > 0) {
      gobp = (double) (h + bb + hbp) / (ab + bb + hbp + sf);
   }
   else {
      gobp = 0.;
   }
   ab0 = t_ab0 + ab;
   h0 = t_h0 + h;
   bb0 = t_bb0 + bb;
   hbp0 = t_hbp0 + hbp;
   sf0 = t_sf0 + sf;
   if (ab0 > 0) {
      savg = (double) h0 / ab0;
   }
   else {
      savg = 0.;
   }

   ipd = (double) ip + (double) ip_f / 3;
   if (ipd > 0.) {
      era = 9 * ((double) r / ipd);
   }
   else {
      era = 0.;
   }

   printf("%-4s %4d %4d %4d %4d %4d %4d %4d %4d %4d %4d %4d %4d %4d %6.3f %6.3f %4d.%d %3d-%3d %6.2f\n",
      "TOT", pa, ab, r, h, rbi, bb, so, lob, s, d, t, hr, gdp, gavg, savg, ip, ip_f, w, l, era);
}

void match(int g, char *aName, char *hName,
           struct team_data team[], struct league_data league[],
           int aTeamIdx, int aLeagueIdx, int hTeamIdx, int hLeagueIdx,
           struct bstatdata abatstat[], struct bstatdata apitbstat[],
           struct pstatdata apitpstat[],
           struct bstatdata hbatstat[], struct bstatdata hpitbstat[],
           struct pstatdata hpitpstat[],
           int *awP, int *hwP)
{
   int i = 0;
   int ar = 0, alo = 0, ali_base = 100, ali = 100, *ahiP = NULL, *ariP = NULL;
   int hr = 0, hlo = 0, hli_base = 200, hli = 200, *hhiP = NULL, *hriP = NULL;
   struct probdata dfltp[NUM_BATTERS];
   struct probdata *probP;
   int n = sizeof(dfltp[0].p)/sizeof(int);
   int c = 2;
   int alob = 0, hlob = 0;
   int pitIdx = g % 6;

   initmem(c, &ahiP, &ariP, &hhiP, &hriP);
   for (i=0; i<NUM_BATTERS; i++) {
      if ((team[aTeamIdx].bat[i].ab > 0 && team[hTeamIdx].pit[i].ab > 0) &&
          (team[hTeamIdx].bat[i].ab > 0 && team[aTeamIdx].pit[i].ab > 0))
      {
         initrand(
                  team, league,
                  aTeamIdx, aLeagueIdx, i,
                  hTeamIdx, hLeagueIdx, pitIdx);
         initrand(
                  team, league,
                  hTeamIdx, hLeagueIdx, i,
                  aTeamIdx, aLeagueIdx, pitIdx);
#if DEBUG
         printf("\n");
#endif
      }
      else {
         defaultprob(dfltp[i].p);
      }
   }

   probP = dfltp;

   i = 0;
   do {
      if (i >= c) {
         int idx = c, extra = c/2;
         c += c/2;
         printf("initmem: i:%d, c:%d\n", i, c);
         initmem(c, &ahiP, &ariP, &hhiP, &hriP);
         clearmem(idx,extra,ahiP,ariP,hhiP,hriP);
      }

      if (team) {
         probP = team[aTeamIdx].batp;
      }
      side(
           probP,
           n,
           i,0,0,&ariP[i], &ahiP[i], &ali, ali_base, abatstat, hpitbstat, hpitpstat, pitIdx, &alob);
      printf("i:%d, ariP[i]:%d, ahiP[i]:%d, alob:%d\n", i, ariP[i], ahiP[i], alob);
      alo = alo + alob;
      ar = ar + ariP[i];
      if (i>=8 && hr > ar) {
         hriP[i] = -1;
         break;
      }
      printf("end of side: top, i:%d, ar:%d, hr:%d\n", i, ar, hr);

      if (team) {
         probP = team[hTeamIdx].batp;
      }
      side(
           probP,
           n,
           i,1,hr-ar,&hriP[i], &hhiP[i], &hli, hli_base, hbatstat, apitbstat, apitpstat, pitIdx, &hlob);
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

   if (ar > hr) {
      apitpstat[pitIdx].w++;
      hpitpstat[pitIdx].l++;
   }
   else {
      hpitpstat[pitIdx].w++;
      apitpstat[pitIdx].l++;
   }

   printf("Game: %d\n", g+1);
   linescore(i,aName,hName,ahiP,ariP,ar,alo,hhiP,hriP,hr,hlo,g,awP,hwP);
   boxscore(aName,abatstat,apitbstat,apitpstat,team,aTeamIdx);
   boxscore(hName,hbatstat,hpitbstat,hpitpstat,team,hTeamIdx);
   free(ahiP);
   free(ariP);
   free(hhiP);
   free(hriP);
}

void addstat(struct bstatdata g_stat[], struct bstatdata s_stat[], int n) {
   int i;
   for (i=0; i<n; i++) {
      s_stat[i].pa  += g_stat[i].pa;
      s_stat[i].ab  += g_stat[i].ab;
      s_stat[i].r   += g_stat[i].r;
      s_stat[i].h   += g_stat[i].h;
      s_stat[i].rbi += g_stat[i].rbi;
      s_stat[i].bb  += g_stat[i].bb;
      s_stat[i].so  += g_stat[i].so;
      s_stat[i].lob += g_stat[i].lob;
      s_stat[i].s   += g_stat[i].s;
      s_stat[i].d   += g_stat[i].d;
      s_stat[i].t   += g_stat[i].t;
      s_stat[i].hr  += g_stat[i].hr;
      s_stat[i].gdp += g_stat[i].gdp;
      s_stat[i].hbp += g_stat[i].hbp;
      s_stat[i].sf  += g_stat[i].sf;
   }
}

void addpstat(struct pstatdata g_stat[], struct pstatdata s_stat[], int n) {
   int i, ip_f;
   for (i=0; i<n; i++) {
      s_stat[i].ip += g_stat[i].ip;
      ip_f = s_stat[i].ip_f;
      ip_f += g_stat[i].ip_f;
      s_stat[i].ip += ip_f / 3;
      s_stat[i].ip_f = ip_f % 3;
      s_stat[i].w += g_stat[i].w;
      s_stat[i].l += g_stat[i].l;
   }
}
void matchset(int n, struct team_data team[], struct league_data league[])
{
   struct bstatdata g_abatstat[NUM_BATTERS] = { 0 };
   struct bstatdata g_hbatstat[NUM_BATTERS] = { 0 };
   struct bstatdata g_apitbstat[NUM_PITCHERS] = { 0 };
   struct bstatdata g_hpitbstat[NUM_PITCHERS] = { 0 };
   struct pstatdata g_apitpstat[NUM_PITCHERS] = { 0 };
   struct pstatdata g_hpitpstat[NUM_PITCHERS] = { 0 };
   int i, aw = 0, hw = 0;
   int aTeamIdx, hTeamIdx;
   int aLeagueIdx, hLeagueIdx;

   for (i=0; i<n; i++) {
      memset(g_abatstat,0,sizeof(struct bstatdata)*NUM_BATTERS);
      memset(g_apitbstat,0,sizeof(struct bstatdata)*NUM_PITCHERS);
      memset(g_apitpstat,0,sizeof(struct pstatdata)*NUM_PITCHERS);

      memset(g_hbatstat,0,sizeof(struct bstatdata)*NUM_BATTERS);
      memset(g_hpitbstat,0,sizeof(struct bstatdata)*NUM_PITCHERS);
      memset(g_hpitpstat,0,sizeof(struct pstatdata)*NUM_PITCHERS);

      if (i%2 == 0) {
         aTeamIdx = 0;
         aLeagueIdx = 0;
         hTeamIdx = 1;
         hLeagueIdx = 0;
         match(i, "Blue", "Red",
               team, league, aTeamIdx, aLeagueIdx,
               hTeamIdx, hLeagueIdx,
               g_abatstat, g_apitbstat, g_apitpstat,
               g_hbatstat, g_hpitbstat, g_hpitpstat, &aw, &hw);
         addstat(g_abatstat, team[aTeamIdx].b_stat, NUM_BATTERS);
         addstat(g_apitbstat, team[aTeamIdx].p_bstat, NUM_PITCHERS);
         addpstat(g_apitpstat, team[aTeamIdx].p_pstat, NUM_PITCHERS);
         addstat(g_hbatstat, team[hTeamIdx].b_stat, NUM_BATTERS);
         addstat(g_hpitbstat, team[hTeamIdx].p_bstat, NUM_PITCHERS);
         addpstat(g_hpitpstat, team[hTeamIdx].p_pstat, NUM_PITCHERS);
      }
      else {
         aTeamIdx = 1;
         aLeagueIdx = 0;
         hTeamIdx = 0;
         hLeagueIdx = 0;
         match(i, "Red", "Blue",
               team, league, aTeamIdx, aLeagueIdx,
               hTeamIdx, hLeagueIdx,
               g_abatstat, g_apitbstat, g_apitpstat,
               g_hbatstat, g_hpitbstat, g_hpitpstat, &hw, &aw);
         addstat(g_hbatstat, team[hTeamIdx].b_stat, NUM_BATTERS);
         addstat(g_hpitbstat, team[hTeamIdx].p_bstat, NUM_PITCHERS);
         addpstat(g_hpitpstat, team[hTeamIdx].p_pstat, NUM_PITCHERS);
         addstat(g_abatstat, team[aTeamIdx].b_stat, NUM_BATTERS);
         addstat(g_apitbstat, team[aTeamIdx].p_bstat, NUM_PITCHERS);
         addpstat(g_apitpstat, team[aTeamIdx].p_pstat, NUM_PITCHERS);
      }
   }
   printf("\n");
   boxscore("Blue",team[0].b_stat, team[0].p_bstat, team[0].p_pstat, NULL, 0);
   boxscore("Red", team[1].b_stat, team[1].p_bstat, team[1].p_pstat, NULL, 0);
}

void readvals(struct batdata *dataP) {
   char inbuf[132], *pInbuf;
   char inValsBuf[132] = { 0 };
   int n1, n2;

   pInbuf = fgets(inbuf,sizeof(inbuf),stdin);
   assert(pInbuf);
#if DEBUG
   printf("inbuf:%s\n", inbuf);
#endif
   n1 = sscanf(pInbuf,"%s\n",inValsBuf);
#if DEBUG
   printf("n1:%d\n", n1);
#endif
   assert(n1 == 1);
#if DEBUG
   printf("inValsBuf:%s\n", inValsBuf);
#endif
   n2 = sscanf(inValsBuf,"PA:%d,AB:%d,H:%d,S:%d,D:%d,T:%d,HR:%d,BB:%d,"
     "SO:%d,GDP:%d,HBP:%d,SH:%d,SF:%d,OUT:%d\n", &dataP->pa, &dataP->ab,
     &dataP->h, &dataP->s, &dataP->d, &dataP->t, &dataP->hr, &dataP->bb,
     &dataP->so, &dataP->gdp, &dataP->hbp, &dataP->sh, &dataP->sf, &dataP->out);
#if DEBUG
   printf("n2:%d\n", n2);
#endif
   assert(n2 == 14);
#if DEBUG
   printf("PA:%d,AB:%d,H:%d,S:%d,D:%d,T:%d,HR:%d,BB:%d,SO:%d,GDP:%d,HBP:%d,"
     "SH:%d,SF:%d,OUT:%d\n", dataP->pa, dataP->ab, dataP->h, dataP->s,
     dataP->d, dataP->t, dataP->hr, dataP->bb, dataP->so, dataP->gdp,
     dataP->hbp, dataP->sh, dataP->sf, dataP->out);
#endif
}

int main(int argc, char *argv[]) {
   struct team_data   team[NUM_TEAMS] = { 0 };
   struct league_data league[NUM_LEAGUES] = { 0 };

   if (argc > 1) {

      int v = atoi(argv[1]);
      if (v == -1) {
         test_advance(argc, argv);
      }
      else {
         initialize(v);
         if (argc > 2) {
#if !defined(USE_DICE)
            int i, j;

            for (j=0; j<NUM_TEAMS; j++) {
               for (i=0; i<NUM_BATTERS; i++) {
                  readvals(&(team[j].bat[i]));
               }
            }
            readvals(&(league[0].bat));
            for (j=0; j<NUM_TEAMS; j++) {
               for (i=0; i<NUM_PITCHERS; i++) {
                  readvals(&(team[j].pit[i]));
               }
            }
            readvals(&(league[0].pit));

            printf("using input values to build probability tables\n");
            matchset(162, team, league);
#else
            printf("using dice to build probability tables\n");
            matchset(162, team, league);
#endif
         }
         else {
            printf("using dice to build probability tables\n");
            matchset(162, team, league);
         }
      }
   }
   else {
      initialize(-1);
      printf("using dice to build probability tables\n");
      matchset(162, team, league);
   }

   exit(0);
}
