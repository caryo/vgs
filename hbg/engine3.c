#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "engine3.h"

struct match_data {
   int nTeams;
   int x;
   int y;
   int z;
   int i;
   int j;
   int done;
};

void match_reset(void *matchH) {
   struct match_data *matchP = matchH;
   int nTeams;

   assert(matchH != NULL);

   nTeams = matchP->nTeams;

   memset(matchP, 0, sizeof(struct match_data));
   matchP->nTeams = nTeams;
   matchP->x = 1;
   matchP->y = 0;
   matchP->z = 0;
   matchP->i = 0;
   matchP->j = 1;
   matchP->done = 0;
}

void match_create(int nTeams, void **matchH) {
   struct match_data *matchP;

   assert(matchH != NULL);
   assert(*matchH == NULL);

   matchP = malloc(sizeof(struct match_data));
   assert(matchP != NULL);
   matchP->nTeams = nTeams;
   match_reset(matchP);

   *matchH = matchP;
}

void match_nextxy(void *matchH) {
   struct match_data *matchP = matchH;

   assert(matchP != NULL);

   if (!matchP->done) {
      matchP->j++;
      if (matchP->j < matchP->nTeams-matchP->i) {
         matchP->x = matchP->x + 1;
         matchP->y = matchP->y + 1;
         printf("a) matchP->x:%d, matchP->y:%d\n", matchP->x, matchP->y);
      }
      else {
         matchP->z += ((matchP->nTeams-1) - matchP->i) * 2;
         matchP->i++;
         if (matchP->i < matchP->nTeams-1) {
            matchP->x = matchP->i + 1;
            matchP->y = 0;
            matchP->j = 1;
            printf("b) matchP->x:%d, matchP->y:%d\n", matchP->x, matchP->y);
         }
         else {
            matchP->done = 1;
         }
      }
   }
}

void match_done(void *matchH, int *doneP) {
   struct match_data *matchP = matchH;

   assert(matchP != NULL);
   assert(doneP != NULL);
   
   *doneP = matchP->done;
}

void match_getxy(void *matchH, int *xP, int *yP) {
   struct match_data *matchP = matchH;

   assert(matchP != NULL);
   assert(xP != NULL);
   assert(yP != NULL);

   *xP = matchP->x;
   *yP = matchP->y;
}
