#pragma once

#include <GL/freeglut.h>
#include <GL/freeglut_std.h>

#include <deque>
#include <functional>
#include <iostream>
#include <mutex>
#include <vector>

enum class Format { kRGBA, kRGB, kBGR, kBGRA, kNone };

class Frame {
 public:
  std::vector<uint8_t> raw_data{};
  uint32_t width{};
  uint32_t height{};
  Format format{Format::kNone};
};

class DisplayOpenGL {
 public:
  explicit DisplayOpenGL();

  void AddToQueue(const Frame &frame);

  void Run();
  void Stop();

 private:
  static DisplayOpenGL *g_self;
  void draw();

  std::mutex mx_display_texture_queue_{};
  std::deque<unsigned> display_texture_queue_{};
  std::deque<Frame> frames_{};

  bool is_running_ {false};
};
