/*
game_stats.c: reads from and writes to stats.txt, which contains some basic numerical statistics about games played
*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

static char * FILE_PATH = "./stats/stats.txt";

enum statlines{NUM_GAMES, NUM_WINS, NUM_LOSSES, PERCENT_WON, PERCENT_LOST};

static int get_num_from_string(const char * line, int length) {
    char ret[length];
    int count = 0;
    int p = 0;
    while (line[p] != '\n' && line[p] != EOF) {
        if (isdigit(line[p])) {
            ret[count] = line[p];
            count += 1;
        }
        p += 1;
    }
    return atoi(ret);
}

static int get_num_from_file_line(FILE * fp, int line) {
    rewind(fp);
    char buffer[255];
    int curline = 0;
    while(fgets(buffer, 255, fp)) {
        if (curline == line) {
            return get_num_from_string(buffer, 255);
        }
        curline += 1;
    }
    return -1;
}

static int num_games(FILE * fp) {
    return get_num_from_file_line(fp, NUM_GAMES);
}

static int num_wins(FILE * fp) {
    return get_num_from_file_line(fp, NUM_WINS);
}

static int num_losses(FILE * fp) {
    return get_num_from_file_line(fp, NUM_LOSSES);
}

void game_stats_update(int win) {
    if (win > 1 || win < 0) return; // value should be 0 or 1
    FILE * fp = fopen(FILE_PATH, "r"); // rather than open and close for each read, open once and pass pointer
    int games = num_games(fp) + 1;
    int wins = num_wins(fp) + win;
    int losses = num_losses(fp) + 1 - win;
    float percent_won = (float)wins / games;
    float percent_lost = (float)losses / games;
    fclose(fp);
    fp = fopen(FILE_PATH, "w");
    fprintf(fp, "Games: %d", games);
    fprintf(fp, "\nWins: %d", wins);
    fprintf(fp, "\nLosses: %d", losses);
    fprintf(fp, "\nPercent Won: %.2f", percent_won * 100);
    fprintf(fp, "\nPercent Lost: %.2f", percent_lost * 100);
    fclose(fp);
}

void game_stats_print() {
    FILE * fp = fopen(FILE_PATH, "r");
    char buffer[255];
    printf("\nStats:\n");
    while(fgets(buffer, 255, fp)) {
        printf("  • %s", buffer);
    }
    printf("\n");
    fflush(stdout);
    fclose(fp);
}

void game_stats_reset() {
    FILE * fp = fopen(FILE_PATH, "w");
    fprintf(fp, "Games: %d", 0);
    fprintf(fp, "\nWins: %d", 0);
    fprintf(fp, "\nLosses: %d", 0);
    fprintf(fp, "\nPercent Won: %.2f", 0.00);
    fprintf(fp, "\nPercent Lost: %.2f", 0.00);
    fclose(fp);
    fclose(fp);
}

/*
int main(){
    game_stats_update(0);
    game_stats_print();
}
*/