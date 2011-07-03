#include <stdio.h>

#include "hbg_help.h"
#include "hbg_io.h"

static const char help_text1[] = {

"  \n"
"  Players take turns (innings) rolling the dice and moving their\n"
"  players using the chart below, following standard baseball rules for\n"
"  outs, extra innings, etc.\n"
"  \n"
"  2 - Wild Pitch (runners advance 1 base)\n"
"  3 - Triple\n"
"  4 - Double\n"
"  5 - Walk\n"
"  6 - Fly Out\n"
"  7 - Ground Out\n"
"  8 - Single\n"
"  9 - Strike Out\n"
"  10 - Foul Ball (roll again)\n"
"  11 - Double Play (if man on base) roll again if bases are empty.\n"
"  12 - HOME RUN!\n"
"  \n"
};

static const char help_text2[] = {

"  \n"
"  Runners advance the same number of bases as the batter.  For example,\n"
"  if there is a man on third base and the batter singles, the runner on\n"
"  third will score (advancing one base to home.)  Roll to see who is the\n"
"  home team.  Keep track of runs and outs during an inning with your\n"
"  opponent's pegs.  Play nine innings, keeping score using the Outfield\n"
"  Scoreboard.  Use a soft cloth or your fingertip to erase the score to\n"
"  begin a new baseball game.  For added fun, on an extra sheet of paper\n"
"  use your favorite baseball team's roster and keep track of each player's\n"
"  statistics.  For batters, try keeping track of singles, doubles, home\n"
"  runs- even batting average!  For pitchers, try tracking strike outs,\n"
"  walks, scoreless innings- even wins!\n"
"  \n"

};

void prompt_for_help() {

    printf("%s", help_text1);
    prompt_for_enter_key();

    printf("%s", help_text2);
    prompt_for_enter_key();
}

