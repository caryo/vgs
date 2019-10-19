#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "engine0.h"
#include "engine1.h"
#include "engine2.h"

#define NUM_TEAMS    2
#define NUM_LEAGUES  1

struct game_data {
   struct team_data   away;
   struct team_data   home;
   struct league_data league;
};

void   initialize(int seed);
void   addstat(struct bstatdata g_stat[], struct bstatdata s_stat[], int n);
void   addpstat(struct pstatdata g_stat[], struct pstatdata s_stat[], int n);
void   matchset(int n, struct team_data team[], struct league_data league[]);
void   readvals(struct batdata *dataP);

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

            fprintf(stderr,"reading values from standard input\n");
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
            printf("using dice roll to build probability tables\n");
            matchset(162, team, league);
#endif
         }
         else {
#if !defined(USE_DICE)
            printf("using random seed to build probability tables\n");
#else
            printf("using dice roll to build probability tables\n");
#endif
            matchset(162, team, league);
         }
      }
   }
   else {
      initialize(-1);
#if !defined(USE_DICE)
      printf("using random seed to build probability tables\n");
#else
      printf("using dice roll to build probability tables\n");
#endif
      matchset(162, team, league);
   }

   exit(0);
}
