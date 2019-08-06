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

// order of rows and columns in the decoded image
extern const int col_order[];
extern const int row_order[];


/*
 * Function to decode raw data into pixel counters values acquired in 14-bits
 * mode.
 *
 * This function decodes single pixel counter data (low or high) from a single
 * chip (128 x 256 pixels). The function allows to decode data directly into a
 * single or composite images.
 * The single image is a single counter pixel matrix that correspond to a
 * complete detector, e.g. two chips detector has a size of 256 x 256 pixels.
 * two chips detector correspond to a size of 256 x 256 pixels.
 * The composite image is an arrangement of a several single images.
 *
 * Parameters:
 * - uint8_t *p_rawdata : pointer to the raw data buffer (single chip)
 * - uint16_t *p_image  : pointer to the decoded image data buffer
 * - int chip_index     : index of currently decoded chip (starts with 0)
 * - int img_x0         : X coordinate (first column) of the currently decoded
 *                        single image
 * - int img_y0         : Y coordinate (first row) of the currently decoded
 *                        single image
 * - int size_x         : line length of the final image (single or composite)
 */
void decode_chip_pixcnt14(uint8_t *p_rawdata, uint16_t *p_image, int chip_index,
                          int img_x0, int img_y0, int size_x){
  const int *row_idx, *col_idx;
  int row, pixcnt_bit, sout_byte;
  uint8_t rawdata_byte;
  uint16_t *p_dest;

  // calculate pointer addresses offsets for raw and decoded data
  p_rawdata += chip_index*CHIP_RAW_SIZE_14BITS;
  p_image += img_y0*size_x + img_x0 + chip_index*CHIP_X;

  row_idx = &row_order[0];
  row = DET_Y;
  do{
    // iterate through bits within pixel counter value (MSB first)
    for(pixcnt_bit=PIXCNT_14_MSB; pixcnt_bit>=PIXCNT_14_LSB; pixcnt_bit--){

      // get a complete readout shift register (one row, 128 bits)
      col_idx = &col_order[0];
      sout_byte = SOUT_LEN_BYTES;

      do{
        rawdata_byte = *p_rawdata++;
        p_dest = p_image + (*row_idx)*size_x;

        *(p_dest + *col_idx++) |= ((rawdata_byte & 0x01) << pixcnt_bit);
        *(p_dest + *col_idx++) |= (((rawdata_byte & 0x02) >> 1) << pixcnt_bit);
        *(p_dest + *col_idx++) |= (((rawdata_byte & 0x04) >> 2) << pixcnt_bit);
        *(p_dest + *col_idx++) |= (((rawdata_byte & 0x08) >> 3) << pixcnt_bit);
        *(p_dest + *col_idx++) |= (((rawdata_byte & 0x10) >> 4) << pixcnt_bit);
        *(p_dest + *col_idx++) |= (((rawdata_byte & 0x20) >> 5) << pixcnt_bit);
        *(p_dest + *col_idx++) |= (((rawdata_byte & 0x40) >> 6) << pixcnt_bit);
        *(p_dest + *col_idx++) |= (((rawdata_byte & 0x80) >> 7) << pixcnt_bit);
      }while(--sout_byte);

    }; // pixcnt_bit
    row_idx++;
  }while(--row);
}


/*
 * Function to correct all pixels values in decoded image acquired in 14 bits
 * mode due to the specific detector counting mode (downward).
 *
 * Details in description of the correct_value_pixcnt14 function.
 *
 * Parameters:
 * - uint16_t *p_image : pointer to the decoded image buffer
 * - int image_size    : image size (number of pixels/values)
 */
void correct_value_image14(uint16_t *p_image, int image_size){
  do{
    *p_image = ~(*p_image-1) & PIXCNT_MASK_14BITS;
    p_image++;
  }while(--image_size);
}


/*
 * Function to correct values of the inter-chips gaps pixels on a 4-bits image.
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
 *
 * Arguments:
 * - uint16_t *p_image    : pointer to the input decoded image
 * - uint16_t *p_corimage : pointer to the corrected image
 * - int *gaps_cols       : pointer to the table with numbers of the first
 *                          column for every inter-chips gap, e.g. column 127
 *                          for the gap between first and second chip, i.e.
 *                          columns 127-128
 * - int gaps_nb          : total number of the inter-chips gaps in the image
 * - int size_x           : horizontal size (line length) of the input image
 * - int size_y           : vertical size (column length) of the input image
 */
void correct_gaps_image14(uint16_t *p_image, uint16_t *p_corrected_image,
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
        *p_corrected_image++ = (uint16_t) ((*p_image)*2/3);
        // middle corrected pixel
        *p_corrected_image++ = (uint16_t) ((*p_image)*1/3 + (*(p_image+1)*1/3));
        p_image++;
      }
      // third and last corrected pixel
      else if(col_cnt == *(gaps_cols+gap_cnt)+1){
        *p_corrected_image++ = (uint16_t) ((*p_image++)*2/3);
        if(gap_cnt < (gaps_nb-1))
          gap_cnt++;
      }
      else
        *p_corrected_image++ = *p_image++;
      } // col_cnt
  } // row_cnt
}

/*
 * Function to decode single counter 14-bits image.
 *
 * Arguments:
 * - uint8_t *p_rawdata : pointer to the raw data buffer
 * - uint16_t *p_image  : pointer to the decoded single counter image buffer
 * - int gap_correction : enable inter-chips gap correction
 */
void decode_image14_onecnt(uint8_t *p_rawdata, uint16_t *p_image,
                           int gap_correction){
  uint16_t *p_tmpimg;
  int img_x0 = 0;
  int img_y0 = 0;
  int gaps_cols[] = {127};
  int gaps_nb = 1;

  // initialize the temporary buffer
  if(gap_correction==1)
    p_tmpimg = calloc(sizeof(uint16_t), DET_SIZE_PIX);
  else
    p_tmpimg = p_image;

  decode_chip_pixcnt14(p_rawdata, p_tmpimg, CHIP_0, img_x0, img_y0, DET_X);
  decode_chip_pixcnt14(p_rawdata, p_tmpimg, CHIP_1, img_x0, img_y0, DET_X);

  // correct pixel counters values
  correct_value_image14(p_tmpimg, DET_SIZE_PIX);

  // apply inter-chip gap correction
  if(gap_correction==1)
    correct_gaps_image14(p_tmpimg, p_image, &gaps_cols[0], gaps_nb, DET_X, DET_Y);
}


/*
 * Function to decode double counter 14-bits image.
 *
 * Arguments:
 * - uint8_t *p_rawdata_low  : pointer to the low pixel counter raw data buffer
 * - uint8_t *p_rawdata_high : pointer to the higher pixel counter raw data buffer
 * - uint16_t *p_image       : pointer to the decoded double counter image buffer.
 * - int geomcorr_enabled    : enable inter-chips gap correction
 */
void decode_image14_twocnt(uint8_t *p_rawdata_low, uint8_t *p_rawdata_high,
                           uint16_t *p_image, int gap_correction){
  uint16_t *p_tmpimg;
  int img_x0[] = {0, 256};
  int img_y0[] = {0, 0};
  int gaps_cols[] = {127, 383};
  int gaps_nb = 2;

  // initialize the temporary buffer
  if(gap_correction==1)
    p_tmpimg = calloc(sizeof(uint16_t), 2*DET_SIZE_PIX);
  else
    p_tmpimg = p_image;

  decode_chip_pixcnt14(p_rawdata_low, p_tmpimg, CHIP_0, img_x0[0], img_y0[0], 2*DET_X);
  decode_chip_pixcnt14(p_rawdata_low, p_tmpimg, CHIP_1, img_x0[0], img_y0[0], 2*DET_X);
  decode_chip_pixcnt14(p_rawdata_high, p_tmpimg, CHIP_0, img_x0[1], img_y0[1], 2*DET_X);
  decode_chip_pixcnt14(p_rawdata_high, p_tmpimg, CHIP_1, img_x0[1], img_y0[1], 2*DET_X);

  // correct pixel counters values
  correct_value_image14(p_tmpimg, 2*DET_SIZE_PIX);

  // apply inter-chip gap correction
  if(gap_correction==1)
    correct_gaps_image14(p_tmpimg, p_image, &gaps_cols[0], gaps_nb, 2*DET_X, DET_Y);
}
