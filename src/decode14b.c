/******************************************************************************
 * File: decode14b.c
 * Author: Arkadiusz Dawiec (arkadiusz.dawiec@synchrotron-soleil.fr)
 * Date: 30/05/2019
 *
 * Set of functions to decode raw image data acquired in 14 bits mode.
 *
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <omp.h>

#include "../include/corrections.h"
#include "../include/constants.h"

#define CLEAR_CNTVAL(CNT_VAL) \
        CNT_VAL[0] = 0;\
        CNT_VAL[1] = 0;\
        CNT_VAL[2] = 0;\
        CNT_VAL[3] = 0;\
        CNT_VAL[4] = 0;\
        CNT_VAL[5] = 0;\
        CNT_VAL[6] = 0;\
        CNT_VAL[7] = 0;\

#define WRITE_CNTVAL_BIT(RAW_BYTE, RAW_DATA, RAW_OFFSET, CNT_VAL, CNT_BIT) \
        RAW_BYTE = *(RAW_DATA + *RAW_OFFSET);\
        CNT_VAL[0] |= ((RAW_BYTE & 0x01) << CNT_BIT);\
        CNT_VAL[1] |= (((RAW_BYTE & 0x02) >> 1) << CNT_BIT);\
        CNT_VAL[2] |= (((RAW_BYTE & 0x04) >> 2) << CNT_BIT);\
        CNT_VAL[3] |= (((RAW_BYTE & 0x08) >> 3) << CNT_BIT);\
        CNT_VAL[4] |= (((RAW_BYTE & 0x10) >> 4) << CNT_BIT);\
        CNT_VAL[5] |= (((RAW_BYTE & 0x20) >> 5) << CNT_BIT);\
        CNT_VAL[6] |= (((RAW_BYTE & 0x40) >> 6) << CNT_BIT);\
        CNT_VAL[7] |= (((RAW_BYTE & 0x80) >> 7) << CNT_BIT);\
        RAW_OFFSET++;\

#define WRITE_IMAGE_DATA(IMG_DATA, IMG_OFFSET, CNT_VAL) \
        *(IMG_DATA + *IMG_OFFSET++) = (0x4000 - CNT_VAL[0]) & PIXCNT_MASK_14BITS;\
        *(IMG_DATA + *IMG_OFFSET++) = (0x4000 - CNT_VAL[1]) & PIXCNT_MASK_14BITS;\
        *(IMG_DATA + *IMG_OFFSET++) = (0x4000 - CNT_VAL[2]) & PIXCNT_MASK_14BITS;\
        *(IMG_DATA + *IMG_OFFSET++) = (0x4000 - CNT_VAL[3]) & PIXCNT_MASK_14BITS;\
        *(IMG_DATA + *IMG_OFFSET++) = (0x4000 - CNT_VAL[4]) & PIXCNT_MASK_14BITS;\
        *(IMG_DATA + *IMG_OFFSET++) = (0x4000 - CNT_VAL[5]) & PIXCNT_MASK_14BITS;\
        *(IMG_DATA + *IMG_OFFSET++) = (0x4000 - CNT_VAL[6]) & PIXCNT_MASK_14BITS;\
        *(IMG_DATA + *IMG_OFFSET++) = (0x4000 - CNT_VAL[7]) & PIXCNT_MASK_14BITS;\


#define DECODE_CNTVAL14(CNT_VAL, RAW_BYTE, RAW_DATA, RAW_OFFSET, IMG_DATA, IMG_OFFSET) \
        CLEAR_CNTVAL(CNT_VAL);\
        \
        WRITE_CNTVAL_BIT(RAW_BYTE, RAW_DATA, RAW_OFFSET, CNT_VAL, 13);\
        WRITE_CNTVAL_BIT(RAW_BYTE, RAW_DATA, RAW_OFFSET, CNT_VAL, 12);\
        WRITE_CNTVAL_BIT(RAW_BYTE, RAW_DATA, RAW_OFFSET, CNT_VAL, 11);\
        WRITE_CNTVAL_BIT(RAW_BYTE, RAW_DATA, RAW_OFFSET, CNT_VAL, 10);\
        WRITE_CNTVAL_BIT(RAW_BYTE, RAW_DATA, RAW_OFFSET, CNT_VAL, 9);\
        WRITE_CNTVAL_BIT(RAW_BYTE, RAW_DATA, RAW_OFFSET, CNT_VAL, 8);\
        WRITE_CNTVAL_BIT(RAW_BYTE, RAW_DATA, RAW_OFFSET, CNT_VAL, 7);\
        WRITE_CNTVAL_BIT(RAW_BYTE, RAW_DATA, RAW_OFFSET, CNT_VAL, 6);\
        WRITE_CNTVAL_BIT(RAW_BYTE, RAW_DATA, RAW_OFFSET, CNT_VAL, 5);\
        WRITE_CNTVAL_BIT(RAW_BYTE, RAW_DATA, RAW_OFFSET, CNT_VAL, 4);\
        WRITE_CNTVAL_BIT(RAW_BYTE, RAW_DATA, RAW_OFFSET, CNT_VAL, 3);\
        WRITE_CNTVAL_BIT(RAW_BYTE, RAW_DATA, RAW_OFFSET, CNT_VAL, 2);\
        WRITE_CNTVAL_BIT(RAW_BYTE, RAW_DATA, RAW_OFFSET, CNT_VAL, 1);\
        WRITE_CNTVAL_BIT(RAW_BYTE, RAW_DATA, RAW_OFFSET, CNT_VAL, 0);\
        \
        WRITE_IMAGE_DATA(IMG_DATA, IMG_OFFSET, CNT_VAL);\

#define DECODE_LINE14(CNT_VAL, RAW_BYTE, RAW_DATA, RAW_OFFSET, IMG_DATA, IMG_OFFSET) \
        \
        DECODE_CNTVAL14(CNT_VAL, RAW_BYTE, RAW_DATA, RAW_OFFSET, IMG_DATA, IMG_OFFSET);\
        DECODE_CNTVAL14(CNT_VAL, RAW_BYTE, RAW_DATA, RAW_OFFSET, IMG_DATA, IMG_OFFSET);\
        DECODE_CNTVAL14(CNT_VAL, RAW_BYTE, RAW_DATA, RAW_OFFSET, IMG_DATA, IMG_OFFSET);\
        DECODE_CNTVAL14(CNT_VAL, RAW_BYTE, RAW_DATA, RAW_OFFSET, IMG_DATA, IMG_OFFSET);\
        DECODE_CNTVAL14(CNT_VAL, RAW_BYTE, RAW_DATA, RAW_OFFSET, IMG_DATA, IMG_OFFSET);\
        DECODE_CNTVAL14(CNT_VAL, RAW_BYTE, RAW_DATA, RAW_OFFSET, IMG_DATA, IMG_OFFSET);\
        DECODE_CNTVAL14(CNT_VAL, RAW_BYTE, RAW_DATA, RAW_OFFSET, IMG_DATA, IMG_OFFSET);\
        DECODE_CNTVAL14(CNT_VAL, RAW_BYTE, RAW_DATA, RAW_OFFSET, IMG_DATA, IMG_OFFSET);\
        DECODE_CNTVAL14(CNT_VAL, RAW_BYTE, RAW_DATA, RAW_OFFSET, IMG_DATA, IMG_OFFSET);\
        DECODE_CNTVAL14(CNT_VAL, RAW_BYTE, RAW_DATA, RAW_OFFSET, IMG_DATA, IMG_OFFSET);\
        DECODE_CNTVAL14(CNT_VAL, RAW_BYTE, RAW_DATA, RAW_OFFSET, IMG_DATA, IMG_OFFSET);\
        DECODE_CNTVAL14(CNT_VAL, RAW_BYTE, RAW_DATA, RAW_OFFSET, IMG_DATA, IMG_OFFSET);\
        DECODE_CNTVAL14(CNT_VAL, RAW_BYTE, RAW_DATA, RAW_OFFSET, IMG_DATA, IMG_OFFSET);\
        DECODE_CNTVAL14(CNT_VAL, RAW_BYTE, RAW_DATA, RAW_OFFSET, IMG_DATA, IMG_OFFSET);\
        DECODE_CNTVAL14(CNT_VAL, RAW_BYTE, RAW_DATA, RAW_OFFSET, IMG_DATA, IMG_OFFSET);\
        DECODE_CNTVAL14(CNT_VAL, RAW_BYTE, RAW_DATA, RAW_OFFSET, IMG_DATA, IMG_OFFSET);\


// order of rows and columns in the decoded image
extern const int raw_bytes_order14[];
extern const int img_col_order[];
extern const int img_row_order[];


/*
 * Function to decode single chip raw data to pixel counters values acquired in
 * 14-bits mode.
 *
 * This function decodes single pixel counter data (low or high) from a single
 * chip (128 x 256 pixels). The function allows to decode data directly into a
 * single or composite images.
 * - the single image is a single counter pixel matrix that correspond to a
 *   complete detector, e.g. two chips detector has a size of 256 x 256 pixels.
 * - the composite image is an arrangement of a several single images
 *
 * Arguments:
 * - uint8_t  *p_rawdata  : pointer to the raw data buffer (single chip)
 * - uint16_t *p_image    : pointer to the decoded image data buffer
 * - int       chip_index : index of currently decoded chip (starts with 0)
 * - int       img_x0     : X coordinate (first column) in the decoded image
 * - int       img_y0     : Y coordinate (first row) in the decoded image
 * - int       img_width  : width of the final image (single or composite)
 */
void decode_pixcnt14 (uint8_t  *p_rawdata,
                      uint16_t *p_image,
                      int       chip_index,
                      int       img_x0,
                      int       img_y0,
                      int       img_width)
{
  const int *p_row_idx;
  int        row;
  uint16_t  *p_image_row;
  const int *rawdata_offset;
  const int *imgdata_offset;
  uint8_t    raw_byte;
  uint16_t   pixcnt_val[8] = {0, 0, 0, 0, 0, 0, 0, 0};

  // add offset to the raw and decoded data for a current chip
  p_rawdata += chip_index*CHIP_RAW_SIZE_14BITS;
  p_image += img_y0*img_width + img_x0;

  // get current row number in the decoded data
  p_row_idx = &img_row_order[0];
  row = DET_Y;
  // decode line by line
  do
    {
      // calculate position of the row in the decoded image
      p_image_row = p_image + (*p_row_idx++)*img_width;

      // get offset for raw and decoded data for the currently decoded line
      rawdata_offset = &raw_bytes_order14[0];
      imgdata_offset = &img_col_order[0];

      // macro function to decode a complete line (128 elements)
      DECODE_LINE14 (pixcnt_val,
                     raw_byte,
                     p_rawdata,
                     rawdata_offset,
                     p_image_row,
                     imgdata_offset);
      // increment raw data pointer (go to the next line)
      p_rawdata += RAW_LINE_LENGTH_14BITS;
    }
  while (--row);
}


/*
 * Function to decode single counter 14-bits image.
 *
 * Arguments:
 * - uint8_t  *p_rawdata                 : pointer to the raw data buffer
 * - uint16_t *p_image                   : pointer to the decoded single counter
 *                                         image buffer
 * - int       virtual_pixels_correction : enable inter-chips gap correction
 */
void decode_image14_onecnt (uint8_t  *p_rawdata,
                            uint16_t *p_image,
                            int       virtual_pixels_correction)
{
  int img_x0[] = {0, 128};
  int img_y0[] = {0, 0};
  int img_width = DET_X;
  int img_height = DET_Y;
  int virt_cols[] = {128};
  int virt_nb = 1;
  int chip[4] = {CHIP_0, CHIP_1};
  int i = 0;

  // modify image size and indexes of the single chip data in case virtual
  // correction is enabled
  if (virtual_pixels_correction == 1)
    {
      img_width = DET_X_CORR;
      img_height = DET_Y_CORR;
      img_x0[0] = 0;
      img_x0[1] = 129;
    }

  // parallel region begin
  omp_set_num_threads(2);
  #pragma omp parallel for
  for (i=0; i<2; i++)
    {
      decode_pixcnt14 (p_rawdata,
                       p_image,
                       chip[i],
                       img_x0[i],
                       img_y0[i],
                       img_width);
    }
  // parallel region end

  if (virtual_pixels_correction == 1)
    {
      virtual_columns_correction_image16 ( p_image,
                                          &virt_cols[0],
                                           virt_nb,
                                           img_width,
                                           img_height);
    }
}


/*
 * Function to decode double counter 14-bits image.
 *
 * Arguments:
 * - uint8_t  *p_rawdata_low              : pointer to the low pixel counter raw
 *                                          data buffer
 * - uint8_t  *p_rawdata_high             : pointer to the higher pixel counter
 *                                          raw data buffer
 * - uint16_t *p_image                    : pointer to the decoded double
 *                                          counter image buffer.
 * - int       virtual_pixels_correction  : enable inter-chips gap correction
 */
void decode_image14_twocnt (uint8_t  *p_rawdata_low,
                            uint8_t  *p_rawdata_high,
                            uint16_t *p_image,
                            int       virtual_pixels_correction)
{
  int      img_x0[] = {0, 128, 256, 384};
  int      img_y0[] = {0, 0, 0, 0};
  int      img_width = 2*DET_X;
  int      img_height = DET_Y;
  int      virt_cols[] = {128, 385};
  int      virt_nb = 2;
  uint8_t *p_rawdata_addr[4] = {p_rawdata_low,
                                p_rawdata_low,
                                p_rawdata_high,
                                p_rawdata_high};
  int      chip[4] = {CHIP_0,
                      CHIP_1,
                      CHIP_0,
                      CHIP_1};
  int      i = 0;

  // modify image size and indexes of the single chip data in case virtual
  // correction is enabled
  if (virtual_pixels_correction == 1)
    {
      img_width = 2*DET_X_CORR;
      img_height = DET_Y_CORR;
      img_x0[0] = 0;
      img_x0[1] = 129;
      img_x0[2] = 257;
      img_x0[3] = 386;
    }

  // parallel region begin
  omp_set_num_threads(4);
  #pragma omp parallel for
  for (i=0; i<4; i++)
    {
      decode_pixcnt14 (p_rawdata_addr[i],
                       p_image,
                       chip[i],
                       img_x0[i],
                       img_y0[i],
                       img_width);
    }
  // parallel region begin

  if (virtual_pixels_correction == 1)
    {
      virtual_columns_correction_image16 ( p_image,
                                          &virt_cols[0],
                                           virt_nb,
                                           img_width,
                                           img_height);
    }
}
