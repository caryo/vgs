#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "engine0.h"
#include "engine1.h"
#include "engine2.h"

struct extrastat_t {
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
   int ab0;
   int h0;
   int bb0;
   int hbp0;
   int sf0;
   int t_ab0;
   int t_h0;
   int t_bb0;
   int t_hbp0;
   int t_sf0;
   double gavg;
   double savg;
   double gobp;
   double sobp;
   double era;
   double ipd;
   int w;
   int l;
   int ip;
   int ip_f;
};

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
   if (team) {
      setprob(team, league,
              aTeamIdx, aLeagueIdx, batIdx,
              bTeamIdx, bLeagueIdx, pitIdx);
   }
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

void linescore(int i, char *aName, char *hName, int ahiP[], int ariP[], int ar, int alo, int apo,
               int hhiP[], int hriP[], int hr, int hlo, int hpo,
               int g, int aw, int al, int hw, int hl)
{
   int ah = 0, hh = 0;
   int in = i+1;

   if (ar > hr) {
      aw++;
      hl++;
   }
   else if (hr > ar) {
      hw++;
      al++;
   }

   printf("%-20s","Team:");
   for (i=0; i<in; i++) {
      printf("%2d ", i+1);
      if (((i+1) % 3) == 0) {
         printf(" ");
      }
   }
   printf("%5c %2c %2c %5s %5s\n", 'R', 'H', 'E', "LOB", "PO");
   printf("%-8s (%3d-%3d): ", aName, aw, al);
   for (i=0; i<in; i++) {
      printf("%2d ", ariP[i]);
      if (((i+1) % 3) == 0) {
         printf(" ");
      }
      ah = ah + ahiP[i];
   }
   printf("%5d %2d %2d %5d %5d\n", ar, ah, 0, alo, apo);
   printf("%-8s (%3d-%3d): ", hName, hw, hl);
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
   printf("%5d %2d %2d %5d %5d\n", hr, hh, 0, hlo, hpo);
}

static void setextrastat(struct extrastat_t *e, struct bstatdata *b, struct bstatdata *t) {
   if (b->ab > 0) {
      e->gavg = (double) b->h / b->ab;
   }
   else {
      e->gavg = 0.;
   }
   if (t) {
      e->ab0 = t->ab;
      e->h0 = t->h;
      e->bb0 = t->bb;
      e->hbp0 = t->hbp;
      e->sf0 = t->sf;
      e->t_ab0 += e->ab0;
      e->t_h0 += e->h0;
      e->t_bb0 += e->bb0;
      e->t_hbp0 += e->hbp0;
      e->t_sf0 += e->sf0;
   }
   else {
      e->ab0 = 0;
      e->h0 = 0;
      e->bb0 = 0;
      e->hbp0 = 0;
      e->sf0 = 0;
      e->t_ab0 = 0;
      e->t_h0 = 0;
      e->t_bb0 = 0;
      e->t_hbp0 = 0;
      e->t_sf0 = 0;
   }
   e->ab0 += b->ab;
   e->h0 += b->h;
   e->bb0 += b->bb;
   e->hbp0 += b->hbp;
   e->sf0 += b->sf;
   if (e->ab0 > 0) {
      e->savg = (double) e->h0 / e->ab0;
   }
   else {
      e->savg = 0.;
   }
   if ((b->ab + b->bb + b->hbp + b->sf) > 0) {
      e->gobp = (double) (b->h + b->bb + b->hbp) /
        (b->ab + b->bb + b->hbp + b->sf);
   }
   else {
      e->gobp = 0.;
   }
   if ((e->ab0 + e->bb0 + e->hbp0 + e->sf0) > 0) {
      e->sobp = (double) (e->h0 + e->bb0 + e->hbp0) /
        (e->ab0 + e->bb0 + e->hbp0 + e->sf0);
   }
   else {
      e->sobp = 0.;
   }

   e->pa  += b->pa;
   e->ab  += b->ab;
   e->r   += b->r;
   e->h   += b->h;
   e->rbi += b->rbi;
   e->bb  += b->bb;
   e->so  += b->so;
   e->lob += b->lob;
   e->s   += b->s;
   e->d   += b->d;
   e->t   += b->t;
   e->hr  += b->hr;
   e->gdp += b->gdp;
}

static void finishextrastat(struct extrastat_t *e) {
   if (e->ab > 0) {
      e->gavg = (double) e->h / e->ab;
   }
   else {
      e->gavg = 0.;
   }
   if ((e->ab+e->bb+e->hbp+e->sf) > 0) {
      e->gobp = (double) (e->h + e->bb + e->hbp) /
        (e->ab + e->bb + e->hbp + e->sf);
   }
   else {
      e->gobp = 0.;
   }
   e->ab0 = e->t_ab0 + e->ab;
   e->h0 = e->t_h0 + e->h;
   e->bb0 = e->t_bb0 + e->bb;
   e->hbp0 = e->t_hbp0 + e->hbp;
   e->sf0 = e->t_sf0 + e->sf;
   if (e->ab0 > 0) {
      e->savg = (double) e->h0 / e->ab0;
   }
   else {
      e->savg = 0.;
   }
   if ((e->ab0 + e->bb0 + e->hbp0 + e->sf0) > 0) {
      e->sobp = (double) (e->h0 + e->bb0 + e->hbp0) /
        (e->ab0 + e->bb0 + e->hbp0 + e->sf0);
   }
   else {
      e->sobp = 0.;
   }
}

void boxscore(struct team_data *game, struct team_data team[], int teamIdx) {
   int i;
   struct extrastat_t e;

   memset(&e,0,sizeof(struct extrastat_t));
   if (team) {
      printf("%s Team:\n", team[teamIdx].name);
   }
   else {
      printf("%s Team:\n", game->name);
   }
   printf("%4s %4s %4s %4s %4s %4s %4s %4s %4s %4s %4s %4s %4s %4s %6s %6s %6s %6s\n",
     "#", "PA", "AB", "R", "H", "RBI", "BB", "SO", "LOB", "S", "D", "T", "HR",
     "GDP", "AVG", "SAVG", "OBP", "SOBP");
   for(i=0; i<NUM_BATTERS; i++) {
      struct bstatdata *batstat = game->b_stat;
      if (team) {
         setextrastat(&e,&(batstat[i]),&(team[teamIdx].b_stat[i]));
      }
      else {
         setextrastat(&e,&(batstat[i]),NULL);
      }
      printf("%4d %4d %4d %4d %4d %4d %4d %4d %4d %4d %4d %4d %4d %4d %6.3f %6.3f %6.3f %6.3f\n",
         i+1, batstat[i].pa, batstat[i].ab, batstat[i].r, batstat[i].h, batstat[i].rbi,
         batstat[i].bb, batstat[i].so, batstat[i].lob, batstat[i].s, batstat[i].d,
         batstat[i].t, batstat[i].hr, batstat[i].gdp, e.gavg, e.savg, e.gobp, e.sobp);
   }

   finishextrastat(&e);

   printf("%-4s %4d %4d %4d %4d %4d %4d %4d %4d %4d %4d %4d %4d %4d %6.3f %6.3f %6.3f %6.3f\n",
      "TOT", e.pa, e.ab, e.r, e.h, e.rbi, e.bb, e.so, e.lob, e.s,
      e.d, e.t, e.hr, e.gdp, e.gavg, e.savg, e.gobp, e.sobp);

   memset(&e,0,sizeof(struct extrastat_t));

   printf("%4s %4s %4s %4s %4s %4s %4s %4s %4s %4s %4s %4s %4s %4s %6s %6s %6s %7s %6s\n",
     "#", "PA", "AB", "R", "H", "RBI", "BB", "SO", "LOB", "S", "D", "T", "HR", "GDP",
     "AVG", "SAVG", "IP", "W-L", "ERA");
   for (i=0; i<NUM_PITCHERS; i++) {
      struct bstatdata *pitbstat = game->p_bstat;
      struct pstatdata *pitpstat = game->p_pstat;
      if (team) {
         setextrastat(&e,&(pitbstat[i]),&(team[teamIdx].p_bstat[i]));
      }
      else {
         setextrastat(&e,&(pitbstat[i]),NULL);
      }
      e.ipd = (double) pitpstat[i].ip + (double) pitpstat[i].ip_f / 3;
      if (e.ipd > 0.) {
         e.era = 9.0 * (pitbstat[i].r / e.ipd);
      }
      else {
         e.era = 0.;
      }
      printf("%4d %4d %4d %4d %4d %4d %4d %4d %4d %4d %4d %4d %4d %4d %6.3f %6.3f %4d.%d %3d-%3d %6.2f\n",
         i+1, pitbstat[i].pa, pitbstat[i].ab, pitbstat[i].r, pitbstat[i].h, pitbstat[i].rbi,
         pitbstat[i].bb, pitbstat[i].so, pitbstat[i].lob, pitbstat[i].s, pitbstat[i].d,
         pitbstat[i].t, pitbstat[i].hr, pitbstat[i].gdp, e.gavg, e.savg,
         pitpstat[i].ip, pitpstat[i].ip_f, pitpstat[i].w, pitpstat[i].l, e.era);

      e.w += pitpstat[i].w;
      e.l += pitpstat[i].l;
      e.ip += pitpstat[i].ip;
      e.ip_f += pitpstat[i].ip_f;
      e.ip += e.ip_f / 3;
      e.ip_f = e.ip_f % 3;
   }

   finishextrastat(&e);

   e.ipd = (double) e.ip + (double) e.ip_f / 3;
   if (e.ipd > 0.) {
      e.era = 9 * ((double) e.r / e.ipd);
   }
   else {
      e.era = 0.;
   }

   printf("%-4s %4d %4d %4d %4d %4d %4d %4d %4d %4d %4d %4d %4d %4d %6.3f %6.3f %4d.%d %3d-%3d %6.2f\n",
      "TOT", e.pa, e.ab, e.r, e.h, e.rbi, e.bb, e.so, e.lob, e.s, e.d, e.t, e.hr,
      e.gdp, e.gavg, e.savg, e.ip, e.ip_f, e.w, e.l, e.era);
}

void match(int g, struct team_data team[], struct league_data league[],
           struct game_data *game)
{
   int i = 0;
   int ar = 0, alo = 0, apo = 0, ali_base = 100, ali = 100, *ahiP = NULL, *ariP = NULL;
   int hr = 0, hlo = 0, hpo = 0, hli_base = 200, hli = 200, *hhiP = NULL, *hriP = NULL;
   struct probdata dfltp[NUM_BATTERS];
   struct probdata *probP;
   int n = sizeof(dfltp[0].p)/sizeof(int);
   int c = 2;
   int alob = 0, hlob = 0;
   int aout = 0, hout = 0;
   int pitIdx;
   int aTeamIdx, hTeamIdx;
   int aLeagueIdx, hLeagueIdx;

   aTeamIdx = game->aTeamIdx;
   aLeagueIdx = game->aLeagueIdx;
   hTeamIdx = game->hTeamIdx;
   hLeagueIdx = game->hLeagueIdx;

   initmem(c, &ahiP, &ariP, &hhiP, &hriP);
   for (i=0; i<NUM_BATTERS; i++) {
      if ((team[aTeamIdx].bat[i].ab > 0 && team[hTeamIdx].pit[i].ab > 0) &&
          (team[hTeamIdx].bat[i].ab > 0 && team[aTeamIdx].pit[i].ab > 0))
      {
         pitIdx = (team[hTeamIdx].w + team[hTeamIdx].l) % NUM_PITCHERS;
         initrand(
                  team, league,
                  aTeamIdx, aLeagueIdx, i,
                  hTeamIdx, hLeagueIdx, pitIdx);
         pitIdx = (team[aTeamIdx].w + team[aTeamIdx].l) % NUM_PITCHERS;
         initrand(
                  team, league,
                  hTeamIdx, hLeagueIdx, i,
                  aTeamIdx, aLeagueIdx, pitIdx);
#if DEBUG
         printf("\n");
#endif
      }
      else {
         int j;
         pitIdx = g % 6;
         if (i==0) {
            defaultprob(dfltp[i].p);
#if DEBUG
            if (g == 0) {
               for (j=0; j<n; j++) {
                  printf("j:%2d, dfltp[%d].p[%d]: %4d, pitIdx:%d\n", j, i, j, dfltp[i].p[j], pitIdx);
               }
               printf("\n");
            }
#endif
         }
         else {
            for (j=0; j<n; j++) {
               dfltp[i].p[j] = dfltp[0].p[j];
            }
         }
      }
   }

   i = 0;
   do {
      if (i >= c) {
         int idx = c, extra = c/2;
         c += c/2;
         printf("initmem: i:%d, c:%d\n", i, c);
         initmem(c, &ahiP, &ariP, &hhiP, &hriP);
         clearmem(idx,extra,ahiP,ariP,hhiP,hriP);
      }

      probP = dfltp;

      if ((team[aTeamIdx].bat[0].ab > 0 && team[hTeamIdx].pit[0].ab > 0) &&
          (team[hTeamIdx].bat[0].ab > 0 && team[aTeamIdx].pit[0].ab > 0))
      {
         probP = team[aTeamIdx].batp;
      }
      pitIdx = (team[hTeamIdx].w + team[hTeamIdx].l) % NUM_PITCHERS;
      side(probP, n,
           i,0,0,&ariP[i], &ahiP[i], &ali, ali_base, game->away.b_stat,
           game->home.p_bstat, game->home.p_pstat, pitIdx, &alob, &hout);
      printf("i:%d, ariP[i]:%d, ahiP[i]:%d, alob:%d\n", i, ariP[i], ahiP[i], alob);
      alo = alo + alob;
      hpo = hpo + hout;
      ar = ar + ariP[i];
      if (i>=8 && hr > ar) {
         hriP[i] = -1;
         break;
      }
      printf("end of side: top, i:%d, ar:%d, hr:%d\n", i, ar, hr);

      probP = dfltp;

      if ((team[aTeamIdx].bat[0].ab > 0 && team[hTeamIdx].pit[0].ab > 0) &&
          (team[hTeamIdx].bat[0].ab > 0 && team[aTeamIdx].pit[0].ab > 0))
      {
         probP = team[hTeamIdx].batp;
      }
      pitIdx = (team[aTeamIdx].w + team[aTeamIdx].l) % NUM_PITCHERS;
      side(probP, n,
           i,1,hr-ar,&hriP[i], &hhiP[i], &hli, hli_base, game->home.b_stat,
           game->away.p_bstat, game->away.p_pstat, pitIdx, &hlob, &aout);
      printf("i:%d, hriP[i]:%d, hhiP[i]:%d, hlob:%d\n", i, hriP[i], hhiP[i], hlob);
      hlo = hlo + hlob;
      apo = apo + aout;
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

   game->away.pa = ali - ali_base;
   game->away.r = ar;
   game->away.lob = alo;
   game->away.po = apo;
   game->home.pa = hli - hli_base;
   game->home.r = hr;
   game->home.lob = hlo;
   game->home.po = hpo;

   if (ar > hr) {
      pitIdx = (team[aTeamIdx].w + team[aTeamIdx].l) % NUM_PITCHERS;
      game->away.p_pstat[pitIdx].w++;
      pitIdx = (team[hTeamIdx].w + team[hTeamIdx].l) % NUM_PITCHERS;
      game->home.p_pstat[pitIdx].l++;
      game->winIdx = aTeamIdx;
   }
   else {
      pitIdx = (team[hTeamIdx].w + team[hTeamIdx].l) % NUM_PITCHERS;
      game->home.p_pstat[pitIdx].w++;
      pitIdx = (team[aTeamIdx].w + team[aTeamIdx].l) % NUM_PITCHERS;
      game->away.p_pstat[pitIdx].l++;
      game->winIdx = hTeamIdx;
   }

   printf("Game: %d\n", g+1);
   linescore(i,team[aTeamIdx].name,team[hTeamIdx].name,
             ahiP,ariP,ar,alo,apo,hhiP,hriP,hr,hlo,hpo,g,
             team[aTeamIdx].w, team[aTeamIdx].l,
             team[hTeamIdx].w, team[hTeamIdx].l);
   boxscore(&game->away,team,aTeamIdx);
   boxscore(&game->home,team,hTeamIdx);

   free(ahiP);
   free(ariP);
   free(hhiP);
   free(hriP);
}
