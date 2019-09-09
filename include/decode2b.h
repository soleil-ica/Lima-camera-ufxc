#ifndef DECODE2B_H_INCLUDED
#define DECODE2B_H_INCLUDED

#ifdef __cplusplus
extern "C"
{
#endif

void  decode_pixcnt2                 (uint8_t   *p_rawdata,
                                      uint8_t   *p_image,
                                      int        chip_index,
                                      int        img_x0,
                                      int        img_y0,
                                      int        img_width);

void  decode_and_accumulate_pixcnt2  (uint8_t   *p_rawdata,
                                      uint32_t  *p_image,
                                      int        chip_index,
                                      int        img_x0,
                                      int        img_y0,
                                      int        img_width);

void  decode_image2_onecnt           (uint8_t   *p_rawdata,
                                      uint8_t   *p_image);

void  decode_image2_twocnt           (uint8_t   *p_rawdata_low,
                                      uint8_t   *p_rawdata_high,
                                      uint8_t   *p_image);

void  decode_image2_pumpprobe        (uint8_t  **p_rawdata,
                                      uint32_t  *p_image,
                                      int        images_nb,
                                      int        virtual_pixels_correction);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // DECODE2B_H_INCLUDED
