#pragma once
#include "picojpeg.h"
#include <string>
#include <vector>

struct JPEGImage {
  unsigned texture_id{};
  std::vector<uint8_t> raw_data{};
  uint64_t width{};
  uint64_t height{};
  std::string scan_type{};

  pjpeg_scan_type_t type{};
  int comps{};
};

JPEGImage decodejpg(std::vector<uint8_t> encoded_img);
