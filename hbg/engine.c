#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "engine0.h"
#include "engine1.h"
#include "engine2.h"
#include "engine3.h"

#define NUM_TEAMS    2
#define NUM_LEAGUES  1

#define TEAM1 "Blue"
#define TEAM2 "Red"

void   initialize(int seed);
void   addstat(struct team_data *g, struct team_data *s, int nBat, int nPit);
void   game_result(struct game_data *game, struct team_data team[]);
void   matchset(int n, int nTeams, struct team_data team[], struct league_data league[]);
void   read_num_teams(int *nTeamsP);
void   read_team_names(int nTeams, struct team_data team[]);
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

void game_result(struct game_data *game, struct team_data team[]) {
   if (game->winIdx == game->aTeamIdx) {
      team[game->aTeamIdx].w++;
      team[game->hTeamIdx].l++;
   }
   else {
      team[game->hTeamIdx].w++;
      team[game->aTeamIdx].l++;
   }
}

void matchset(int n, int nTeams, struct team_data team[], struct league_data league[]) {
   int i;
   struct game_data gamestat;
   void *matchP = NULL;
   int x, y, done;

   match_create(nTeams, &matchP);
   match_done(matchP, &done);

#if DEBUG
   printf("number of games initial for season: %d\n", n);
#endif
   n = ((n - (n % ((nTeams-1)*2))) / 2) * nTeams;
#if DEBUG
   printf("number of games adjusted for season: %d\n", n);
#endif
   while(i<n) {
      memset(&gamestat, 0, sizeof(struct game_data));
      if (i%2 == 0) {
         match_getxy(matchP, &x, &y);
         match_nextxy(matchP);
         gamestat.aTeamIdx = y;
         gamestat.aLeagueIdx = 0;
         gamestat.hTeamIdx = x;
         gamestat.hLeagueIdx = 0;
      }
      else {
         gamestat.aTeamIdx = x;
         gamestat.aLeagueIdx = 0;
         gamestat.hTeamIdx = y;
         gamestat.hLeagueIdx = 0;
         match_done(matchP, &done);
         if (done) {
            match_reset(matchP);
            match_done(matchP,&done);
         }
      }
      match(i, team, league, &gamestat);
      addstat(&gamestat.away, &team[gamestat.aTeamIdx], NUM_BATTERS, NUM_PITCHERS);
      addstat(&gamestat.home, &team[gamestat.hTeamIdx], NUM_BATTERS, NUM_PITCHERS);
      game_result(&gamestat, team);
      i++;
      if (done) break;
   }
   printf("\n");
   for (i=0; i<nTeams; i++) {
      boxscore(&team[i], NULL, 0);
   }
}

void read_num_teams(int *nTeamsP) {
   char inbuf[132], *pInbuf;
   char inValsBuf[132] = { 0 };
   int n1, n2, nTeams;

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
   n2 = sscanf(inValsBuf,"nTeams:%d\n", &nTeams);
#if DEBUG
   printf("n2:%d\n", n2);
#endif
   assert(n2 == 1);
#if DEBUG
   printf("nTeams:%d\n", nTeams);
#endif
   *nTeamsP = nTeams;
}

void read_team_names(int nTeams, struct team_data team[]) {
   char inbuf[132], *pInbuf;
   char inValsBuf[132] = { 0 };
   char teamName[64] = { 0 };
   int i, n1, n2;
   size_t teamNameL;

   for (i=0; i<nTeams; i++) {
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
      n2 = sscanf(inValsBuf,"teamName:%s\n", teamName);
#if DEBUG
      printf("n2:%d\n", n2);
#endif
      assert(n2 == 1);
      teamNameL = strlen(teamName);
#if DEBUG
      printf("teamName:%s, strlen(teamName):%d\n", teamName, (int) teamNameL);
#endif
      team[i].nameBuf = malloc(sizeof(char)*(teamNameL+1));
      assert(team[i].nameBuf != NULL);
      team[i].name = team[i].nameBuf;
      strncpy(team[i].name,teamName,teamNameL);
      team[i].name[teamNameL] = '\0';
#if DEBUG
      printf("team[%d].name:%s\n", i, team[i].name);
#endif
   }
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
   struct team_data  *team = NULL;
   struct league_data league[NUM_LEAGUES] = { 0 };
   int nTeams = 0;
   int k;

   nTeams = NUM_TEAMS;
   team = malloc(sizeof(struct team_data) * nTeams);
   assert(team != NULL);
   memset(team, 0, sizeof(struct team_data) * nTeams);
   team[0].name = TEAM1;
   team[1].name = TEAM2;

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
            read_num_teams(&nTeams);
            printf("nTeams:%d\n", nTeams);
            if (nTeams > 2) {
               team = realloc(team,sizeof(struct team_data) * nTeams);
               assert(team != NULL);
            }
            read_team_names(nTeams, team);
            for (j=0; j<nTeams; j++) {
               for (i=0; i<NUM_BATTERS; i++) {
                  readvals(&(team[j].bat[i]));
               }
            }
            readvals(&(league[0].bat));
            for (j=0; j<nTeams; j++) {
               for (i=0; i<NUM_PITCHERS; i++) {
                  readvals(&(team[j].pit[i]));
               }
            }
            readvals(&(league[0].pit));

            printf("using input values to build probability tables\n");
            matchset(162, nTeams, team, league);
#else
            printf("using dice roll to build probability tables\n");
            matchset(162, nTeams, team, league);
#endif
         }
         else {
#if !defined(USE_DICE)
            printf("using random seed to build probability tables\n");
#else
            printf("using dice roll to build probability tables\n");
#endif
            matchset(162, nTeams, team, league);
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
      matchset(162, nTeams, team, league);
   }

   for (k=0; k<nTeams; k++) {
      if (team[k].nameBuf) {
         free(team[k].nameBuf);
      }
   }
   free(team);

   exit(0);
}
