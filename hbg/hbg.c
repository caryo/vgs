#include "hbg_help.h"
#include "hbg_roll.h"
#include "hbg_io.h"
#include "hbg_play.h"

int main(int argc, char *argv[]) {

    randomize();

    prompt_for_help();

    create_pool_of_dice_rolls();
    print_pool_of_dice_rolls();

    prompt_for_enter_key();

    summarize_pool_of_dice_rolls();

    prompt_for_enter_key();

    print_outcomes();

    prompt_for_enter_key();

    print_sample_play_results();
}

