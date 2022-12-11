#include <ctime>
#include <memory>

using namespace std;

void initField(int *field, int len) {
  if (len) {
    initField(&field[1], len - 1);
    field[len - 1] = -1;
  }
}

void initBombs(int *field, int len, int bombs) {
  if (bombs) {
    initBombs(field, len, bombs - 1);
    int l = rand();
    while (l) {
      l %= len;
      l -= field[l];
    }
    field[l] = 0;
    field[l ? l : len - 1] -= 1;
  }
}

void main2(int sizeX, int sizeY, int bombs) {
  srand(time(nullptr));
  int fieldCount = sizeX * sizeY;

  int *field = (int *)malloc(fieldCount);
  initField(field, fieldCount);
  initBombs(field, fieldCount, bombs);

  free(field);
}
