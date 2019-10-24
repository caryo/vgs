#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "engine0.h"
#include "engine1.h"
#include "engine2.h"

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

void boxscore(struct team_data *game,
              struct team_data team[], int teamIdx)
{
   int i;
   int pa, ab, r, h, rbi, bb, so, lob, s, d, t, hr, gdp, hbp, sf;
   int ab0, h0, bb0, hbp0, sf0;
   int t_ab0 = 0, t_h0 = 0, t_bb0 = 0, t_hbp0 = 0, t_sf0 = 0;
   double gavg = 0., savg = 0., gobp = 0., sobp = 0., era = 0., ipd;
   int w, l, ip, ip_f;

   pa=0; ab=0; r=0; h=0; rbi=0; bb=0; so=0; lob=0; s=0; d=0; t=0; hr=0; gdp=0; hbp=0; sf=0;
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
      struct bstatdata *pitbstat = game->p_bstat;
      struct pstatdata *pitpstat = game->p_pstat;
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
