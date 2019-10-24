#ifndef ENGINE2_H
#define ENGINE2_H

#define NUM_BATTERS  9
#define NUM_PITCHERS 6

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

struct team_data {
   struct batdata   bat[NUM_BATTERS];
   struct batdata   pit[NUM_PITCHERS];
   struct probdata  batp[NUM_BATTERS];
   struct bstatdata b_stat[NUM_BATTERS];
   struct bstatdata p_bstat[NUM_PITCHERS];
   struct pstatdata p_pstat[NUM_PITCHERS];
   int    w;
   int    l;
   int    pa;
   int    r;
   int    lob;
   int    po;
   char  *nameBuf;
   char  *name;
};

struct game_data {
   struct team_data   away;
   struct team_data   home;
   int aTeamIdx;
   int aLeagueIdx;
   int hTeamIdx;
   int hLeagueIdx;
   int winIdx;
};

struct league_data {
   struct batdata   bat;
   struct batdata   pit;
};

long   myround(double x);
void   defaultprob(int p[]);
int    log5calc(double b, double bl, double p, double pl);
void   setprob(
               struct team_data team[], struct league_data league[],
               int aTeamIdx, int aLeagueIdx, int batIdx,
               int bTeamIdx, int bLeagueIdx, int pitIdx);
void   initrand(
                struct team_data team[], struct league_data league[],
                int aTeamIdx, int aLeagueIdx, int batIdx,
                int bTeamIdx, int bLeagueIdx, int pitIdx);
void   initmem(int c, int *ahiP[], int *ariP[], int *hhiP[], int *hriP[]);
void   clearmem(int idx, int c, int ahiP[], int ariP[], int hhiP[], int hriP[]);
void   linescore(int i, char *aName, char *hName, int ahiP[], int ariP[],
               int ar, int alo, int apo, int hhiP[],
               int hriP[], int hr, int hlo, int hpo, int g, int aw, int al,
               int hw, int hl);
void   boxscore(struct team_data *game,
                struct team_data team[], int teamIdx);
void   match(int g, struct team_data team[], struct league_data league[],
             struct game_data *game);

#endif
