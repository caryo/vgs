#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "hbg_roll.h"

static uint8_t n1_count[12];
static uint8_t n2_count[12];
static uint8_t n1n2_count[6][6];
static uint8_t total_count[12];

static roll_counter = 0;
static uint8_t n1_value[256];
static uint8_t n2_value[256];

uint8_t roll_dice() {

   uint8_t n1, n2;

   n1 = random() % 6;
   n1_value[roll_counter] = n1;

   n2 = random() % 6;
   n2_value[roll_counter++] = n2;

   n1_count[n1]++;
   n2_count[n2]++;
   n1n2_count[n1][n2]++;
   total_count[n1+n2]++;

   return (++n1) + (++n2);
}

uint8_t roll_value(uint8_t idx) {

   return (n1_value[idx] + 1) + (n2_value[idx] + 1);
}

void roll_counts(uint8_t idx, uint8_t *n1, uint8_t *n2, uint8_t *total) {

   *n1 = n1_count[idx];
   *n2 = n2_count[idx];
   *total = total_count[idx];
}

void randomize() {

   srand((unsigned int) time(NULL));
}

void create_pool_of_dice_rolls() {

    uint16_t i;

    printf("\n");
    for (i=0; i<256; i++) {
        roll_dice();
    }
    printf("\n");
}

void print_pool_of_dice_rolls() {

    uint16_t i;

    printf("\n");
    for (i=0; i<256; i++) {
        printf("%3d ", roll_value(i));
        if (i>0 && (i+1)%16==0) {
            printf("\n");
        }
    }
    printf("\n");
}

void summarize_pool_of_dice_rolls() {

    uint8_t i;
    uint8_t n1, n2, total;
    uint16_t n;

    printf("\n");
    for (i=0,n=0; i<11; i++,n+=total) {
        roll_counts(i, &n1, &n2, &total);
        printf("outcome: %2d, total = %2d\n", i+2, total);
    }
    printf("\n");
    printf("total = %3d\n", n);
    printf("\n");
}


void print_outcomes() {

    uint8_t i, j;

    printf("\n");
    printf("       1:   2:   3:   4:   5:   6:\n");
    for (i=0;i<6;i++) {
        printf("%2d:", i+1);
        for (j=0;j<6;j++) {
            printf("%5d", n1n2_count[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

