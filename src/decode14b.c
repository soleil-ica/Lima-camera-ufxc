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

#include "../include/constants.h"

extern const int col_order[];
extern const int row_order[];


/*  Function to update the 14 bits pixel counter value in function of the raw
 *  data bit:
 *
 *  Args:
 *  - uint16_t *p_pixcnt14: pointer to the correct 14 bits pixel counter value
 *  - uint8_t rawbit      : isolated value of the current raw bit: i.e. 0x01,
 *                          0x02, 0x04, ..., 0x80)
 *  - int bit_pos         : position (number) of the current bit within the
 *                          pixel counter value
 */
void update_bit_pixcnt14(uint16_t *p_pixcnt14, uint8_t rawbit, int bit_pos){
  // the pixel counter value is changed only if the raw bit is not 0. The
  // counter pixel buffer has been initialized with 0.
  if (rawbit != 0)
    *p_pixcnt14 = *p_pixcnt14 | (1<<bit_pos);
//  else
//    *p_pixcnt14 = *p_pixcnt14 & (0<<bit_pos);
}


/*  Function to correct pixel counter value acquired in 14 bits mode due to
 *  the specific detector counting mode (downward):
 *   events = 0     => counter = 0
 *   events = 1     => counter = 16383 (0x3FFF)
 *   events = 2     => counter = 16382 (0x3FFE)
 *   events = 16383 => counter = 1 (0x0001)
 *
 *  Args:
 *  - uint16_t *p_image : pointer to the pixel counter value
 */
void correct_pixcnt14(uint16_t *p_image){
  if (*p_image != 0)
    *p_image = ~(*p_image-1) & PIXCNT_MASK_14BITS;
}


/*  Function to correct all image pixels values acquired in 14 bits mode
 *  mode due to the specific detector counting mode (downward).
 *
 *  Args:
 *  - uint16_t *p_image : pointer to the decoded image buffer
 */
void correct_image14(uint16_t *p_image, int imgsize){
  int i = 0;
  for(i = 0; i<imgsize; i++)
    correct_pixcnt14(p_image+i);
}


/*  Function to decode single image raw data acquired in 14 bits mode. The
 *  function decodes single pixel counter (low or high) image. Memory allocated
 *  for the decoded data should correspond to the size of the single image,
 *  i.e. DET_X * DET_Y
 *
 *  Args:
 *  - uint8_t *p_rawdata  : pointer to the raw data buffer
 *  - uint16_t *p_image   : pointer to the decoded single counter image buffer
 *  - int chip_index      : index of currently decoded chip (starting from 0),
 *                          it is used to calculate correct offsets for raw
 *                          and decoded images
 */
void decode_onechip_pixcnt14(uint8_t *p_rawdata, uint16_t *p_image,
                             int chip_index){
  /*  const in pointers declaration is only to eliminate warning when
   *  trying to assign pointer to constant (pointer can change constant value)
   */

  // Pointers to correct pixel row and column taking into account specific
  // readout order. The *row_idx and *col_idx points to the constant LUTs
  const int *row_idx, *col_idx;
  int row, pixcnt_bit, sout_byte, sout_bit; // loops indexes
  int rawdata_cnt = 0;
  int pixcnt = 0;
  uint8_t rawdata_bit = 0;

  // calculate pointer addresses offsets for raw and decoded data
  int rawdata_offset = chip_index*CHIP_RAW_SIZE_14BITS;
  int chipdata_offset = chip_index*CHIP_X;

  // first half number of Bytes is for the first chip
  row_idx = &row_order[0];
  for(row=0; row<DET_Y; row++){
    // iterate through bits within pixel counter value (MSB first 13 to 0)
    for(pixcnt_bit=13; pixcnt_bit>=0; pixcnt_bit--){
      // 16 Bytes to read a complete readout shift register
      // (one bit for every pixel in a row)
      col_idx = &col_order[0];
      for(sout_byte=0; sout_byte<SOUT_LEN_BYTES; sout_byte++){
        for(sout_bit=0; sout_bit<8; sout_bit++){
          // isolate single bit in the current rawdata Byte
          rawdata_bit = *(p_rawdata+rawdata_cnt+rawdata_offset) & (1<<sout_bit);
          // get correct pixel counter position
          pixcnt = *col_idx           // column index
                   + (*row_idx)*DET_Y // row index
                   + chipdata_offset; // chip index
          // update destination counter value
          update_bit_pixcnt14(p_image+pixcnt, rawdata_bit, pixcnt_bit);
          col_idx++;
        } // sout_bit
        rawdata_cnt++;
      } // sout_byte
    } // pixcnt_bit
    row_idx++;
  } // row
}


/*  Function to decode 14 bits image from a single pixel counter. Memory
 *  allocated for the decoded data should correspond to the size of the single
 *  image, i.e. DET_X * DET_Y.
 *
 *  Args:
 *  - uint8_t *p_rawdata  : pointer to the raw data buffer
 *  - uint16_t *p_image   : pointer to the decoded image buffer
 */
void decode_image14(uint8_t *p_rawdata, uint16_t *p_image){

  // decode first chip data
  decode_onechip_pixcnt14(p_rawdata, p_image, CHIP_0);
  // decode second chip data
  decode_onechip_pixcnt14(p_rawdata, p_image, CHIP_1);
  // correct counter values due to downward pixel counting
  correct_image14(p_image, DET_SIZE_PIX);
}


/*  Function to decode single image raw data acquired in 14 bits mode. The
 *  function decodes single pixel counter image but takes care of the current
 *  pixel counter index (low or high). Memory allocated for the decoded data
 *  should correspond to the size of the double detector image,
 *  i.e. (2*DET_X) * DET_Y
 *
 *  Args:
 *  - uint8_t *p_rawdata    : pointer to the raw data buffer
 *  - uint16_t *p_imgdata2  : pointer to the decoded single counter image buffer
 *  - int chip_index        : index of currently decoded chip (starting from 0),
 *                            it is used to calculate correct offsets for raw
 *                            and decoded images
 *  - int counter_index     : index of the currently decoded pixel counter
 */
void decode_onechip_pixcnt14_imgsel(uint8_t *p_rawdata, uint16_t *p_image,
                                    int chip_index, int img_rowlen,
                                    int img_x0, int img_y0){
  /*  const in pointers declaration is only to eliminate warning when
   *  trying to assign pointer to constant (pointer can change constant value)
   */

  // Pointers to correct pixel row and column taking into account specific
  // readout order. The *row_idx and *col_idx points to the constant LUTs
  const int *row_idx, *col_idx;
  int row, pixcnt_bit, sout_byte, sout_bit; // loops indexes
  int rawdata_cnt = 0;
  int pixcnt = 0;
  uint8_t rawdata_bit = 0;

  // calculate pointer addresses offsets for raw and decoded data
  int rawdata_offset = chip_index*CHIP_RAW_SIZE_14BITS;
  int image_offset = img_y0*img_rowlen + img_x0 + chip_index*CHIP_X;

  // first half number of Bytes is for the first chip
  row_idx = &row_order[0];
  for(row=0; row<DET_Y; row++){
    // iterate through bits within pixel counter value (MSB first 13 to 0)
    for(pixcnt_bit=13; pixcnt_bit>=0; pixcnt_bit--){
      // 16 Bytes to read a complete readout shift register
      // (one bit for every pixel in a row)
      col_idx = &col_order[0];
      for(sout_byte=0; sout_byte<SOUT_LEN_BYTES; sout_byte++){
        for(sout_bit=0; sout_bit<8; sout_bit++){
          // isolate single bit in the current rawdata Byte
          rawdata_bit = *(p_rawdata+rawdata_cnt+rawdata_offset) & (1<<sout_bit);
          // get correct pixel counter position
          pixcnt = image_offset + (*row_idx)*img_rowlen + *col_idx;

          // update destination counter value
          update_bit_pixcnt14(p_image+pixcnt, rawdata_bit, pixcnt_bit);
          col_idx++;
        } // sout_bit
        rawdata_cnt++;
      } // sout_byte
    } // pixcnt_bit
    row_idx++;
  } // row
}


/*  Function to decode 14 bits image from two pixel counters. Memory
 *  allocated for the decoded data should correspond to the size of the double
 *  detector image, i.e. (2*DET_X) * DET_Y
 *
 *  Args:
 *  - uint8_t *p_rawdata_low  : pointer to the lower counter raw data buffer
 *  - uint8_t *p_rawdata_high : pointer to the higher counter raw data buffer
 *  - uint16_t *p_imgdata     : pointer to the decoded image buffer
 */
void decode_image14_twocnts(uint8_t *p_rawdata_low, uint8_t *p_rawdata_high,
                            uint16_t *p_imgdata){

  int row_len = 2*DET_X;

  // chip 1, low counter
  decode_onechip_pixcnt14_imgsel(p_rawdata_low, p_imgdata, CHIP_0,
                                 row_len, 0, 0);
  // chip 2, low counter
  decode_onechip_pixcnt14_imgsel(p_rawdata_low, p_imgdata, CHIP_1,
                                 row_len, 0, 0);
  // chip 1, high counter
  decode_onechip_pixcnt14_imgsel(p_rawdata_high, p_imgdata, CHIP_0,
                                 row_len, 256, 0);
  // chip 2, high counter
  decode_onechip_pixcnt14_imgsel(p_rawdata_high, p_imgdata, CHIP_1,
                                 row_len, 256, 0);

  // correct counter values due to downward pixel counting
  correct_image14(p_imgdata, 2*DET_SIZE_PIX);
}


/*  Function to correct the inter-chips gaps.
 *  The inter-chips gap is the distance between two adjacent readout chips
 *  that is covered by larger pixel sensor.
 *  The inter-chips gap correction is based on redistribution of the total
 *  count of two pixels into three pixels. The middle pixel is so called
 *  the virtual pixel.
 *  Original image:  ... | pix 126 |    pix 127   |    pix 128   | pix 129 | ...
 *  Corrected image: ... | cor 126 | cor 127 | virtual | cor 128 | cor 129 | ...
 *    cor 126 = pix 126
 *    cor 127 = 2/3 pix 127
 *    virtual = 1/3 pix 127 + 1/3 pix 128
 *    cor 128 = 2/3 pix 128
 *    cor 129 = pix 129
 *
 *  Args:
 *  - uint16_t *p_image       : pointer to the horizontal/raw image
 *  - uint16_t *p_corr_image  : pointer to the corrected image
 *  - int imgsize_x           : horizontal (row) size of the input/raw image
 *  - int imgsize_y           : vertical (col) size of the input/raw image
 */
//void geomcorr_image14(uint16_t *p_image, uint16_t *p_corr_image,
//                      int imgsize_x, int imgsize_y){
void geomcorr_image14(int imgsize_x, int imgsize_y){
  int i = 0;
  int j = 0;
  int virt_col = CHIP_X;

  printf("indexes:\n");
  for(i=0; i<imgsize_x; i++){
    if(i==virt_col){
      printf("%d\n", i);
    }
    else if(i==virt_col+1){
      printf("%d\n", i);
      virt_col += DET_X;
    }
  }

}
