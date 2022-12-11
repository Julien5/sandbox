#include <algorithm>
#include <ctime>
#include <iostream>
#include <vector>

using namespace std;

struct Field {
  bool mBomb = false;
  bool mHidden = true;
  int mCount = 0;
};

ostream &operator<<(ostream &o, const Field &f) {
  if (f.mBomb) {
    o << '*';
  } else {
    if (f.mCount) {
      o << f.mCount;
    } else {
      o << '.';
    }
  }
  return o;
}

int main(int argc, char *argv[]) {
  if (argc >= 4) {
    srand(time(0));

    pair<unsigned, unsigned> boardSize;
    boardSize.first = stoul(argv[1]);
    boardSize.second = stoul(argv[2]);
    unsigned bombs = stoul(argv[3]);

    if (bombs < (boardSize.first * boardSize.second)) {

      // init

      vector<vector<Field>> fields;
      fields.resize(boardSize.second + 2, vector<Field>{boardSize.first + 2});

      std::vector<std::pair<unsigned, unsigned>> freeFields;
      freeFields.reserve((boardSize.first + 2) * (boardSize.second + 2) + 10);
      for (unsigned y = 1; y < (fields.size() - 1); ++y) {
        for (unsigned x = 1; x < (fields[y].size() - 1); ++x) {
          freeFields.push_back(make_pair(x, y));
        }
      }

      // fill

      for (unsigned c = 0; c < bombs; ++c) {
        auto iter = freeFields.begin();
        iter += rand() % freeFields.size();
        Field &f = fields[iter->second][iter->first];
        f.mBomb = true;
        for (int xf = -1; xf <= 1; ++xf) {
          for (int yf = -1; yf <= 1; ++yf) {
            fields[yf + iter->second][xf + iter->first].mCount += 1;
          }
        }
        freeFields.erase(iter);
      }

      // print

      for (unsigned y = 1; y < (fields.size() - 1); ++y) {
        for (unsigned x = 1; x < (fields[y].size() - 1); ++x) {
          cout << fields[y][x];
        }
        cout << '\n';
      }
    } else {
      cerr << "Bomb count greater board fields" << std::endl;
    }
  } else {
    cerr << "Missing arguments ! \n"
         << "minesweeper  x-axis y-axis bombs" << std::endl;
  }
  return 0;
}
