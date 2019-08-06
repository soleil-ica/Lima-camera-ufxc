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

// order of rows and columns in the decoded image
extern const int col_order[];
extern const int row_order[];


/*
 * Function to decode raw data into pixel counters values acquired in 2-bits
 * mode.
 *
 * This function decodes single pixel counter data (low or high) from a single
 * chip (128 x 256 pixels). The function allows to decode data directly into a
 * single or composite images.
 * The single image is a single counter pixel matrix that correspond to a
 * complete detector, e.g. two chips detector has a size of 256 x 256 pixels.
 * The composite image is an arrangement of a several single images.
 *
 * Parameters:
 * - uint8_t *p_rawdata : pointer to the raw data buffer (single chip)
 * - uint8_t *p_image   : pointer to the decoded image data buffer
 * - int chip_index     : index of currently decoded chip (starts with 0)
 * - int img_x0         : X coordinate (first column) of the currently decoded
 *                        single image
 * - int img_y0         : Y coordinate (first row) of the currently decoded
 *                        single image
 * - int size_x         : line length of the final image (single or composite)
 */
void decode_chip_pixcnt2(uint8_t *p_rawdata, uint8_t *p_image, int chip_index,
                         int img_x0, int img_y0, int size_x){
  const int *row_idx, *col_idx;
  int row, sout_byte;
  uint8_t rawdata_byte;
  uint8_t *p_dest;

  // calculate pointer addresses offsets for raw and decoded data
  p_rawdata += chip_index*CHIP_RAW_SIZE_2BITS;
  p_image += img_y0*size_x + img_x0 + chip_index*CHIP_X;
  row_idx = &row_order[0];
  row = DET_Y;
  do{

    // decode second bit of the pixel counter
    col_idx = &col_order[0];
    sout_byte = SOUT_LEN_BYTES;
    do{
      rawdata_byte = *p_rawdata++;
      p_dest = p_image + (*row_idx)*size_x;

      *(p_dest + *col_idx++) |= ((rawdata_byte & 0x01) << 1);
      *(p_dest + *col_idx++) |= (((rawdata_byte & 0x02) >> 1) << 1);
      *(p_dest + *col_idx++) |= (((rawdata_byte & 0x04) >> 2) << 1);
      *(p_dest + *col_idx++) |= (((rawdata_byte & 0x08) >> 3) << 1);
      *(p_dest + *col_idx++) |= (((rawdata_byte & 0x10) >> 4) << 1);
      *(p_dest + *col_idx++) |= (((rawdata_byte & 0x20) >> 5) << 1);
      *(p_dest + *col_idx++) |= (((rawdata_byte & 0x40) >> 6) << 1);
      *(p_dest + *col_idx++) |= (((rawdata_byte & 0x80) >> 7) << 1);
    }while(--sout_byte);

    // decode the first bit of the pixel counter
    col_idx = &col_order[0];
    sout_byte = SOUT_LEN_BYTES;
    do{
      rawdata_byte = *p_rawdata++;
      p_dest = p_image + (*row_idx)*size_x;

      *(p_dest + *col_idx++) |= ((rawdata_byte & 0x01) << 0);
      *(p_dest + *col_idx++) |= (((rawdata_byte & 0x02) >> 1) << 0);
      *(p_dest + *col_idx++) |= (((rawdata_byte & 0x04) >> 2) << 0);
      *(p_dest + *col_idx++) |= (((rawdata_byte & 0x08) >> 3) << 0);
      *(p_dest + *col_idx++) |= (((rawdata_byte & 0x10) >> 4) << 0);
      *(p_dest + *col_idx++) |= (((rawdata_byte & 0x20) >> 5) << 0);
      *(p_dest + *col_idx++) |= (((rawdata_byte & 0x40) >> 6) << 0);
      *(p_dest + *col_idx++) |= (((rawdata_byte & 0x80) >> 7) << 0);
    }while(--sout_byte);

    row_idx++;
  }while(--row);
}


/*
 * Function to correct all pixels values in decoded image acquired in 2 bits
 * mode due to the specific detector counting mode (downward).
 *
 * Details in description of the correct_value_pixcnt2 function.
 *
 * Parameters:
 * - uint8_t *p_image : pointer to the decoded image buffer
 * - int image_size   : image size (number of pixels/values)
 */
void correct_value_image2(uint8_t *p_image, int image_size){
  do{
    //if (*p_image != 0){
    //  if(*p_image == 3) // single event - most probably, checked at first
    //    *p_image = 1;
    //  else if(*p_image == 1) // three events
    //    *p_image = 3;
    // no need to compare for two events because downward counter is already
    // equal to 2 (see function comment)
    //}
   *p_image = ~(*p_image-1) & PIXCNT_MASK_2BITS;
    p_image++;
  }while(--image_size);
}


/*
 * Function to accumulate 2-bits decoded images.
 *
 * Parameters:
 * - uint32_t *p_accumulated_image : resulting summed image
 * - uint8_t *p_image              : single 2-bits image
 * - int image_size                : image size (number of pixels/values)
 */
void accumulate_image2(uint32_t *p_accumulated_image, uint8_t *p_image,
                       int image_size){
  do{
    *p_accumulated_image = *p_accumulated_image + *p_image;
    p_accumulated_image++;
    p_image++;
  }while(--image_size);
}


/*
 * Function to correct values of the inter-chips gaps pixels on 32-bits
 * accumulated pixels.
 *
 * The inter-chips gap is the distance between two adjacent readout chips
 * that is covered by larger pixel sensor.
 * The inter-chips gap correction is based on redistribution of the total
 * count of two larger pixels into three pixels of the normal size.
 *
 * original image:  ... | pix 126 |    pix 127   |    pix 128   | pix 129 | ...
 * corrected image: ... | cor 126 | cor 127 | cor 128 | cor 129 | cor 130 | ...
 *
 * cor 126 = pix 126
 * cor 127 = 2/3 pix 127
 * cor 128 = 1/3 pix 127 + 1/3 pix 128
 * cor 129 = 2/3 pix 128
 * cor 130 = pix 129
 *gap_correction
 * Arguments:
 * - uint32_t *p_image    : pointer to the input decoded image
 * - uint16_t *p_corrected_image : pointer to the corrected image
 * - int *gaps_cols       : pointer to the table with numbers of the first
 *                          column for every inter-chips gap, e.g. column 127
 *                          for the gap between first and second chip, i.e.
 *                          columns 127-128
 * - int gaps_nb          : total number of the inter-chips gaps in the image
 * - int size_x           : horizontal size (line length) of the input image
 * - int size_y           : vertical size (column length) of the input image
 */
void correct_gaps_image2_acc(uint32_t *p_image, uint32_t *p_corrected_image,
                             int *gaps_cols, int gaps_nb, int size_x, int size_y){
  int row_cnt = 0;
  int col_cnt = 0;
  int gap_cnt = 0;

  for(row_cnt=0; row_cnt<size_y; row_cnt++){
    gap_cnt = 0; // reset current gap for every row
    for(col_cnt=0; col_cnt<size_x; col_cnt++){
      // first gap pixel
      if(col_cnt == *(gaps_cols+gap_cnt)){
        // first corrected pixel
        *p_corrected_image++ = (uint32_t) ((*p_image)*2/3);
        // middle corrected pixel
        *p_corrected_image++ = (uint32_t) ((*p_image)*1/3 + (*(p_image+1)*1/3));
        p_image++;
      }
      // third and last corrected pixel
      else if(col_cnt == *(gaps_cols+gap_cnt)+1){
        *p_corrected_image++ = (uint32_t) ((*p_image++)*2/3);
        if(gap_cnt < (gaps_nb-1))
          gap_cnt++;
      }
      else
        *p_corrected_image++ = *p_image++;
    } // col_cnt
  } // row_cnt
}


/*
 * Function to decode single counter 2-bits image.
 *
 * Inter-chip gap correction.
 * Because, the 2-bits images are acquired with a very short gate, therefore
 * by default there is no inter-chips gaps correction. For this correction an
 * image with higher statistics is required (e.g. on the sum of multiple single
 * frames).
 *
 * Arguments:
 * - uint8_t *p_rawdata : pointer to the raw data buffer
 * - uint8_t *p_image   : pointer to the decoded single counter image buffer
 */
void decode_image2_onecnt(uint8_t *p_rawdata, uint8_t *p_image){
  int img_x0 = 0;
  int img_y0 = 0;

  decode_chip_pixcnt2(p_rawdata, p_image, CHIP_0, img_x0, img_y0, DET_X);
  decode_chip_pixcnt2(p_rawdata, p_image, CHIP_1, img_x0, img_y0, DET_X);

  // correct pixel counters values
  correct_value_image2(p_image, DET_SIZE_PIX);
}


/*
 * Function to decode double counter 2-bits image.
 *
 * Inter-chip gap correction.
 * Because, the 2-bits images are acquired with a very short gate, therefore
 * by default there is no inter-chips gaps correction. For this correction an
 * image with higher statistics is required (e.g. on the sum of multiple single
 * frames).
 *
 * Arguments:
 * - uint8_t *p_rawdata_low  : pointer to the low pixel counter raw data buffer
 * - uint8_t *p_rawdata_high : pointer to the higher pixel counter raw data buffer
 * - uint8_t *p_image   : pointer to the decoded single counter image buffer
 */
void decode_image2_twocnt(uint8_t *p_rawdata_low, uint8_t *p_rawdata_high,
                          uint8_t *p_image){
  int img_x0[] = {0, 256};
  int img_y0[] = {0, 0};

  decode_chip_pixcnt2(p_rawdata_low, p_image, CHIP_0, img_x0[0], img_y0[0], 2*DET_X);
  decode_chip_pixcnt2(p_rawdata_low, p_image, CHIP_1, img_x0[0], img_y0[0], 2*DET_X);
  decode_chip_pixcnt2(p_rawdata_high, p_image, CHIP_0, img_x0[1], img_y0[1], 2*DET_X);
  decode_chip_pixcnt2(p_rawdata_high, p_image, CHIP_1, img_x0[1], img_y0[1], 2*DET_X);

  // correct pixel counters values
  correct_value_image2(p_image, 2*DET_SIZE_PIX);
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
 * Parmeters:
 * - uint8_t **p_rawdata : table of pointer to the raw data, each pointer
 *                         correspond to a single counter image (low or high)
 * - uint32_t *p_image   : pinter to the decoded image buffer
 * - int images_nb       : number of the acquired images, low and high counters
 *                         the number of images should be multiplication of 4
 * - int gap_correction  : enable inter-chips gap correction
 */
void decode_image2_pumpprobe(uint8_t **p_rawdata, uint32_t *p_image,
                             int images_nb, int gap_correction){
  uint8_t *p_tmpimg; // for decoded image
  uint32_t *p_tmpimg_acc; // for accumulated image
  int img_n = 0;
  int img_size = 4*DET_SIZE_PIX;
  int img_x0[] = {0, 256, 0, 256};
  int img_y0[] = {0, 0, 256, 256};
  int gaps_cols[] = {127, 383};
  int gaps_nb = 2;

  // temporary buffer for decoded image
  p_tmpimg = calloc(sizeof(uint8_t), img_size);

  // temporary buffer for accumulated image
  if(gap_correction==1)
    p_tmpimg_acc = calloc(sizeof(uint32_t), img_size);
  else
    p_tmpimg_acc = p_image;


  img_n = images_nb/4;

  do{
    // decode 4 images into 1 composite image
    decode_chip_pixcnt2(*p_rawdata, p_tmpimg, CHIP_0, img_x0[0], img_y0[0], 2*DET_X);
    decode_chip_pixcnt2(*p_rawdata++, p_tmpimg, CHIP_1, img_x0[0], img_y0[0], 2*DET_X);

    decode_chip_pixcnt2(*p_rawdata, p_tmpimg, CHIP_0, img_x0[1], img_y0[1], 2*DET_X);
    decode_chip_pixcnt2(*p_rawdata++, p_tmpimg, CHIP_1, img_x0[1], img_y0[1], 2*DET_X);

    decode_chip_pixcnt2(*p_rawdata, p_tmpimg, CHIP_0, img_x0[2], img_y0[2], 2*DET_X);
    decode_chip_pixcnt2(*p_rawdata++, p_tmpimg, CHIP_1, img_x0[2], img_y0[2], 2*DET_X);

    decode_chip_pixcnt2(*p_rawdata, p_tmpimg, CHIP_0, img_x0[3], img_y0[3], 2*DET_X);
    decode_chip_pixcnt2(*p_rawdata++, p_tmpimg, CHIP_1, img_x0[3], img_y0[3], 2*DET_X);

    // correct and add composite image
    correct_value_image2(p_tmpimg, img_size);
    accumulate_image2(p_tmpimg_acc, p_tmpimg, img_size);
    // reset memory before decoding new series of four images
    memset(p_tmpimg, 0, img_size);

  }while(--img_n);

  // apply inter-chip gap correction
  if(gap_correction==1)
    correct_gaps_image2_acc(p_tmpimg_acc, p_image, &gaps_cols[0], gaps_nb,
                            2*DET_X, 2*DET_Y);
}
