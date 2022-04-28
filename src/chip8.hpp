#ifndef CHIP_8_HPP
#define CHIP_8_HPP

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

class chip8 {
public:
  chip8();
  chip8(const char *infile, unsigned short clockspeed = 300);

  ~chip8();

  void loadGame(const char *infile);
  void init();
  void fetch();
  void decode();

  inline bool getScreenRefresh() { return screen_refresh; }
  inline void disableScreenRefresh() { screen_refresh = false; }
  inline std::string getScreen() {
    std::stringstream ss;
    for (int i = 0; i < 32; i++) {
      for (int j = 0; j < 64; j++) {
        std::string temp = display[j][i] ? " " : "#";
        ss << temp;
      }
      ss << '\n';
    }
    return ss.str();
  }
  inline unsigned long getLastUpdated() { return last_updated; }
  inline void setLastUpdated(unsigned long timestamp) {
    last_updated = timestamp;
  }
  inline void decrementTimers() {
    if (delay_timer > 0)
      delay_timer--;
    if (sound_timer > 0)
      sound_timer--;
  }
  inline bool toBeep() {
    if (sound_timer > 0)
      return true;
    return false;
  }
  inline unsigned short getOpcode() { return opcode; }
  inline void setKeyDown(unsigned char kc) {
    key_down = true;
    key_code = kc;
  }
  inline void setKeyUp() { key_down = false; }
  // void execute();
private:
  unsigned char memory[0xfff]; // 4096 bytes of memory
  unsigned char V[16];         // registers
  bool display[64][32];        // 64 x 32 display with a pixel being on/off
  unsigned short pc;
  unsigned short I;
  std::vector<unsigned short> stack;
  unsigned short delay_timer;
  unsigned short sound_timer;
  unsigned short opcode;
  bool screen_refresh;
  unsigned long last_updated = 0;
  bool key_down;
  unsigned char key_code;
  unsigned short clockspeed = 300;
};

#endif