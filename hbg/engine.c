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

#define TEAM1          "Blue"
#define TEAM1_NICKNAME "BLU"
#define TEAM2          "Red"
#define TEAM2_NICKNAME "RED"

void   initialize(int seed);
void   addstat(struct team_data *g, struct team_data *s, int nBat, int nPit);
void   game_result(struct game_data *game, struct team_data team[]);
void   matchset(int n, int nTeams, struct team_data team[], struct league_data league[]);
void   read_num_teams(int *nTeamsP);
void   read_team_names(int nTeams, struct team_data team[]);
void   readvals(struct batdata *dataP);
void   batleaders(int nTeams, struct team_data team[], int nItems, int teamIdx[], int batIdx[]);
void   pitleaders(int nTeams, struct team_data team[], int nItems, int teamIdx[], int pitIdx[]);

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
   int total;
   if (game->winIdx == game->aTeamIdx) {
      team[game->aTeamIdx].w++;
      team[game->hTeamIdx].l++;
   }
   else {
      team[game->hTeamIdx].w++;
      team[game->aTeamIdx].l++;
   }
   total = game->away.r + game->away.lob + game->home.po;
#if DEBUG
   printf("away- pa:%d = r:%d + lob:%d + po:%d ( total:%d )\n",
      game->away.pa, game->away.r, game->away.lob, game->home.po, total);
#endif
   assert(game->away.pa == total);
   total = game->home.r + game->home.lob + game->away.po;
#if DEBUG
   printf("home- pa:%d = r:%d + lob:%d + po:%d ( total:%d )\n",
      game->home.pa, game->home.r, game->home.lob, game->away.po, total);
#endif
   assert(game->home.pa == total);

   team[game->aTeamIdx].pa += game->away.pa;
   team[game->aTeamIdx].r += game->away.r;
   team[game->aTeamIdx].lob += game->away.lob;
   team[game->aTeamIdx].po += game->away.po;

   team[game->hTeamIdx].pa += game->home.pa;
   team[game->hTeamIdx].r += game->home.r;
   team[game->hTeamIdx].lob += game->home.lob;
   team[game->hTeamIdx].po += game->home.po;
}

void matchset(int n, int nTeams, struct team_data team[], struct league_data league[]) {
   int i = 0;
   struct game_data gamestat;
   void *matchP = NULL;
   int x, y, done;
   int teamIdList[NUM_BATTERS], batIdList[NUM_BATTERS], pitIdList[NUM_PITCHERS];

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
#if DEBUG
   printf("season_result: %-15s %3s %3s %4s %4s %4s %4s\n",
          "Team", "W", "L", "PA", "R", "LOB", "PO");
#endif
   for (i=0; i<nTeams; i++) {
      boxscore(&team[i], NULL, 0);
#if DEBUG
      printf("season_result: %-15s %3d %3d %4d %4d %4d %4d\n", team[i].name, team[i].w, team[i].l,
        team[i].pa, team[i].r, team[i].lob, team[i].po);
#endif
   }
   batleaders(nTeams, team, NUM_BATTERS, teamIdList, batIdList); 
   printf("batting leaders:\n");
   print_batheader("Team");
   for (i=0; i<NUM_BATTERS; i++) {
      int teamIdx = teamIdList[i];
      int batIdx = batIdList[i];
      struct extrastat_t e;
#if DEBUG
      printf("teamIdx:%d batIdx:%d\n", teamIdx, batIdx);
#endif
      memset(&e,0,sizeof(struct extrastat_t));
      setextrastat(&e,&(team[teamIdx].b_stat[batIdx]),NULL,NULL);
      print_batstat(team[teamIdx].nickName,batIdx,
        &(team[teamIdx].b_stat[batIdx]),&e);
   }
   pitleaders(nTeams, team, NUM_PITCHERS, teamIdList, pitIdList); 
   printf("pitching leaders:\n");
   print_pitheader("Team");
   for (i=0; i<NUM_PITCHERS; i++) {
      int teamIdx = teamIdList[i];
      int pitIdx = pitIdList[i];
      struct extrastat_t e;
#if DEBUG
      printf("teamIdx:%d pitIdx:%d\n", teamIdx, pitIdx);
#endif
      memset(&e,0,sizeof(struct extrastat_t));
      setextrastat(&e,&(team[teamIdx].p_bstat[pitIdx]),NULL,
        &(team[teamIdx].p_pstat[pitIdx]));
      print_pitstat(team[teamIdx].nickName,pitIdx,
        &(team[teamIdx].p_bstat[pitIdx]),
        &(team[teamIdx].p_pstat[pitIdx]),&e);
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
   char nickName[32] = { 0 };
   int i, n1, n2;
   size_t teamNameL;
   size_t nickNameL;

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
      n2 = sscanf(inValsBuf,"teamNickName:%s\n", nickName);
#if DEBUG
      printf("n2:%d\n", n2);
#endif
      assert(n2 == 1);
      nickNameL = strlen(nickName);
#if DEBUG
      printf("teamNickName:%s, strlen(nickName):%d\n", nickName, (int) nickNameL);
#endif
      team[i].nameBuf = malloc(sizeof(char)*(teamNameL+1));
      assert(team[i].nameBuf != NULL);
      team[i].name = team[i].nameBuf;
      strncpy(team[i].name,teamName,teamNameL);
      team[i].name[teamNameL] = '\0';
#if DEBUG
      printf("team[%d].name:%s\n", i, team[i].name);
#endif
      team[i].nickNameBuf = malloc(sizeof(char)*(nickNameL+1));
      assert(team[i].nickNameBuf != NULL);
      team[i].nickName = team[i].nickNameBuf;
      strncpy(team[i].nickName,nickName,nickNameL);
      team[i].nickName[nickNameL] = '\0';
#if DEBUG
      printf("team[%d].nickName:%s\n", i, team[i].nickName);
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

struct sortdata_t {
   int teamIdx;
   int idx;
   enum { SORTDATA_FVAL, SORTDATA_IVAL } t;
   enum { SORT_ASC, SORT_DEC } dir;
   union {
      double fval;
      int ival;
   } d;
};

static int compare_sortdata(const void *aP, const void *bP) {
   struct sortdata_t *a = (struct sortdata_t *) aP;
   struct sortdata_t *b = (struct sortdata_t *) bP;
   int c;

   assert(a->t == b->t);
   assert(a->dir == b->dir);

   if (a->t == SORTDATA_FVAL) {
      if (a->dir == SORT_DEC) {
         if (a->d.fval > b->d.fval) {
            c = -1;
         }
         else if (a->d.fval < b->d.fval) {
            c = 1;
         }
         else {
            c = 0;
         }
      }
      else {
         if (a->d.fval < b->d.fval) {
            c = -1;
         }
         else if (a->d.fval > b->d.fval) {
            c = 1;
         }
         else {
            c = 0;
         }
      }
   }
   else {
      if (a->dir == SORT_DEC) {
         if (a->d.ival > b->d.ival) {
            c = -1;
         }
         else if (a->d.ival < b->d.ival) {
            c = 1;
         }
         else {
            c = 0;
         }
      }
      else {
         if (a->d.ival < b->d.ival) {
            c = -1;
         }
         else if (a->d.ival > b->d.ival) {
            c = 1;
         }
         else {
            c = 0;
         }
      }
   }

   return c;
}

void batleaders(int nTeams, struct team_data team[], int nItems, int teamIdx[], int batIdx[]) {
   struct sortdata_t *data;
   size_t dataL;
   int i, j;
   dataL = nTeams * NUM_BATTERS;
   data = malloc(sizeof(struct sortdata_t)*dataL);
   assert(data != NULL);
   for (i=0; i<nTeams; i++) {
      for (j=0; j<NUM_BATTERS; j++) {
         int offset;
         double avg;
         avg = (double) team[i].b_stat[j].h / team[i].b_stat[j].ab;
         offset = (i * NUM_BATTERS) + j;
         assert(offset < dataL);
#if DEBUG
         printf("i:%d j:%d NUM_BATTERS:%d offset:%d\n", i, j, NUM_BATTERS, offset);
#endif
         data[offset].teamIdx = i;
         data[offset].idx = j;
         data[offset].t = SORTDATA_FVAL;
         data[offset].dir = SORT_DEC;
         data[offset].d.fval = avg;
      }
   }
   qsort(data,dataL,sizeof(struct sortdata_t),compare_sortdata);
   for (i=0; i<nItems; i++) {
      teamIdx[i] = data[i].teamIdx;
      batIdx[i] = data[i].idx;
   }
   free(data);
}

void pitleaders(int nTeams, struct team_data team[], int nItems, int teamIdx[], int pitIdx[]) {
   struct sortdata_t *data;
   size_t dataL;
   int i, j;
   dataL = nTeams * NUM_PITCHERS;
   data = malloc(sizeof(struct sortdata_t)*dataL);
   assert(data != NULL);
   for (i=0; i<nTeams; i++) {
      for (j=0; j<NUM_PITCHERS; j++) {
         int offset;
         double avg;
         avg = (double) team[i].p_bstat[j].h / team[i].p_bstat[j].ab;
         offset = (i * NUM_PITCHERS) + j;
         assert(offset < dataL);
#if DEBUG
         printf("i:%d j:%d NUM_PITCHERS:%d offset:%d\n", i, j, NUM_PITCHERS, offset);
#endif
         data[offset].teamIdx = i;
         data[offset].idx = j;
         data[offset].t = SORTDATA_FVAL;
         data[offset].dir = SORT_ASC;
         data[offset].d.fval = avg;
      }
   }
   qsort(data,dataL,sizeof(struct sortdata_t),compare_sortdata);
   for (i=0; i<nItems; i++) {
      teamIdx[i] = data[i].teamIdx;
      pitIdx[i] = data[i].idx;
   }
   free(data);
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
   team[0].nickName = TEAM1_NICKNAME;
   team[1].name = TEAM2;
   team[1].nickName = TEAM2_NICKNAME;

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
