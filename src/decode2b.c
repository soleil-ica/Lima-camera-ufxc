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

#include "../include/constants.h"

extern const int col_order[];
extern const int row_order[];


/*  Function to update the 2 bits pixel counter value in function of the raw
 *  data bit:
 *
 *  Args:
 *  - uint8_t *p_pixcnt2  : pointer to the correct 2 bits pixel counter value
 *  - uint8_t rawbit      : isolated value of the current raw bit: i.e. 0x01,
 *                          0x02, 0x04, ..., 0x80)
 *  - int bit_pos         : position (number) of the current bit within the
 *                          pixel counter value
 */
void update_bit_pixcnt2(uint8_t *p_pixcnt2, uint8_t rawbit, int bit_pos){
  // the pixel counter value is changed only if the raw bit is not 0. The
  // counter pixel buffer has been initialized with 0.
  if (rawbit != 0)
    *p_pixcnt2 = *p_pixcnt2 | (1<<bit_pos);
//  else
//    *p_pixcnt2 = *p_pixcnt2 & (0<<bit_pos);
}


/*  Function to correct pixel counter value acquired in 2 bits mode due to
 *  the specific detector counting mode (downward):
 *   events = 0     => counter = 0
 *   events = 1     => counter = 3 (0x3FFF)
 *   events = 2     => counter = 2 (0x3FFE)
 *   events = 3     => counter = 1 (0x0001)
 *
 *  Args:
 *  - uint16_t *p_pixcnt2 : pointer to the pixel counter value
 */
void correct_pixcnt2(uint8_t *p_pixcnt2){
  //  most pixels will be equal 0 therefore it checked at the beginning
  if (*p_pixcnt2 != 0){
    // single event
    if(*p_pixcnt2 == 3)
      *p_pixcnt2 = 1;
    // three events
    else if(*p_pixcnt2 == 1)
      *p_pixcnt2 = 3;
    // no need to compare for two events because downward counter is already
    // equal to 2 (see function comment)
  }
}


/*  Function to correct all image pixels values acquired in 2 bits mode
 *  mode due to the specific detector counting mode (downward).
 *
 *  Args:
 *  - uint8_t *p_image : pointer to the decoded image buffer
 */
void correct_image2(uint8_t *p_image, int imgsize){
  int i = 0;
  for(i = 0; i<imgsize; i++)
    correct_pixcnt2(p_image+i);
}


/*  Function to decode single image raw data acquired in 2 bits mode. The
 *  function decodes single pixel counter (low or high) image. Memory allocated
 *  for the decoded data should correspond to the size of the single image,
 *  i.e. DET_X * DET_Y
 *
 *  Args:
 *  - uint8_t *p_rawdata  : pointer to the raw data buffer
 *  - uint8_t *p_image    : pointer to the decoded single counter image buffer
 *  - int chip_index      : index of currently decoded chip (starting from 0),
 *                          it is used to calculate correct offsets for raw
 *                          and decoded images
 */
void decode_onechip_pixcnt2(uint8_t *p_rawdata, uint8_t *p_image,
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
  int rawdata_offset = chip_index*CHIP_RAW_SIZE_2BITS;
  int chipdata_offset = chip_index*CHIP_X;

  // first half number of Bytes is for the first chip
  row_idx = &row_order[0];
  for(row=0; row<DET_Y; row++){
    // iterate through bits within pixel counter value (MSB first 1 to 0)
    for(pixcnt_bit=1; pixcnt_bit>=0; pixcnt_bit--){
      // 16 Bytes to read a complete readout shift register
      // (one bit for every pixel in a row)
      col_idx = &col_order[0];
      for(sout_byte=0; sout_byte<SOUT_LEN_BYTES; sout_byte++){
        for(sout_bit=0; sout_bit<8; sout_bit++){
          // isolate single bit in the current rawdata Byte
          rawdata_bit = *(p_rawdata+rawdata_cnt+rawdata_offset) & (1<<sout_bit);
          // get correct pixel counter position
          pixcnt = *col_idx           // column index
                   + (*row_idx)*DET_X // row index
                   + chipdata_offset; // chip index
          // update destination counter value
          update_bit_pixcnt2(p_image+pixcnt, rawdata_bit, pixcnt_bit);
          col_idx++;
        } // sout_bit
        rawdata_cnt++;
      } // sout_byte
    } // pixcnt_bit
    row_idx++;
  } // row
}


/*  Function to decode 2 bits image from a single pixel counter. Memory
 *  allocated for the decoded data should correspond to the size of the single
 *  image, i.e. DET_X * DET_Y.
 *
 *  Args:
 *  - uint8_t *p_rawdata  : pointer to the raw data buffer
 *  - uint8_t *p_image    : pointer to the decoded image buffer
 */
void decode_image2(uint8_t *p_rawdata, uint8_t *p_image){

  // decode first chip data
  decode_onechip_pixcnt2(p_rawdata, p_image, CHIP_0);
  // decode second chip data
  decode_onechip_pixcnt2(p_rawdata, p_image, CHIP_1);
  // correct counter values due to downward pixel counting
  correct_image2(p_image, DET_SIZE_PIX);
}


/*
 */
void decode_onechip_pixcnt2_imgsel(uint8_t *p_rawdata, uint8_t *p_image,
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
  int rawdata_offset = chip_index*CHIP_RAW_SIZE_2BITS;
  int image_offset = img_y0*img_rowlen + img_x0 + chip_index*CHIP_X;

  // first half number of Bytes is for the first chip
  row_idx = &row_order[0];
  for(row=0; row<DET_Y; row++){
    // iterate through bits within pixel counter value (MSB first 1 to 0)
    for(pixcnt_bit=1; pixcnt_bit>=0; pixcnt_bit--){
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
          update_bit_pixcnt2(p_image+pixcnt, rawdata_bit, pixcnt_bit);
          col_idx++;
        } // sout_bit
        rawdata_cnt++;
      } // sout_byte
    } // pixcnt_bit
    row_idx++;
  } // row
}


/*  Function to accumulate 2 bits decoded images
 *  Args:
 *  - uint32_t *p_sum_image : destination images (summed)
 *  - uint8_t *p_image      : single 2-bits frame that will be added to the
 *                            resulting image
 *  - int nb_values         : number of values in the image, i.e. image size
 */
void sum_image2_pumpprobe(uint32_t *p_sum_image, uint8_t *p_image,
                          int nb_values){
  int i = 0;

  for(i=0; i<nb_values; i++){
    *(p_sum_image+i) = *(p_sum_image+i) + *(p_image+i);
  }
}


/*
 *  Function to decode 2 bits raw data following the pump and probe-probe
 *  acquisition.
 *
 *  The series of single counters raw data (low and high) of size 256x256
 *  pixels are decoded, summed and arranged into a single image of size 512x512
 *  pixels. Optionally the final image might be as well geometrically corrected
 *  for the inter-chip gap. The image after geometrical correction will have
 *  size of 512x514 pixels.
 *
 *  -------
 *  Summing
 *  The series of the images acquired during the acquisition has the specific
 *  order:
 *  image:  |  IMG 0  |  IMG 1  |  IMG 2  |  IMG 3  |  IMG 4  |  IMG 5  | ...
 *  probe:  | probe 0 | probe 0 | probe 1 | probe 1 | probe 0 | probe 0 | ...
 *  counter:|  low    |  high   |  low    |  high   |  low    |  high   | ...
 *
 *  Each image concerns the acquisition from the single electron bunch. In order
 *  to increase the statistics the images are summed in function of the probe
 *  and the pixel counter into 4 separated images of size 256x256 pixels:
 *   - probe 0, counter low
 *   - probe 0, counter high
 *   - probe 1, counter low
 *   - probe 1, counter high
 *
 *  ------------------
 *  Images arrangement
 *  in order to facilitate visualization, four decoded and summed images are
 *  arranged into a single image of larger size (512x512 pixels). Each quadrant
 *  on the arranged imaged correspond to the differed summed images:
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
 *  Args:
 *  - uint8_t **p_rawdata : table of pointer to the raw data, each pointer
 *                          is for the individual image (low or high counter)
 *  - uint32_t *p_image   : pinter to the decoded image (512x512 pixels)
 *  - int nb_images       : number of the acquired images, low and high counters
 *                          the number of images should be multiplication of 4
 *  - int summing_enable  : enable summing of the images
 *  - int geomcorr_enable : enable geometrical corrections
 */
void decode_image2_pumpprobe(uint8_t **p_rawdata, uint32_t *p_image,
                             int nb_images, int geomcorr_enable){
  // the img_index variable (counter) determines the type of the image that is
  // being decoded:
  // 0 : probe 1, counter low
  // 1 : probe 1, counter high
  // 2 : probe 2, counter low
  // 3 : probe 2, counter high
  int img_index = 0;
  int img_size = 4*DET_SIZE_PIX;
  int row_len = 2*DET_X;
  uint8_t *p_tmpimg;
  int i=0;
  int img_x0[] = {0, 256, 0, 256};
  int img_y0[] = {0, 0, 256, 256};

  // allocate temporary image for four decoded images: probe 0 and probe 1
  p_tmpimg = malloc(img_size*sizeof(uint8_t));
  memset(p_tmpimg, 0, img_size);

  for(i=0; i<nb_images; i++){

    // decode images chip by chip
    decode_onechip_pixcnt2_imgsel(*(p_rawdata+i), p_tmpimg, CHIP_0, row_len,
                                  img_x0[img_index], img_y0[img_index]);
    decode_onechip_pixcnt2_imgsel(*(p_rawdata+i), p_tmpimg, CHIP_1, row_len,
                                  img_x0[img_index], img_y0[img_index]);
    // increment image index
    if(img_index==3){
      // reset image index - all four images have been decoded
      img_index=0;
      // correct pixel counters
      correct_image2(p_tmpimg, img_size);
      // accumulate images
      sum_image2_pumpprobe(p_image, p_tmpimg, img_size);
      // reset memory before decoding new series of four images
      memset(p_tmpimg, 0, img_size);
    }
    else{
      img_index++;
    }
  }
}
