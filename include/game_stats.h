#ifndef _Game_Stats_
#define _Game_Stats_

// game_stats_update: updates the stats file; pass 0 for loss and 1 for win
void game_stats_update(int win);

// game_stats_print: print the content of the stats file with some minor formatting changes
void game_stats_print();

// game_stats_reset: clears the contents of the stats file
void game_stats_reset();

#endif