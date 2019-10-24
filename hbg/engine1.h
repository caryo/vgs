#ifndef ENGINE1_H
#define ENGINE1_H

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

int    roll(void);
int    maprand(int p[], int n, int z0);
int    genrand(int p[], int n);
void   onbase(int ob[], int li);
int    offbase(int ob[]);
char * result_code(int z);
int    result(int z, int a, int *o, int *h, int *r, int *c, int *gdp, int idx, int ob[], int rli[], int *rlo);
void   statrun(struct bstatdata stats[], int idx, int result);
void   stat(struct bstatdata stats[], int idx, int result, int gdp, int rbi, int lob);
void   side(struct probdata p[], int n,
            int i, int b, int d, int *r, int *h, int *li,
            int li_base, struct bstatdata batstat[], struct bstatdata pitbstat[],
            struct pstatdata pitpstat[], int pitIdx, int *lob, int *out);

#endif
