/******************************************************************************
 * File: decode2b.c
 * Author: Arkadiusz Dawiec (arkadiusz.dawiec@synchrotron-soleil.fr)
 * Date: 30/05/2019
 *
 * Set of functions to decode raw image data acquired in 2 bits mode.
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
        *(IMG_DATA + *IMG_OFFSET++) = (0x04 - CNT_VAL[0]) & PIXCNT_MASK_2BITS;\
        *(IMG_DATA + *IMG_OFFSET++) = (0x04 - CNT_VAL[1]) & PIXCNT_MASK_2BITS;\
        *(IMG_DATA + *IMG_OFFSET++) = (0x04 - CNT_VAL[2]) & PIXCNT_MASK_2BITS;\
        *(IMG_DATA + *IMG_OFFSET++) = (0x04 - CNT_VAL[3]) & PIXCNT_MASK_2BITS;\
        *(IMG_DATA + *IMG_OFFSET++) = (0x04 - CNT_VAL[4]) & PIXCNT_MASK_2BITS;\
        *(IMG_DATA + *IMG_OFFSET++) = (0x04 - CNT_VAL[5]) & PIXCNT_MASK_2BITS;\
        *(IMG_DATA + *IMG_OFFSET++) = (0x04 - CNT_VAL[6]) & PIXCNT_MASK_2BITS;\
        *(IMG_DATA + *IMG_OFFSET++) = (0x04 - CNT_VAL[7]) & PIXCNT_MASK_2BITS;\

#define ACCUMULATE_IMAGE_DATA(IMG_DATA, IMG_OFFSET, CNT_VAL) \
        *(IMG_DATA + *IMG_OFFSET++) += (0x04 - CNT_VAL[0]) & PIXCNT_MASK_2BITS;\
        *(IMG_DATA + *IMG_OFFSET++) += (0x04 - CNT_VAL[1]) & PIXCNT_MASK_2BITS;\
        *(IMG_DATA + *IMG_OFFSET++) += (0x04 - CNT_VAL[2]) & PIXCNT_MASK_2BITS;\
        *(IMG_DATA + *IMG_OFFSET++) += (0x04 - CNT_VAL[3]) & PIXCNT_MASK_2BITS;\
        *(IMG_DATA + *IMG_OFFSET++) += (0x04 - CNT_VAL[4]) & PIXCNT_MASK_2BITS;\
        *(IMG_DATA + *IMG_OFFSET++) += (0x04 - CNT_VAL[5]) & PIXCNT_MASK_2BITS;\
        *(IMG_DATA + *IMG_OFFSET++) += (0x04 - CNT_VAL[6]) & PIXCNT_MASK_2BITS;\
        *(IMG_DATA + *IMG_OFFSET++) += (0x04 - CNT_VAL[7]) & PIXCNT_MASK_2BITS;\

#define DECODE_CNTVAL2(CNT_VAL, RAW_BYTE, RAW_DATA, RAW_OFFSET, IMG_DATA, IMG_OFFSET) \
        CLEAR_CNTVAL(CNT_VAL);\
        \
        WRITE_CNTVAL_BIT(RAW_BYTE, RAW_DATA, RAW_OFFSET, CNT_VAL, 1);\
        WRITE_CNTVAL_BIT(RAW_BYTE, RAW_DATA, RAW_OFFSET, CNT_VAL, 0);\
        \
        WRITE_IMAGE_DATA(IMG_DATA, IMG_OFFSET, CNT_VAL);\

#define DECODE_AND_ACCUMULATE_CNTVAL2(CNT_VAL, RAW_BYTE, RAW_DATA, RAW_OFFSET, IMG_DATA, IMG_OFFSET) \
        CLEAR_CNTVAL(CNT_VAL);\
        \
        WRITE_CNTVAL_BIT(RAW_BYTE, RAW_DATA, RAW_OFFSET, CNT_VAL, 1);\
        WRITE_CNTVAL_BIT(RAW_BYTE, RAW_DATA, RAW_OFFSET, CNT_VAL, 0);\
        \
        ACCUMULATE_IMAGE_DATA(IMG_DATA, IMG_OFFSET, CNT_VAL);\

#define DECODE_LINE2(CNT_VAL, RAW_BYTE, RAW_DATA, RAW_OFFSET, IMG_DATA, IMG_OFFSET) \
        \
        DECODE_CNTVAL2(CNT_VAL, RAW_BYTE, RAW_DATA, RAW_OFFSET, IMG_DATA, IMG_OFFSET);\
        DECODE_CNTVAL2(CNT_VAL, RAW_BYTE, RAW_DATA, RAW_OFFSET, IMG_DATA, IMG_OFFSET);\
        DECODE_CNTVAL2(CNT_VAL, RAW_BYTE, RAW_DATA, RAW_OFFSET, IMG_DATA, IMG_OFFSET);\
        DECODE_CNTVAL2(CNT_VAL, RAW_BYTE, RAW_DATA, RAW_OFFSET, IMG_DATA, IMG_OFFSET);\
        DECODE_CNTVAL2(CNT_VAL, RAW_BYTE, RAW_DATA, RAW_OFFSET, IMG_DATA, IMG_OFFSET);\
        DECODE_CNTVAL2(CNT_VAL, RAW_BYTE, RAW_DATA, RAW_OFFSET, IMG_DATA, IMG_OFFSET);\
        DECODE_CNTVAL2(CNT_VAL, RAW_BYTE, RAW_DATA, RAW_OFFSET, IMG_DATA, IMG_OFFSET);\
        DECODE_CNTVAL2(CNT_VAL, RAW_BYTE, RAW_DATA, RAW_OFFSET, IMG_DATA, IMG_OFFSET);\
        DECODE_CNTVAL2(CNT_VAL, RAW_BYTE, RAW_DATA, RAW_OFFSET, IMG_DATA, IMG_OFFSET);\
        DECODE_CNTVAL2(CNT_VAL, RAW_BYTE, RAW_DATA, RAW_OFFSET, IMG_DATA, IMG_OFFSET);\
        DECODE_CNTVAL2(CNT_VAL, RAW_BYTE, RAW_DATA, RAW_OFFSET, IMG_DATA, IMG_OFFSET);\
        DECODE_CNTVAL2(CNT_VAL, RAW_BYTE, RAW_DATA, RAW_OFFSET, IMG_DATA, IMG_OFFSET);\
        DECODE_CNTVAL2(CNT_VAL, RAW_BYTE, RAW_DATA, RAW_OFFSET, IMG_DATA, IMG_OFFSET);\
        DECODE_CNTVAL2(CNT_VAL, RAW_BYTE, RAW_DATA, RAW_OFFSET, IMG_DATA, IMG_OFFSET);\
        DECODE_CNTVAL2(CNT_VAL, RAW_BYTE, RAW_DATA, RAW_OFFSET, IMG_DATA, IMG_OFFSET);\
        DECODE_CNTVAL2(CNT_VAL, RAW_BYTE, RAW_DATA, RAW_OFFSET, IMG_DATA, IMG_OFFSET);\

#define DECODE_AND_ACCUMULATE_LINE2(CNT_VAL, RAW_BYTE, RAW_DATA, RAW_OFFSET, IMG_DATA, IMG_OFFSET) \
        \
        DECODE_AND_ACCUMULATE_CNTVAL2(CNT_VAL, RAW_BYTE, RAW_DATA, RAW_OFFSET, IMG_DATA, IMG_OFFSET);\
        DECODE_AND_ACCUMULATE_CNTVAL2(CNT_VAL, RAW_BYTE, RAW_DATA, RAW_OFFSET, IMG_DATA, IMG_OFFSET);\
        DECODE_AND_ACCUMULATE_CNTVAL2(CNT_VAL, RAW_BYTE, RAW_DATA, RAW_OFFSET, IMG_DATA, IMG_OFFSET);\
        DECODE_AND_ACCUMULATE_CNTVAL2(CNT_VAL, RAW_BYTE, RAW_DATA, RAW_OFFSET, IMG_DATA, IMG_OFFSET);\
        DECODE_AND_ACCUMULATE_CNTVAL2(CNT_VAL, RAW_BYTE, RAW_DATA, RAW_OFFSET, IMG_DATA, IMG_OFFSET);\
        DECODE_AND_ACCUMULATE_CNTVAL2(CNT_VAL, RAW_BYTE, RAW_DATA, RAW_OFFSET, IMG_DATA, IMG_OFFSET);\
        DECODE_AND_ACCUMULATE_CNTVAL2(CNT_VAL, RAW_BYTE, RAW_DATA, RAW_OFFSET, IMG_DATA, IMG_OFFSET);\
        DECODE_AND_ACCUMULATE_CNTVAL2(CNT_VAL, RAW_BYTE, RAW_DATA, RAW_OFFSET, IMG_DATA, IMG_OFFSET);\
        DECODE_AND_ACCUMULATE_CNTVAL2(CNT_VAL, RAW_BYTE, RAW_DATA, RAW_OFFSET, IMG_DATA, IMG_OFFSET);\
        DECODE_AND_ACCUMULATE_CNTVAL2(CNT_VAL, RAW_BYTE, RAW_DATA, RAW_OFFSET, IMG_DATA, IMG_OFFSET);\
        DECODE_AND_ACCUMULATE_CNTVAL2(CNT_VAL, RAW_BYTE, RAW_DATA, RAW_OFFSET, IMG_DATA, IMG_OFFSET);\
        DECODE_AND_ACCUMULATE_CNTVAL2(CNT_VAL, RAW_BYTE, RAW_DATA, RAW_OFFSET, IMG_DATA, IMG_OFFSET);\
        DECODE_AND_ACCUMULATE_CNTVAL2(CNT_VAL, RAW_BYTE, RAW_DATA, RAW_OFFSET, IMG_DATA, IMG_OFFSET);\
        DECODE_AND_ACCUMULATE_CNTVAL2(CNT_VAL, RAW_BYTE, RAW_DATA, RAW_OFFSET, IMG_DATA, IMG_OFFSET);\
        DECODE_AND_ACCUMULATE_CNTVAL2(CNT_VAL, RAW_BYTE, RAW_DATA, RAW_OFFSET, IMG_DATA, IMG_OFFSET);\
        DECODE_AND_ACCUMULATE_CNTVAL2(CNT_VAL, RAW_BYTE, RAW_DATA, RAW_OFFSET, IMG_DATA, IMG_OFFSET);\


// order of rows and columns in the decoded image
extern const int raw_bytes_order2[];
extern const int img_col_order[];
extern const int img_row_order[];

/*
 * Function to decode single chip raw data to pixel counters values acquired in
 * 2-bits mode.
 *
 * This function decodes single pixel counter data (low or high) from a single
 * chip (128 x 256 pixels). The function allows to decode data directly into a
 * single or composite images.
 * - the single image is a single counter pixel matrix that correspond to a
 *   complete detector, e.g. two chips detector has a size of 256 x 256 pixels.
 * - the composite image is an arrangement of a several single images
 *
 * Arguments:
 * - uint8_t *p_rawdata  : pointer to the raw data buffer (single chip)
 * - uint8_t *p_image    : pointer to the decoded image data buffer
 * - int      chip_index : index of currently decoded chip (starts with 0)
 * - int      img_x0     : X coordinate (first column) in the decoded image
 * - int      img_y0     : Y coordinate (first row) in the decoded image
 * - int      img_width  : width of the final image (single or composite)
 */
void decode_pixcnt2 (uint8_t *p_rawdata,
                     uint8_t *p_image,
                     int      chip_index,
                     int      img_x0,
                     int      img_y0,
                     int      img_width)
{
  const int *p_row_idx;
  int        row;
  uint8_t   *p_image_row;
  const int *rawdata_offset;
  const int *imgdata_offset;
  uint8_t    raw_byte;
  uint8_t    pixcnt_val[8] = {0, 0, 0, 0, 0, 0, 0, 0};

  // add offset to the raw and decoded data for a current chip
  p_rawdata += chip_index*CHIP_RAW_SIZE_2BITS;
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
      rawdata_offset = &raw_bytes_order2[0];
      imgdata_offset = &img_col_order[0];

      // macro function to decode a complete line (128 elements)
      DECODE_LINE2 (pixcnt_val,
                    raw_byte,
                    p_rawdata,
                    rawdata_offset,
                    p_image_row,
                    imgdata_offset);
      // increment raw data pointer (go to the next line)
      p_rawdata += RAW_LINE_LENGTH_2BITS;
    }
  while (--row);
}


/*
 * Function to decode and accumulate single chip raw data to pixel counters
 * values acquired in 2-bits mode.
 *
 * This function decodes single pixel counter data (low or high) from a single
 * chip (128 x 256 pixels) and add to a final decoded image. Therefore, a series
 * of 2-bots images can be summed into a single accumulated image.
 *
 * Similarly to decode_pixcnt2 function, data can be decoded and accumulated
 * directly into a single or composite images.
 * - the single image is a single counter pixel matrix that correspond to a
 *   complete detector, e.g. two chips detector has a size of 256 x 256 pixels.
 * - the composite image is an arrangement of a several single images
 *
 * Arguments:
 * - uint8_t  *p_rawdata  : pointer to the raw data buffer (single chip)
 * - uint32_t *p_image    : pointer to the decoded image data buffer
 * - int       chip_index : index of currently decoded chip (starts with 0)
 * - int       img_x0     : X coordinate (first column) in the decoded image
 * - int       img_y0     : Y coordinate (first row) in the decoded image
 * - int       img_width  : width of the final image (single or composite)
 */
void decode_and_accumulate_pixcnt2 (uint8_t  *p_rawdata,
                                    uint32_t *p_image,
                                    int       chip_index,
                                    int       img_x0,
                                    int       img_y0,
                                    int       img_width)
{
  const int *p_row_idx;
  int        row;
  uint32_t  *p_image_row;
  const int *rawdata_offset;
  const int *imgdata_offset;
  uint8_t    raw_byte;
  uint32_t   pixcnt_val[8] = {0, 0, 0, 0, 0, 0, 0, 0};

  // add offset to the raw and decoded data for a current chip
  p_rawdata += chip_index*CHIP_RAW_SIZE_2BITS;
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
      rawdata_offset = &raw_bytes_order2[0];
      imgdata_offset = &img_col_order[0];

      // macro function to decode a complete line (128 elements)
      DECODE_AND_ACCUMULATE_LINE2 (pixcnt_val,
                                   raw_byte,
                                   p_rawdata,
                                   rawdata_offset,
                                   p_image_row,
                                   imgdata_offset);
      // increment raw data pointer (go to the next line)
      p_rawdata += RAW_LINE_LENGTH_2BITS;
    }
  while (--row);
}


/*
 * Function to decode single counter 2-bits image.
 *
 * Because, the 2-bits images are acquired with a very short gate, thus very
 * low statistics the inter-chips gaps correction.
 *
 * Arguments:
 * - uint8_t *p_rawdata : pointer to the raw data buffer
 * - uint8_t *p_image   : pointer to the decoded single counter image buffer
 */
void decode_image2_onecnt (uint8_t *p_rawdata,
                           uint8_t *p_image)
{
  int img_x0[] = {0, 128};
  int img_y0[] = {0, 0};
  int img_width = DET_X;
  int chip[4] = {CHIP_0, CHIP_1};
  int i = 0;

  omp_set_num_threads(2);
  #pragma omp parallel for
  for (i=0; i<2; i++)
    {
      decode_pixcnt2 (p_rawdata,
                      p_image,
                      chip[i],
                      img_x0[i],
                      img_y0[i],
                      img_width);
    }
}


/*
 * Function to decode double counter 2-bits image.
 *
 * Because, the 2-bits images are acquired with a very short gate, thus very
 * low statistics the inter-chips gaps correction.
 *
 * Arguments:
 * - uint8_t *p_rawdata_low  : pointer to the low pixel counter raw data buffer
 * - uint8_t *p_rawdata_high : pointer to the higher pixel counter raw data buffer
 * - uint8_t *p_image        : pointer to the decoded double counter image buffer
 */
void decode_image2_twocnt (uint8_t *p_rawdata_low,
                           uint8_t *p_rawdata_high,
                           uint8_t *p_image)
{
  int      img_x0[] = {0, 128, 256, 384};
  int      img_y0[] = {0, 0, 0, 0};
  int      img_width = 2*DET_X;
  uint8_t *p_rawdata_addr[4] = {p_rawdata_low,
                                p_rawdata_low,
                                p_rawdata_high,
                                p_rawdata_high};
  int      chip[4] = {CHIP_0,
                      CHIP_1,
                      CHIP_0,
                      CHIP_1};
  int      i = 0;

  omp_set_num_threads(4);
  #pragma omp parallel for
  for (i=0; i<4; i++)
    {
      decode_pixcnt2 (p_rawdata_addr[i],
                      p_image,
                      chip[i],
                      img_x0[i],
                      img_y0[i],
                      img_width);
    }
}


/*
 * Function to decode pump and probe-probe 2-bits images.
 *
 * The series of single counters raw data images acquired in 2-bits mode are
 * decoded, summed and arranged into a composite image assembled out of 4 single
 * counters images placed in 2x2 array. The final size size before inter-chips
 * gap correction is 512 x 512 pixels.
 *
 * Images accumulation
 * The series of the images acquired during the acquisition has the specific order:
 *  image:   | IMG 0 | IMG 1 | IMG 2 | IMG 3 | IMG 4 | IMG 5 | ...
 *  probe:   |   0   |   0   |   1   |   1   |   0   |   0   | ...
 *  counter: |  low  | high  |  low  | high  |  low  | high  | ...
 *
 * Every image has been acquired with a very short gate (counting time) in order
 * to isolate photons from the single electron bunch. To increase the statistics
 * the individual images are summed in function of the probe and the pixel
 * counter into 4 separated images:
 *  - probe 0, counter low
 *  - probe 0, counter high
 *  - probe 1, counter low
 *  - probe 1, counter high
 *
 * Composite image
 * To facilitate visualization, four decoded and summed images are arranged
 * into a composite image of larger size (512x512 pixels). Each quadrant
 * on the arranged imaged correspond to the single accumulated image:
 *
 *  ----------------------------------
 *  |                                |
 *  |    probe 0    |    probe 0     |
 *  |  counter low  |  counter high  |
 *  |                                |
 *  ----------------------------------
 *  |                                |
 *  |    probe 1    |    probe 1     |
 *  |  counter low  |  counter high  |
 *  |                                |
 *  ----------------------------------
 *
 * Arguments:
 * - uint8_t **p_rawdata                 : table of pointer to the raw data,
 *                                         each pointer correspond to a single
 *                                         counter image (low or high)
 * - uint32_t *p_image                   : pointer to the decoded image buffer
 * - int       images_nb                 : number of the acquired images, low
 *                                         and high counters the number of
 *                                         images should be multiplication of 4
 * - int       virtual_pixels_correction : enable inter-chips gap correction
 */
void decode_image2_pumpprobe (uint8_t **p_rawdata,
                              uint32_t *p_image,
                              int       images_nb,
                              int       virtual_pixels_correction)
{
  int       img_n = 0;
  int       img_x0[] = {0, 128, 256, 384,   0, 128, 256, 384};
  int       img_y0[] = {0,   0,   0,   0, 256, 256, 256, 256};
  int       img_width = 2*DET_X;
  int       img_height = 2*DET_Y;
  int       virt_cols[] = {128, 385};
  int       virt_nb = 2;
  int       i = 0;
  uint32_t *p_tmpimg;
  int       img_size = 0;

  // temporary buffer for accumulated image
  if (virtual_pixels_correction == 1)
    {
      img_width = 2*DET_X_CORR;
      img_height = 2*DET_Y_CORR;
      img_x0[0] = 0;
      img_x0[1] = 129;
      img_x0[2] = 257;
      img_x0[3] = 386;
      img_x0[4] = 0;
      img_x0[5] = 129;
      img_x0[6] = 257;
      img_x0[7] = 386;
    }

  img_size = img_width*img_height;
  img_n = images_nb/4;

  // parallel region begin
  #pragma omp parallel private(p_tmpimg)
  {
    // allocate temporary image buffer for every thread
    p_tmpimg = calloc(img_size, sizeof(uint32_t));

    #pragma omp for //schedule(runtime)
    for (i=0; i<img_n; i++)
      {
        // decode 4 images into 1 composite image
        // image 1 (2 chips)
        decode_and_accumulate_pixcnt2 (*(p_rawdata + i*4),
                                        p_tmpimg,
                                        CHIP_0,
                                        img_x0[0],
                                        img_y0[0],
                                        img_width);
        decode_and_accumulate_pixcnt2 (*(p_rawdata + i*4),
                                        p_tmpimg,
                                        CHIP_1,
                                        img_x0[1],
                                        img_y0[1],
                                        img_width);

        // image 2 (2 chips)
        decode_and_accumulate_pixcnt2 (*(p_rawdata + i*4 + 1),
                                        p_tmpimg,
                                        CHIP_0,
                                        img_x0[2],
                                        img_y0[2],
                                        img_width);
        decode_and_accumulate_pixcnt2 (*(p_rawdata + i*4 + 1),
                                        p_tmpimg,
                                        CHIP_1,
                                        img_x0[3],
                                        img_y0[3],
                                        img_width);

        // image 3 (2 chips)
        decode_and_accumulate_pixcnt2 (*(p_rawdata + i*4 + 2),
                                        p_tmpimg,
                                        CHIP_0,
                                        img_x0[4],
                                        img_y0[4],
                                        img_width);
        decode_and_accumulate_pixcnt2 (*(p_rawdata + i*4 + 2),
                                        p_tmpimg,
                                        CHIP_1,
                                        img_x0[5],
                                        img_y0[5],
                                        img_width);

        // image 4 (2 chips)
        decode_and_accumulate_pixcnt2 (*(p_rawdata + i*4 + 3),
                                        p_tmpimg,
                                        CHIP_0,
                                        img_x0[6],
                                        img_y0[6],
                                        img_width);
        decode_and_accumulate_pixcnt2 (*(p_rawdata + i*4 + 3),
                                        p_tmpimg,
                                        CHIP_1,
                                        img_x0[7],
                                        img_y0[7],
                                        img_width);
      }

    // sum temporary images from every thread
    #pragma omp critical
    {
      sum_images32(p_image, p_tmpimg, img_size);
    }

    free(p_tmpimg);
  }

  // apply inter-chip gap correction
  if (virtual_pixels_correction == 1)
    {
      virtual_columns_correction_image32 ( p_image,
                                          &virt_cols[0],
                                           virt_nb,
                                           img_width,
                                           img_height);
    }
}
