#include <stdlib.h>
#include <stdio.h>

int advance(int a, int *b, int *c, int *r1, int *r2) {
   int x, r;

   /* special case when x is 0 and a is 7 - treat x as a 1 */
   if (a == 7 && x == 0) {
      x = 1;
#if DEBUG
         printf("x =%d\n", x);
#endif
      r = advance_x(a, x, b, c, r1, r2);
   }
   else {
      int q0, q1, i;

      r  = 0;
      q0 = a;
      q1 = a;
      *b = 0;
      *c = 0;
      *r1 = 0;
      *r2 = 0;

      for (i=0; q0 && i<3; i++) {
#if DEBUG
         printf("i=%d, q0=0x%02x, q1=0x%02x, b=0x%02x\n", i, q0, q1, *b);
#endif
         if ((q0 % 2) == 0) {
            break;
         }
         q1 = q1 & ~(1 << i);
         *b |= 1;
         *b = *b << 1;
         q0 = q0 >> 1;
      }

      *b |= 1;

      *c = q1 | *b;
#if DEBUG
      printf("q0=0x%02x, q1=0x%02x, b=0x%02x, c=0x%02x\n", q0, q1, *b, *c);
#endif
   }

   return r;
}

int advance_x(int a, int x, int *b, int *c, int *r1, int *r2) {
   int r, q, i;

   if (x > 0) {
      *b  = (a << x) + (1 << (x-1));
   }
   else {
      *b  = (a << 1);
   }

   *r1 = (*b & 0xf8);
   r   = *r1 >> 3;
   *c  = *b & 0x07;

   q = r;

   *r2 = 0;

   for (i=0; q && i<4; i++) {
#if DEBUG
      printf("i=%d, q=0x%02x, r2=0x%02x\n", i, q, *r2);
#endif
      if ((q % 2) == 1) {
         *r2 += 1;
      }
      q = q >> 1;
   }

   return r;
}

void initialize() {
   srand(time(NULL));
}

int test_advance(int argc, char *argv[]) {
   int i;
   int a;
   int x;
   int r;
   int b;
   int c;
   int r1;
   int r2;

#if DEBUG
   for (i=0; i<argc; i++) {
      printf("argv[%d]=%s\n", i, argv[i]);
   }
   printf("\n");
#endif

   if (argc <= 2) {
      printf("error: argc=%d, expected at least 2 integer values as input - exiting\n", argc);
      exit(1);
   }

   a = atoi(argv[1]);
   x = atoi(argv[2]);

   if (a < 0) {
      a = 0;
   }
   else if (a > 7) {
      a = 7;
   }

   if (x < -1) {
      x = -1;
   }
   else if (x > 4) {
      x = 4;
   }

#if DEBUG
   printf("a =%d (0x%02x)\n", a, a);
   printf("x =%d\n", x);
#endif

   if (x <= 0) {
      if (x == 0) {
         r = advance(a, &b, &c, &r1, &r2);
      }
      else if (x == -1) {
         r = advance_x(a, 0, &b, &c, &r1, &r2);
      }
   }
   else {
      r = advance_x(a, x, &b, &c, &r1, &r2);
   }

#if DEBUG
   printf("r1=%d (0x%02x)\n", r1, r1);
   printf("r =%d (0x%02x)\n", r, r);
   printf("b =%d (0x%02x)\n", b, b);
   printf("c =%d (0x%02x)\n", c, c);
   printf("r2=%d (0x%02x)\n", r2, r2);
#endif
}

int result(int z, int a, int *o, int *h, int *r, int *c) {
   int x, f, w;
   int b, r1, r2;

   x = -1;
   f = 0;
   w = 0;

   switch(z) {
      case  2:
         x = 0;
         w = 0;
      break;

      case  3:
         x = 3;
         *h = *h + 1;
      break;

      case  4:
         x = 2;
         *h = *h + 1;
      break;

      case  5:
         x = 0;
         w = 1;
      break;

      case 6:
      case 7:
      case 9:
         break;

      case  8:
         x = 1;
         *h = *h + 1;
      break;

      case 10:
         f = 1;
      break;

      case 11:
         x = -2;
      break;

      case 12:
         x = 4;
         *h = *h + 1;
      break;

      default:
      break;
   }

   if (w == 1) {
      advance(a, &b, c, &r1, &r2);
      *r = *r + r2;
   }
   else if (x >= 0) {
      advance_x(a, x, &b, c, &r1, &r2);
      *r = *r + r2;
   }
   else {
      if (x == -2) {
         if (*c > 0) {
            if (*c & 0x01) {
               *c = *c & ~(0x01);
            }
            else if (*c & 0x02) {
               *c = *c & ~(0x02);
            }
            else if (*c & 0x04) {
               *c = *c & ~(0x04);
            }
            *o = *o + 1;
            if (*o < 3) {
               *o = *o + 1;
            }
         }
      }
      else if (x == -1) {
         if (f == 0) {
            *o = *o + 1;
         }
      }
   }

   return z;
}

int side(int *r, int *h) {
   int z1, z2, z;
   int o;
#if 0
   int h, r;
#endif
   int a, c;

#if 0
   r = 0;
   h = 0;
#endif
   o = 0;
   c = 0;

   do {
      a =  c;

      z1 = (rand() % 6) + 1;
      z2 = (rand() % 6) + 1;

      z = z1 + z2;

#if DEBUG
   printf("z1=%d z2=%d z=%2d\n", z1, z2, z);
#endif

#if DEBUG
#if 0
      printf("a=0x%02x o=%d h=%2d r=%2d z=%2d\n", a, o, h, r, z);
#else
      printf("a=0x%02x o=%d h=%2d r=%2d z=%2d\n", a, o, *h, *r, z);
#endif
#endif

#if 0
      result(z, a, &o, &h, &r, &c);
#else
      result(z, a, &o, h, r, &c);
#endif

   } while (o<3);

#if DEBUG
#if 0
   printf("a=0x%02x o=%d h=%2d r=%2d\n", a, o, h, r);
#else
   printf("a=0x%02x o=%d h=%2d r=%2d\n", a, o, *h, *r);
#endif
#endif
}

int match() {
   int i;
   int ar = 0, ah = 0, ahi[9] = { 0 }, ari[9] = { 0 };
   int hr = 0, hh = 0, hhi[9] = { 0 }, hri[9] = { 0 };

   for (i=0; i<9; i++) {
      side(&ari[i], &ahi[i]);
      side(&hri[i], &hhi[i]);
   }
   printf("A: ");
   for (i=0; i<9; i++) {
      printf("%2d ", ari[i]);
      ar = ar + ari[i];
      ah = ah + ahi[i];
   }
   printf("%5d %2d %2d\n", ar, ah, 0);
   printf("H: ");
   for (i=0; i<9; i++) {
      printf("%2d ", hri[i]);
      hr = hr + hri[i];
      hh = hh + hhi[i];
   }
   printf("%5d %2d %2d\n", hr, hh, 0);
}

int main(int argc, char *argv[]) {

   if (argc > 1) {
      test_advance(argc, argv);
   }
   else {
      initialize();
      match();
   }

   exit(0);
}
