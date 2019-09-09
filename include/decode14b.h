#ifndef DECODE14B_H_INCLUDED
#define DECODE14B_H_INCLUDED

#ifdef __cplusplus
extern "C"
{
#endif

void  decode_pixcnt14         (uint8_t    *p_rawdata,
                               uint16_t   *p_image,
                               int         chip_index,
                               int         img_x0,
                               int         img_y0,
                               int         img_width);

void  decode_image14_onecnt   (uint8_t    *p_rawdata,
                               uint16_t   *p_image,
                               int         virtual_pixels_correction);

void  decode_image14_twocnt   (uint8_t    *p_rawdata_low,
                               uint8_t    *p_rawdata_high,
                               uint16_t   *p_image,
                               int         virtual_pixels_correction);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // DECODE14B_H_INCLUDED
