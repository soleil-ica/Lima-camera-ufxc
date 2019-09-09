/******************************************************************************
 * File: decode14b.c
 * Author: Arkadiusz Dawiec (arkadiusz.dawiec@synchrotron-soleil.fr)
 * Date: 23/08/2019
 *
 * Set of functions to correct decoded images.
 * Available corrections:
 *  - vertical inter-chips gap (column) on 16b and 32b images
 *  - sum of the images
 *
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

/*
 * Functions to correct values of the inter-chips gaps pixels.
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
 */

/*
 * Arguments:
 * - uint16_t *p_image            : pointer to the input decoded image
 * - int      *p_virtual_columns  : pointer to the table with numbers of the
 *                                  virtual columns
 * - int       virtual_columns_nb : total number of the inter-chips gaps
 * - int       img_width          : width of the final image
 * - int       img_height         : height of the final image
 */
void virtual_columns_correction_image16 (uint16_t *p_image,
                                         int      *p_virtual_columns,
                                         int       virtual_columns_nb,
                                         int       img_width,
                                         int       img_height)
{
  int  row = 0;
  int  virt_nb = 0;
  int *p_virt;

  row = img_height;
  do
    {
      p_virt = p_virtual_columns;
      virt_nb = virtual_columns_nb;
      do
        {
          // correct virtual pixel value
          *(p_image + *p_virt)     = (uint16_t) ((*(p_image + *p_virt - 1)
                                                + *(p_image + *p_virt + 1))
                                                  * 1/3);
          // correct left pixel
          *(p_image + *p_virt - 1) = (uint16_t) (*(p_image + *p_virt - 1) * 2/3);
          // correct right pixel
          *(p_image + *p_virt + 1) = (uint16_t) (*(p_image + *p_virt + 1) * 2/3);
          p_virt++;
        }
      while (--virt_nb);
      p_image += img_width;
    }
  while (--row);
}


/*
 * Arguments:
 * - uint32_t *p_image            : pointer to the input decoded image
 * - int      *p_virtual_columns  : pointer to the table with numbers of the
 *                                  virtual columns
 * - int       virtual_columns_nb : total number of the inter-chips gaps
 * - int       img_width          : width of the final image
 * - int       img_height         : height of the final image
 */
void virtual_columns_correction_image32 (uint32_t *p_image,
                                         int      *p_virtual_columns,
                                         int       virtual_columns_nb,
                                         int       img_width,
                                         int       img_height)
{
  int  row = 0;
  int  virt_nb = 0;
  int *p_virt;

  row = img_height;
  do
    {
      p_virt = p_virtual_columns;
      virt_nb = virtual_columns_nb;
      do
        {
          // correct virtual pixel value
          *(p_image + *p_virt)     = (uint32_t) ((*(p_image + *p_virt - 1)
                                                + *(p_image + *p_virt + 1))
                                                  * 1/3);
          // correct left pixel
          *(p_image + *p_virt - 1) = (uint32_t) (*(p_image + *p_virt - 1) * 2/3);
          // correct right pixel
          *(p_image + *p_virt + 1) = (uint32_t) (*(p_image + *p_virt + 1) * 2/3);
          p_virt++;
        }
      while (--virt_nb);
      p_image += img_width;
    }
  while (--row);
}


/*
 * Function to sum two 32-bits images.
 *
 * Arguments:
 * - uint32_t *p_image_sum  : pointer to the summed image
 * - uint32_t *p_image      : pinter to the image that will be added to the
 *                            summed image
 * - int       image_size   : image size (number of elements)
 */
void sum_images32 (uint32_t *p_image_sum,
                   uint32_t *p_image,
                   int       image_size)
{
  do
    {
      *p_image_sum = *p_image_sum + *p_image++;
      p_image_sum++;
    }
  while (--image_size);
}
