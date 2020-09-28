
#include <iostream>
#include<ctime>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/file.h>
#include <sys/mman.h>
#include <sys/wait.h>
using namespace std;

struct region {
  int row, col;
  bool gameover = false;
  bool parent = false;
  bool child = false;
  bool start = false;
};

void error_and_die(const char *msg) {
  perror(msg);
  exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
  if (strcmp(argv[1],"0")==0) {
    int r;

    const char *memname = "memory";
    const size_t region_size = sysconf(sizeof(struct region));

    int fd = shm_open(memname, O_CREAT | O_TRUNC | O_RDWR, 0666);
    if (fd == -1)
      error_and_die("shm_open");

    r = ftruncate(fd, region_size);
    if (r != 0)
      error_and_die("ftruncate");

    void *ptr = mmap(0, region_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED)
      error_and_die("mmap");
    close(fd);
    struct region* rptr = (struct region*)ptr;

    pid_t pid = fork();

    int map[4][4];
    for (int i = 0; i < 4; i++)
      for (int j = 0; j < 4; j++)
        map[i][j] = 0;
    srand(time(0) + pid);
    int row1 = rand() % 4;
    int col1 = rand() % 4;
    int row2, col2;
    bool boat = true;

    do {
      int s = rand() % 4;
      if (s == 0 && row1 - 1 >= 0)
      {
        row2 = row1 - 1;
        col2 = col1;
      }
      else if (s == 1 && col1 + 1 <= 3)
      {
        col2 = col1 + 1;
        row2 = row1;
      }
      else if (s == 2 && row1 + 1 <= 3)
      {
        row2 = row1 + 1;
        col2 = col1;
      }
      else if (s == 3 && col1 - 1 >= 0)
      {
        col2 = col1 - 1;
        row2 = row1;
      }
      else
        boat = false;
    } while (!boat);

    /*child*/
    if (pid == 0) {
      cout << "[" << getpid() << " Child]: The gunboat: (" << row1 << "," << col1 << ") (" << row2 << "," << col2 << ")" << endl;
      rptr->parent = true;
      int hit = 0;
      int bombs = 0;

      while (!(rptr->gameover)) {
        if (rptr->child) {
          if ((rptr->row == row1&&rptr->col == col1) || (rptr->row == row2&&rptr->col == col2))
          {
            hit++;
            if (hit == 2) {
              cout << "[" << getpid() << " Child]: hit and sinking" << endl;
              cout << "[" << getppid() << " Parent]: " << getppid() << " wins with " << bombs << " bombs" << endl;
              rptr->gameover = true;
              if (rptr->gameover)
                exit(0);
            }
            else
              cout << "[" << getpid() << " Child]: hit" << endl;
          }
          else
            cout << "[" << getpid() << " Child]: missed" << endl;
          do {
            rptr->row = rand() % 4;
            rptr->col = rand() % 4;
          } while (map[rptr->row][rptr->col] != 0);
          map[rptr->row][rptr->col] = 1;
          cout << "[" << getpid() << " Child]: bombing (" << rptr->row << "," << rptr->col << ")" << endl;
          bombs++;
          rptr->child = false;
          rptr->parent = true;
        }
      }
    }

    /*parent*/
    else {
      cout << "[" << getpid() << " Parent]: The gunboat: (" << row1 << "," << col1 << ") (" << row2 << "," << col2 << ")" << endl;
      int hit = 0;
      int bombs = 0;

      while (!(rptr->gameover)) {
        if (rptr->parent)
        {
          if (rptr->start) {
            if ((rptr->row == row1&&rptr->col == col1) || (rptr->row == row2&&rptr->col == col2))
            {
              hit++;
              if (hit == 2) {
                cout << "[" << getpid() << " Parent]: hit and sinking" << endl;
                cout << "[" << getpid() << " Parent]: " << pid << " wins with " << bombs << " bombs" << endl;
                rptr->gameover = true;
                if (rptr->gameover)
                  exit(0);
              }
              else
                cout << "[" << getpid() << " Parent]: hit" << endl;
            }
            else
              cout << "[" << getpid() << " Parent]: missed" << endl;
          }

          do {
            rptr->row = rand() % 4;
            rptr->col = rand() % 4;
          } while (map[rptr->row][rptr->col] != 0);
          map[rptr->row][rptr->col] = 1;
          cout << "[" << getpid() << " Parent]: bombing (" << rptr->row << "," << rptr->col << ")" << endl;
          bombs++;
          rptr->start = true;
          rptr->parent = false;
          rptr->child = true;
        }
      }
    }

    r = munmap(ptr, region_size);
    if (r != 0)
      error_and_die("munmap");

    r = shm_unlink(memname);
    if (r != 0)
      error_and_die("shm_unlink");
  }
  else
    cout << "無進階功能" << endl;
  return 0;
}