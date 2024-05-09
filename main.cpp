#include <csignal>
#include <cstddef>
#include <cstdio>
#include <fstream>
#include <iomanip>
#include <ios>
#include <iostream>
#include <sstream>

#define SIZE 256

using namespace std;

enum TYPECHAR { DOT, OTHER, CHAR, LINE };

struct Token {
  unsigned char value;
  TYPECHAR type;
};

void hexDump(size_t offset, void *addr, int len) {
  int i;
  Token bufferLine[17];
  unsigned char *pc = static_cast<unsigned char *>(addr);

  for (i = 0; i < len; i++) {
    if ((i % 16) == 0) {
      if (i != 0) {
        for (Token x : bufferLine) {
          if (x.type == TYPECHAR::DOT)
            cout << '.';
          else if (x.type == TYPECHAR::OTHER)
            cout << "\033[1;31m" << '.' << "\033[0m";
          else if (x.type == TYPECHAR::LINE)
            cout << x.value;
          else
            cout << "\033[1;32m" << x.value << "\033[0m";
        }
        cout << endl;
      }

      cout << setw(8) << setfill('0') << hex << offset << ": ";

      offset += 16;
    }
    stringstream hexa;
    hexa << setw(2) << setfill('0') << hex << static_cast<int>(pc[i]);
    if (pc[i] == 0x00) {
      cout << "\e[1m" << hexa.str() << "\e[0m";
    } else if ((pc[i] < 0x20) || (pc[i] > 0x7e)) {
      cout << "\033[1;31m" << hexa.str() << "\033[0m";
    } else {
      cout << "\033[1;32m" << hexa.str() << "\033[0m";
    }

    if ((i % 2) == 1)
      cout << " ";

    if (pc[i] == 0x00) {
      Token new_token;
      new_token.type = TYPECHAR::DOT;
      new_token.value = pc[i];
      bufferLine[i % 16] = new_token;
    } else if ((pc[i] < 0x20) || (pc[i] > 0x7e)) {
      Token new_token;
      new_token.type = TYPECHAR::OTHER;
      new_token.value = pc[i];
      bufferLine[i % 16] = new_token;
    } else {
      Token new_token;
      new_token.type = TYPECHAR::CHAR;
      new_token.value = pc[i];
      bufferLine[i % 16] = new_token;
    }

    Token new_token;
    new_token.type = TYPECHAR::LINE;
    new_token.value = '\0';
    bufferLine[(i % 16) + 1] = new_token;
  }

  while ((i % 16) != 0) {
    cout << "  ";
    if (i % 2 == 1)
      cout << " ";
    i++;
  }
  for (Token x : bufferLine) {
    if (x.type == TYPECHAR::DOT)
      cout << '.';
    else if (x.type == TYPECHAR::OTHER)
      cout << "\033[1;31m" << '.' << "\033[0m";
    else if (x.type == TYPECHAR::LINE)
      cout << x.value;
    else
      cout << "\033[1;32m" << x.value << "\033[0m";
  }
  cout << endl;
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    cerr << "[*] Usage : ./xxd_cpp <file>";
    exit(1);
  }

  ifstream file(argv[1], ios::binary);

  if (!file.is_open()) {
    cerr << "[-] File '" << argv[1] << "' couldn't be open !";
    exit(1);
  }

  unsigned char buffer[SIZE];
  size_t n;
  size_t offset = 0;

  while ((n = file.readsome(reinterpret_cast<char *>(buffer), SIZE)) > 0) {
    hexDump(offset, buffer, n);
    offset += n;
  }

  file.close();

  return 0;
}
