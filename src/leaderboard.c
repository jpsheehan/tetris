#include <allegro5/allegro.h>

#include "leaderboard.h"
#include "utils.h"

#define MAX_MODE_IDENTIFIERS 4
char mode_identifiers[MAX_MODE_IDENTIFIERS] = {
    'M',
    'S',
    'U',
    'E'};

/*
STORAGE FORMAT:

Each line is formatted like this:

  [ M, N0, N1, N2, N3, N4, '\0', S0, S1, S2, S3, S4, S5, S6, S7, '\n' ]

Where:
  M  = the mode, 'M', 'S', 'U', or 'E'
  Nx = the xth character of the user's name
  Sx = the xth byte of the score (least significant byte first, uint64_t)
*/

static bool does_leaderboard_file_exist(void);
static void sort_highscores(SCORE *scores, size_t n);

static char *leaderboard_filename = "leaderboard.dat";

void serialise_score(ALLEGRO_FILE* file, SCORE* score)
{
  al_fwrite(file, score, sizeof(SCORE));
}

bool deserialise_score(ALLEGRO_FILE* file, SCORE* score)
{
  SCORE temp = { 0 };

  size_t n = al_fread(file, &temp, sizeof(SCORE));

  if (n != sizeof(SCORE)) return false;
  if (temp.name[MAX_USERNAME_CHARACTERS] != '\0') return false;
  if (temp.mode < 0 || temp.mode >= MAX_MODES) return false;

  *score = temp;
  return true;
}

size_t leaderboard_read_high_scores(MODE mode, size_t n, SCORE *buffer)
{
  size_t num_read = 0;

  ALLEGRO_FILE *file = al_fopen(leaderboard_filename, "rb");
  if (file == NULL)
    return 0;

  while (num_read < n && !al_feof(file))
  {
    SCORE score = { 0 };

    if (!deserialise_score(file, &score)) continue;
    if (score.mode != mode) continue;

    buffer[num_read++] = score;
  }

  al_fclose(file);

  sort_highscores(buffer, num_read);

  return num_read;
}

bool leaderboard_is_worthy(MODE mode, uint64_t score)
{
  if (!does_leaderboard_file_exist())
  {
    return true;
  }

  SCORE scores[MAX_LEADERBOARD_ENTRIES] = {0};
  size_t num_read = leaderboard_read_high_scores(mode, MAX_LEADERBOARD_ENTRIES, scores);

  if (num_read == 0)
    return true;
  
  if (num_read < MAX_LEADERBOARD_ENTRIES)
    return true;

  sort_highscores(scores, num_read);
  return (score > scores[MAX((MAX_LEADERBOARD_ENTRIES - 1), num_read)].value);
}

void leaderboard_add_score(MODE mode, const char *name, uint64_t value)
{
  SCORE score = {
      .value = value,
      .name = {0},
  };
  strncpy(score.name, name, MAX_USERNAME_CHARACTERS);

  ALLEGRO_FILE *file = al_fopen(leaderboard_filename, "ab");
  if (file == NULL)
    safe_exit("Could not append to leaderboard", 1);

  serialise_score(file, &score);

  al_fclose(file);
}

static bool does_leaderboard_file_exist(void)
{
  return al_filename_exists(leaderboard_filename);
}

static int score_comparison_desc(const void *a, const void *b)
{
  return ((SCORE *)b)->value - ((SCORE *)a)->value;
}

static void sort_highscores(SCORE *scores, size_t n)
{
  qsort(scores, n, sizeof(SCORE), score_comparison_desc);
}

void leaderboard_dump(void)
{
  SCORE buffer[MAX_LEADERBOARD_ENTRIES] = {0};
  size_t n;
  char *names[MAX_MODES] = {
      "Marathon",
      "Sprint",
      "Ultra",
      "Endless"};

  for (int i = 0; i < MAX_MODES; i++)
  {
    n = leaderboard_read_high_scores((MODE)i, MAX_LEADERBOARD_ENTRIES, buffer);

    if (n == 0)
    {
      printf("No scores for %s mode\n", names[i]);
    }
    else
    {
      printf("Read %lu scores for %s mode\n", n, names[i]);
      for (int j = 0; j < n; j++)
      {
        printf("  %s got %lu points\n", buffer[j].name, buffer[j].value);
      }
      printf("\n");
    }
  }
}