#pragma once

#include <jpeglib.h>
#include <turbojpeg.h>

#include <cstdint>
#include <stdexcept>
#include <vector>

#include "display.hh"

struct EncodedFrame {
  std::vector<uint8_t> raw_data;
};

struct Decoder {
  virtual Frame Decode(const EncodedFrame& encoded_frame) = 0;
};

class JpegDecoder : public Decoder {
 public:
  JpegDecoder() { handle_ = tj3Init(TJINIT_DECOMPRESS); }

  Frame Decode(const EncodedFrame& encoded_frame) override {
    int32_t width, height;
    auto ret = tjDecompressHeader(
        handle_, const_cast<uint8_t*>(encoded_frame.raw_data.data()),
        encoded_frame.raw_data.size(), &width, &height);
    if (ret) {
      throw std::runtime_error(tjGetErrorStr2(handle_));
    }
    auto total_size = width * height * 4u;
    auto f = Frame{.width = static_cast<uint32_t>(width),
                   .height = static_cast<uint32_t>(height),
                   .format = Format::kRGBA};

    f.raw_data.resize(total_size);
    ret = tjDecompress2(handle_, encoded_frame.raw_data.data(),
                        encoded_frame.raw_data.size(),
                        const_cast<uint8_t*>(f.raw_data.data()), width,
                        width * 4, height, 8, 0);
    if (ret) {
      throw std::runtime_error(tjGetErrorStr2(handle_));
    }

    return f;
  };

 private:
  tjhandle handle_;
  // std::unique_ptr<
  // std::<
};