#include "chip8.hpp"

#include <fstream>
#include <iostream>
#include <random>

unsigned char chip8_fontset[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

chip8::chip8() {}

chip8::chip8(const char *infile, unsigned short clkspd) {
  clockspeed = clkspd;
  loadGame(infile);
  init();
}

chip8::~chip8() {}

void chip8::loadGame(const char *infile) {
  std::ifstream in(infile, std::ios_base::binary);
  char current_byte = 0;
  int idx = 0;
  while (in.get(current_byte)) {
    memory[0x200 + idx] = current_byte;
    idx++;
  }
}

void chip8::init() {
  // clear all objects
  for (auto &byte : memory)
    byte = 0x00; // set byte to 0
  for (auto &reg : V)
    reg = 0; // set registers to 0
  for (auto &pixel_x : display)
    for (auto &pixel_x_y : pixel_x)
      pixel_x_y = 0; // set each pixel to 0/false
  pc = 0x200;        // set pc to 512 (start of program)
  I = 0;
  // for (auto &item : stack)
  // item = 0; // remove all stack items
  stack.clear();
  stack.resize(16);
  delay_timer = 0;
  sound_timer = 0;
  opcode = 0;

  // Set fonts
  for (int i = 0; i < 80; i++) {
    memory[i + 0x50] = chip8_fontset[i];
  }

  srand(time(0));

  std::ofstream log("c8.log");
  for (int i = 0x050; i <= 0x09f; i++) {
    log << std::hex << (int)memory[i] << std::dec << " ";
  }
}

void chip8::fetch() {
  opcode = (memory[pc] << 8) | (memory[pc + 1]);
  pc += 2;
}

void chip8::decode() {
  std::ofstream log("c8.log", std::ios_base::app);
  log << opcode << std::endl;
  switch (opcode & 0xf000) {
  case 0x0000:
    switch (opcode) {
    case 0x00e0:
      for (auto &pixel_x : display)
        for (auto &pixel_x_y : pixel_x)
          pixel_x_y = 0; // set each pixel to 0/false
      break;
    case 0x00ee:
      pc = stack.back();
      stack.pop_back();
      break;
    }
    break;
  case 0x1000: {
    pc = (opcode & 0x0fff);
    break;
  }
  case 0x2000: {
    stack.push_back(pc);
    pc = (opcode & 0x0fff);
	break;
  }
  case 0x3000: {
    if (V[(opcode & 0x0f00) >> 8] == (opcode & 0x00ff)) {
      pc += 2;
    }
    break;
  }
  case 0x4000: {
    if (V[(opcode & 0x0f00) >> 8] != (opcode & 0x00ff)) {
      pc += 2;
    }
    break;
  }
  case 0x5000: {
    if (V[(opcode & 0x0f00) >> 8] == V[(opcode & 0x00f0) >> 4]) {
      pc += 2;
    }
    break;
  }
  case 0x6000: {
    V[(opcode & 0x0f00) >> 8] = (opcode & 0x00ff);
    break;
  }
  case 0x7000: {
    V[(opcode & 0x0f00) >> 8] += opcode & 0x00ff;
    break;
  }
  case 0x8000: { // TODO: borrows
    switch (opcode & 0x000f) {
    case 0x0000:
      V[(opcode & 0x0f00) >> 8] = V[(opcode & 0x00f0) >> 4];
      break;
    case 0x0001:
      V[(opcode & 0x0f00) >> 8] |= V[(opcode & 0x00f0) >> 4];
      break;
    case 0x0002:
      V[(opcode & 0x0f00) >> 8] &= V[(opcode & 0x00f0) >> 4];
      break;
    case 0x0003:
      V[(opcode & 0x0f00) >> 8] ^= V[(opcode & 0x00f0) >> 4];
      break;
    case 0x0004:
      V[(opcode & 0x0f00) >> 8] += V[(opcode & 0x00f0) >> 4];
      break;
    case 0x0005:
      V[(opcode & 0x0f00) >> 8] -= V[(opcode & 0x00f0) >> 4];
      break;
    case 0x0006: {
      // V[(opcode & 0x0f00) >> 8] = V[(opcode & 0x00f0) >> 4];
      // get lost value after bit shift
      // https://stackoverflow.com/questions/9461951/how-do-i-get-the-lost-bit-from-a-bit-shift
      bool last_bit = (V[(opcode & 0x0f00) >> 8] & 0x01);
      V[(opcode & 0x0f00) >> 8] >>= 1;
      V[0xf] = last_bit;
      break;
    }
    case 0x0007:
      V[(opcode & 0x0f00) >> 8] =
          V[(opcode & 0x00f0) >> 4] - V[(opcode & 0x0f00) >> 8];
      break;
    }
    break;
  }
  case 0x9000: {
    if (V[(opcode & 0x0f00) >> 8] != V[(opcode & 0x00f0) >> 4]) {
      pc += 2;
    }
    break;
  }
  case 0xa000: {
    I = opcode & 0x0fff;
    break;
  }
  case 0xb000: {
    pc = V[0] + (opcode & 0x0fff);
    break;
  }
  case 0xc000: {
    V[(opcode & 0x0f00) >> 8] = (rand() % (0xff + 1)) & (opcode & 0x00ff);
    break;
  }
  // https://github.com/JamesGriffin/CHIP-8-Emulator/blob/d1870538c009d3d76bd5d91319560edc58d16eae/src/chip8.cpp#L319-L345
  case 0xd000: {
    unsigned short x = V[((opcode & 0x0f00) >> 8)] % 64;
    unsigned short y = V[((opcode & 0x00f0) >> 4)] % 32;
    unsigned short height = opcode & 0x000f;
    unsigned short pixel;

    V[0xF] = 0;
    for (int yline = 0; yline < height; yline++) {
      pixel = memory[I + yline];
      for (int xline = 0; xline < 8; xline++) {
        if ((pixel & (0x80 >> xline)) != 0) {
          if (display[x + xline][y + yline] == 1) {
            V[0xF] = 1;
          }
          display[x + xline][y + yline] ^= 1;
        }
      }
    }

    screen_refresh = true;
    break;
  }
  case 0xe000: {
    switch (opcode & 0x0ff) {
    case 0x009e:
      if (V[(opcode & 0x0f00) >> 8] == key_code && key_down)
        pc += 2;
      break;
    case 0x00a1:
      if (V[(opcode & 0x0f00) >> 8] != key_code || key_down)
        pc += 2;
      break;
    }
	break;
  }
  case 0xf000: {
    switch (opcode & 0x00ff) {
    case 0x0007:
      V[(opcode & 0x0f00) >> 8] = delay_timer;
      break;
    case 0x0015:
      delay_timer = V[(opcode & 0x0f00) >> 8];
      break;
    case 0x0018:
      sound_timer = V[(opcode & 0x0f00) >> 8];
      break;
	case 0x0029:
		I = V[(opcode & 0x0F00) >> 8] * 0x5;
	case 0x0055:
		for (int i = 0; i < (V[(opcode & 0x0f00) >> 8]) + 1; i++) {
			memory[I + i] = V[i];
		}

		I += ((opcode & 0x0f00) >> 8) + 1;
		break;
    case 0x0065:
		for (int i = 0; i < (V[(opcode & 0x0f00) >> 8]) + 1; i++) {
			V[i] = memory[I + i];
		}

		// I += ((opcode & 0x0f00) >> 8) + 1;
		break;
    }
    break;
  }
  default:
    log << "Unknown opcode: " << std::hex << opcode << std::dec << std::endl;
    break;
  }
}
