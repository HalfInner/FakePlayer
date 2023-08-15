#include "helper.h"

#include <algorithm>
#include <cstdint>
#include <string_view>
#include <vector>

#include "picojpeg.h"

namespace {
using uint = uint64_t;
using uint8 = uint8_t;

// static FILE *g_pInFile;
static uint g_nInFileSize;
static uint g_nInFileSizeIdx;
static uint g_nInFileOfs;
//------------------------------------------------------------------------------
unsigned char pjpeg_need_bytes_callback(unsigned char *pBuf,
                                        unsigned char buf_size,
                                        unsigned char *pBytes_actually_read,
                                        void * /* pCallback_data */) {
  uint n = std::min(g_nInFileSize - g_nInFileOfs, static_cast<uint>(buf_size));
  // if (n && (fread(pBuf, 1, n, g_pInFile) != n)) return
  // PJPG_STREAM_READ_ERROR;
  *pBytes_actually_read = (unsigned char)(n);
  g_nInFileOfs += n;
  return 0;
}

uint8 *pjpeg_load_from_file(const char *data_buf, int data_buf_size, int *x,
                            int *y, int *comps, pjpeg_scan_type_t *pScan_type,
                            int reduce) {
  pjpeg_image_info_t image_info;
  int mcu_x = 0;
  int mcu_y = 0;
  uint row_pitch;
  uint8 *pImage;
  uint8 status;
  uint decoded_width, decoded_height;
  uint row_blocks_per_mcu, col_blocks_per_mcu;

  *x = 0;
  *y = 0;
  *comps = 0;
  if (pScan_type) *pScan_type = PJPG_GRAYSCALE;

  // g_pInFile = fopen(pFilename, "rb");
  if (!data_buf) return NULL;

  g_nInFileOfs = 0;

  // fseek(g_pInFile, 0, SEEK_END);
  g_nInFileSize = data_buf_size;
  // fseek(g_pInFile, 0, SEEK_SET);

  status = pjpeg_decode_init(&image_info, pjpeg_need_bytes_callback, NULL,
                             (unsigned char)reduce);

  if (status) {
    printf("pjpeg_decode_init() failed with status %u\n", status);
    if (status == PJPG_UNSUPPORTED_MODE) {
      printf("Progressive JPEG files are not supported.\n");
    }

    return NULL;
  }

  if (pScan_type) *pScan_type = image_info.m_scanType;

  // In reduce mode output 1 pixel per 8x8 block.
  decoded_width = reduce ? (image_info.m_MCUSPerRow * image_info.m_MCUWidth) / 8
                         : image_info.m_width;
  decoded_height = reduce
                       ? (image_info.m_MCUSPerCol * image_info.m_MCUHeight) / 8
                       : image_info.m_height;

  row_pitch = decoded_width * image_info.m_comps;
  pImage = (uint8 *)malloc(row_pitch * decoded_height);
  if (!pImage) {
    return NULL;
  }

  row_blocks_per_mcu = image_info.m_MCUWidth >> 3;
  col_blocks_per_mcu = image_info.m_MCUHeight >> 3;

  for (;;) {
    int y, x;
    uint8 *pDst_row;

    status = pjpeg_decode_mcu();

    if (status) {
      if (status != PJPG_NO_MORE_BLOCKS) {
        printf("pjpeg_decode_mcu() failed with status %u\n", status);

        free(pImage);
        return NULL;
      }

      break;
    }

    if (mcu_y >= image_info.m_MCUSPerCol) {
      free(pImage);
      return NULL;
    }

    if (reduce) {
      // In reduce mode, only the first pixel of each 8x8 block is valid.
      pDst_row = pImage + mcu_y * col_blocks_per_mcu * row_pitch +
                 mcu_x * row_blocks_per_mcu * image_info.m_comps;
      if (image_info.m_scanType == PJPG_GRAYSCALE) {
        *pDst_row = image_info.m_pMCUBufR[0];
      } else {
        uint y, x;
        for (y = 0; y < col_blocks_per_mcu; y++) {
          uint src_ofs = (y * 128U);
          for (x = 0; x < row_blocks_per_mcu; x++) {
            pDst_row[0] = image_info.m_pMCUBufR[src_ofs];
            pDst_row[1] = image_info.m_pMCUBufG[src_ofs];
            pDst_row[2] = image_info.m_pMCUBufB[src_ofs];
            pDst_row += 3;
            src_ofs += 64;
          }

          pDst_row += row_pitch - 3 * row_blocks_per_mcu;
        }
      }
    } else {
      // Copy MCU's pixel blocks into the destination bitmap.
      pDst_row = pImage + (mcu_y * image_info.m_MCUHeight) * row_pitch +
                 (mcu_x * image_info.m_MCUWidth * image_info.m_comps);

      for (y = 0; y < image_info.m_MCUHeight; y += 8) {
        const int by_limit = std::min(
            8, image_info.m_height - (mcu_y * image_info.m_MCUHeight + y));

        for (x = 0; x < image_info.m_MCUWidth; x += 8) {
          uint8 *pDst_block = pDst_row + x * image_info.m_comps;

          // Compute source byte offset of the block in the decoder's MCU
          // buffer.
          uint src_ofs = (x * 8U) + (y * 16U);
          const uint8 *pSrcR = image_info.m_pMCUBufR + src_ofs;
          const uint8 *pSrcG = image_info.m_pMCUBufG + src_ofs;
          const uint8 *pSrcB = image_info.m_pMCUBufB + src_ofs;

          const int bx_limit = std::min(
              8, image_info.m_width - (mcu_x * image_info.m_MCUWidth + x));

          if (image_info.m_scanType == PJPG_GRAYSCALE) {
            int bx, by;
            for (by = 0; by < by_limit; by++) {
              uint8 *pDst = pDst_block;

              for (bx = 0; bx < bx_limit; bx++) *pDst++ = *pSrcR++;

              pSrcR += (8 - bx_limit);

              pDst_block += row_pitch;
            }
          } else {
            int bx, by;
            for (by = 0; by < by_limit; by++) {
              uint8 *pDst = pDst_block;

              for (bx = 0; bx < bx_limit; bx++) {
                pDst[0] = *pSrcR++;
                pDst[1] = *pSrcG++;
                pDst[2] = *pSrcB++;
                pDst += 3;
              }

              pSrcR += (8 - bx_limit);
              pSrcG += (8 - bx_limit);
              pSrcB += (8 - bx_limit);

              pDst_block += row_pitch;
            }
          }
        }

        pDst_row += (row_pitch * 8);
      }
    }

    mcu_x++;
    if (mcu_x == image_info.m_MCUSPerRow) {
      mcu_x = 0;
      mcu_y++;
    }
  }

  *x = decoded_width;
  *y = decoded_height;
  *comps = image_info.m_comps;

  return pImage;
}
}  // namespace

unsigned char ReadBytes(unsigned char *pBuf, unsigned char buf_size,
                        unsigned char *pBytes_actually_read,
                        void *pCallback_data) {
  auto encoded_img = static_cast<std::vector<uint8_t> *>(pCallback_data);
  *pBytes_actually_read =
      std::min(static_cast<size_t>(buf_size), encoded_img->size());
  auto ret =
      memcpy_s(pBuf, buf_size, encoded_img->data(), *pBytes_actually_read);
  encoded_img->erase(std::begin(*encoded_img),
                     std::begin(*encoded_img) + *pBytes_actually_read);
  return ret;
}

JPEGImage decodejpg(std::vector<uint8_t> encoded_img) {
  JPEGImage img;
  // auto buf = pjpeg_load_from_file(
  //     reinterpret_cast<const char *>(encoded_img.data()), encoded_img.size(),
  //     reinterpret_cast<int *>(&img.width),
  //     reinterpret_cast<int *>(&img.height), &img.comps, &img.type, 0);

  pjpeg_need_bytes_callback_t cb = ReadBytes;
  pjpeg_image_info_t pInfo;
  pjpeg_decode_init(&pInfo, cb, static_cast<void *>(&encoded_img), 0);

  img.width = pInfo.m_width;
  img.height = pInfo.m_height;
  img.scan_type = pInfo.m_scanType;
  img.comps = pInfo.m_comps;
  // pInfo.

  return img;
}