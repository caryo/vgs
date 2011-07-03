#include <stdint.h>

extern uint8_t roll_dice();

extern uint8_t roll_value(uint8_t idx);

extern void roll_counts(uint8_t idx, uint8_t* n1, uint8_t* n2, uint8_t* total);

extern void randomize();

extern void create_pool_of_dice_rolls();

extern void print_pool_of_dice_rolls();

extern void summarize_pool_of_dice_rolls();

extern void print_outcomes();

