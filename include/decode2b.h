#ifndef DECODE2B_H_INCLUDED
#define DECODE2B_H_INCLUDED

#ifdef __cplusplus
extern "C"
{
#endif
void update_bit_pixcnt2(uint8_t *p_pixcnt2, uint8_t rawbit, int bit_pos);
void correct_pixcnt2(uint8_t *p_pixcnt2);
void correct_image2(uint8_t *p_image, int imgsize);
// functions to decode single counter data into separated images
void decode_onechip_pixcnt2(uint8_t *p_rawdata, uint8_t *p_image,
                            int chip_index);
void decode_image2(uint8_t *p_rawdata, uint8_t *p_image);
// functions to decode two counter data (low and high) into single image
void decode_onechip_pixcnt2_imgsel(uint8_t *p_rawdata, uint8_t *p_image,
                                   int chip_index, int img_rowlen,
                                   int img_x0, int img_y0);
void decode_image2_twocnts(uint8_t *p_rawdata_low, uint8_t *p_rawdata_high,
                           uint8_t *p_image);
// functions to decode counter data following the pump and probe scheme
void sum_image2_pumpprobe(uint32_t *p_sum_image, uint8_t *p_image,
                          int nb_values);
void decode_image2_pumpprobe(uint8_t **p_rawdata, uint32_t *p_image,
                             int nb_images, int geomcorr_enable);
#ifdef __cplusplus
}
#endif // __cplusplus

#endif // DECODE2B_H_INCLUDED
