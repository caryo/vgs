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

void   initialize(int seed);
void   addstat(struct team_data *g, struct team_data *s, int nBat, int nPit);
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

void addstat(struct team_data *g, struct team_data *s, int nBat, int nPit) {
   int i, ip_f;

   for (i=0; i<nBat; i++) {
      s->b_stat[i].pa  += g->b_stat[i].pa;
      s->b_stat[i].ab  += g->b_stat[i].ab;
      s->b_stat[i].r   += g->b_stat[i].r;
      s->b_stat[i].h   += g->b_stat[i].h;
      s->b_stat[i].rbi += g->b_stat[i].rbi;
      s->b_stat[i].bb  += g->b_stat[i].bb;
      s->b_stat[i].so  += g->b_stat[i].so;
      s->b_stat[i].lob += g->b_stat[i].lob;
      s->b_stat[i].s   += g->b_stat[i].s;
      s->b_stat[i].d   += g->b_stat[i].d;
      s->b_stat[i].t   += g->b_stat[i].t;
      s->b_stat[i].hr  += g->b_stat[i].hr;
      s->b_stat[i].gdp += g->b_stat[i].gdp;
      s->b_stat[i].hbp += g->b_stat[i].hbp;
      s->b_stat[i].sf  += g->b_stat[i].sf;
   }

   for (i=0; i<nPit; i++) {
      s->p_bstat[i].pa  += g->p_bstat[i].pa;
      s->p_bstat[i].ab  += g->p_bstat[i].ab;
      s->p_bstat[i].r   += g->p_bstat[i].r;
      s->p_bstat[i].h   += g->p_bstat[i].h;
      s->p_bstat[i].rbi += g->p_bstat[i].rbi;
      s->p_bstat[i].bb  += g->p_bstat[i].bb;
      s->p_bstat[i].so  += g->p_bstat[i].so;
      s->p_bstat[i].lob += g->p_bstat[i].lob;
      s->p_bstat[i].s   += g->p_bstat[i].s;
      s->p_bstat[i].d   += g->p_bstat[i].d;
      s->p_bstat[i].t   += g->p_bstat[i].t;
      s->p_bstat[i].hr  += g->p_bstat[i].hr;
      s->p_bstat[i].gdp += g->p_bstat[i].gdp;
      s->p_bstat[i].hbp += g->p_bstat[i].hbp;
      s->p_bstat[i].sf  += g->p_bstat[i].sf;

      s->p_pstat[i].ip += g->p_pstat[i].ip;
      ip_f = s->p_pstat[i].ip_f;
      ip_f += g->p_pstat[i].ip_f;
      s->p_pstat[i].ip += ip_f / 3;
      s->p_pstat[i].ip_f = ip_f % 3;
      s->p_pstat[i].w += g->p_pstat[i].w;
      s->p_pstat[i].l += g->p_pstat[i].l;
   }
}

void matchset(int n, struct team_data team[], struct league_data league[]) {
   int i, aw = 0, hw = 0;
   struct game_data gamestat;

   for (i=0; i<n; i++) {
      if (i%2 == 0) {
         memset(&gamestat, 0, sizeof(struct game_data));
         gamestat.aTeamIdx = 0;
         gamestat.aLeagueIdx = 0;
         gamestat.hTeamIdx = 1;
         gamestat.hLeagueIdx = 0;
         match(i, "Blue", "Red", team, league, &gamestat, &aw, &hw);
         addstat(&gamestat.away, &team[0], NUM_BATTERS, NUM_PITCHERS);
         addstat(&gamestat.home, &team[1], NUM_BATTERS, NUM_PITCHERS);
      }
      else {
         memset(&gamestat, 0, sizeof(struct game_data));
         gamestat.aTeamIdx = 1;
         gamestat.aLeagueIdx = 0;
         gamestat.hTeamIdx = 0;
         gamestat.hLeagueIdx = 0;
         match(i, "Red", "Blue", team, league, &gamestat, &hw, &aw);
         addstat(&gamestat.home, &team[0], NUM_BATTERS, NUM_PITCHERS);
         addstat(&gamestat.away, &team[1], NUM_BATTERS, NUM_PITCHERS);
      }
   }
   printf("\n");
   boxscore("Blue", &team[0], NULL, 0);
   boxscore("Red", &team[1], NULL, 0);
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
