#include <chrono>
#include <fstream>
#include <future>
#include <ios>
#include <thread>

#include "decoder.hh"
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

    auto d = JpegDecoder{};
    EncodedFrame f;
    auto frame_file =
        std::ifstream{"/mnt/d/programming/dev_content/graphics/profil_kaj.jpg",
                      std::ios_base::binary};
    f.raw_data =
        std::vector<uint8_t>{std::istreambuf_iterator<char>(frame_file),
                             std::istreambuf_iterator<char>()};

    Frame morda;
    try {
      morda = d.Decode(f);
    } catch (std::exception &e) {
      std::cout << e.what() << std::flush;
    }
    using namespace std::chrono_literals;
    while (stop - now < 9s) {
      display.AddToQueue(red);
      std::this_thread::sleep_for(100ms);
      display.AddToQueue(green);
      std::this_thread::sleep_for(100ms);
      display.AddToQueue(blue);
      std::this_thread::sleep_for(100ms);
      display.AddToQueue(morda);
      std::this_thread::sleep_for(100ms);
      stop = std::chrono::steady_clock::now();
    }
  });

  auto w2 = std::async(std::launch::async, [&display]() {
    using namespace std::chrono_literals;
    auto start = std::chrono::steady_clock::now();
    for (auto now = start; now - start < 10s;
         now = std::chrono::steady_clock::now())
      ;
    display.Stop();
  });
  display.Run();
}