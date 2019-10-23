#ifndef ENGINE3_H
#define ENGINE3_H

void match_reset(void *matchH);
void match_create(int nTeams, void **matchH);
void match_nextxy(void *matchH);
void match_done(void *matchH, int *doneP);
void match_getxy(void *matchH, int *xP, int *yP);

#endif
