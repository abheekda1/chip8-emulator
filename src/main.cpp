#include "chip8.hpp"

#include <chrono>
#include <fstream>
#include <iostream>
#include <ncurses.h>

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << "Add game path as an argument" << std::endl;
    return EXIT_FAILURE;
  }

  chip8 c8{argv[1], argc >= 3 ? static_cast<unsigned short>(atoi(argv[2])) : static_cast<unsigned short>(300)};

  setlocale(LC_ALL, "");
  initscr();
  curs_set(false);
  nodelay(stdscr, true);
  cbreak();
  noecho();

#ifdef DEBUG 
  std::string opcodeList;
#endif

  for (;;) {
    auto current_time = std::chrono::system_clock::now();
    auto timer = std::chrono::duration_cast<std::chrono::milliseconds>(
                     current_time.time_since_epoch())
                     .count() -
                 c8.getLastUpdated();
    if (timer < (1000 / c8.getClockspeed())) {
      continue;
    }
    c8.setLastUpdated(std::chrono::duration_cast<std::chrono::milliseconds>(
                          current_time.time_since_epoch())
                          .count());

    c8.decrementTimers();
    c8.fetch();
    c8.decode();
    clear();
#ifdef DEBUG // TODO: laggy
    std::stringstream ss;
    ss << std::hex << c8.getOpcode() << std::dec << std::endl;
    opcodeList += ss.str();
    if (opcodeList.length() > 50) {
      opcodeList = opcodeList.substr(5);
    }
    printw(c8.getScreen().c_str());
    refresh();
    //printw(ss.str().c_str());
    printw(opcodeList.c_str());
#else
    if (c8.getScreenRefresh()) {
      printw(c8.getScreen().c_str());
      refresh();
      c8.disableScreenRefresh();
#endif

      char key = getch();
      switch (key) {
      case '1':
        c8.setKeyDown(0x01);
        break;
      case '2':
        c8.setKeyDown(0x02);
        break;
      case '3':
        c8.setKeyDown(0x03);
        break;
      case 'q':
        c8.setKeyDown(0x04);
        break;
      case 'w':
        c8.setKeyDown(0x05);
        break;
      case 'e':
        c8.setKeyDown(0x06);
        break;
      case 'a':
        c8.setKeyDown(0x07);
        break;
      case 's':
        c8.setKeyDown(0x08);
        break;
      case 'd':
        c8.setKeyDown(0x09);
        break;
      case 'z':
        c8.setKeyDown(0x0a);
        break;
      case 'x':
        c8.setKeyDown(0x00);
        break;
      case 'c':
        c8.setKeyDown(0x0c);
        break;
      case '4':
        c8.setKeyDown(0x0c);
        break;
      case 'r':
        c8.setKeyDown(0x0d);
        break;
      case 'f':
        c8.setKeyDown(0x0e);
        break;
      case 'v':
        c8.setKeyDown(0x0f);
        break;
      case ' ':
        nodelay(stdscr, false);
        if (getch() == ' ') {
          nodelay(stdscr, true);
        }
        break;
      case ERR:
        c8.setKeyUp();
        break;
      }

      if (c8.toBeep()) {
        beep();
      }
#ifndef DEBUG // closing parenthesis for the screen refresh if-statement
    }
#endif
    c8.decrementTimers();
  }
}
