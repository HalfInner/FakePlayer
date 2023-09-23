#include <chrono>
#include <future>
#include <thread>

#include "display.hh"

Frame GenerateDumpFrame(uint8_t r = 0, uint8_t g = 0, uint8_t b = 0,
                        uint8_t a = 255) {
  Frame f;
  f.height = 640;
  f.width = 480;
  f.format = Format::kRGBA;
  auto total_size = f.height * f.width * 4;
  f.raw_data.reserve(total_size);
  for (int i = 0; i < total_size; ++i) {
    uint8_t color;
    switch (i % 4) {
      case 0:
        color = r;
        break;
      case 1:
        color = g;
        break;
      case 2:
        color = b;
        break;
      case 3:
        color = a;
        break;
    }
    f.raw_data.push_back(color);
  }
  return f;
}

int main() {
  DisplayOpenGL display;
  // Frame

  auto w = std::async(std::launch::async, [&display]() {
    auto red = GenerateDumpFrame(255);
    auto green = GenerateDumpFrame(0, 255);
    auto blue = GenerateDumpFrame(0, 0, 255);
    auto now = std::chrono::steady_clock::now();
    auto stop = now;
    using namespace std::chrono_literals;
    while (stop - now < 10s) {
        display.AddToQueue(red);
        std::this_thread::sleep_for(100ms);
        display.AddToQueue(green);
        std::this_thread::sleep_for(100ms);
        display.AddToQueue(blue);
        std::this_thread::sleep_for(100ms);
        stop = std::chrono::steady_clock::now();
    }
    display.Stop();
  });
  display.Run();
}