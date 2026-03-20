
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <inttypes.h>

/* show_progress_bar_mini */
void show_progress_bar_mini(int8_t percent) {
    fputc('\r', stdout);
    fputc('[', stdout);
    for (int8_t i = 0; i < percent/10; i++)
    {
        fputc('.', stdout);
    }
    for (int8_t i = percent/10; i < 10; i++)
    {
        fputc('-', stdout);
    }
    fputc(']', stdout);
    fprintf(stdout, "%2d%%", percent);
    fflush(stdout);
}
/* chrono is a start-stop chronometer */
void chrono() {
  static clock_t start, stop;
  static int started = 0;
  if(started) {
    stop = clock();
    printf("Timing %f\n", (double)(stop - start)/CLOCKS_PER_SEC);
    started = 0;
  } else {
    start = clock();
    started = 1;
  }
}