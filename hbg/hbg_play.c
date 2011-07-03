#include <stdint.h>
#include <stdio.h>
#include "hbg_roll.h"

static const char *play_result[11] = {

   " 2 - Wild Pitch",
   " 3 - Triple",
   " 4 - Double",
   " 5 - Walk",
   " 6 - Fly Out",
   " 7 - Ground Out",
   " 8 - Single",
   " 9 - Strike Out",
   "10 - Foul Ball (roll again)",
   "11 - Double Play (if man on base) roll again if bases are empty.",
   "12 - HOME RUN!"
};
   
const char* convert_play_result_to_string(const uint8_t result) {

   if (result < 2 || result > 12)
      return (const char*) 0;

   return play_result[result - 2];
}

void print_sample_play_results() {

   uint8_t i, idx;
   const char* result_s;

   for (i=0;i<16;i++) {
      idx = roll_value(i);
      result_s = convert_play_result_to_string(idx);
      if (result_s) {
         printf("outcome %2d: descr: %s\n", idx, result_s);
      }
   }
}

