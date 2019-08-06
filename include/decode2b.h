#ifndef DECODE2B_H_INCLUDED
#define DECODE2B_H_INCLUDED

#ifdef __cplusplus
extern "C"
{
#endif

void decode_chip_pixcnt2(uint8_t *p_rawdata, uint16_t *p_image, int chip_index,
                         int img_x0, int img_y0, int size_x);
void correct_value_image2(uint8_t *p_image, int image_size);
void accumulate_image2(uint32_t *p_accumulated_image, uint8_t *p_image,
                       int image_size);
void correct_gaps_image2_acc(uint32_t *p_image, uint32_t *p_corrected_image,
                             int *gaps_cols, int gaps_nb, int size_x, int size_y);
void decode_image2_onecnt(uint8_t *p_rawdata, uint8_t *p_image);
void decode_image2_twocnt(uint8_t *p_rawdata_low, uint8_t *p_rawdata_high,
                          uint8_t *p_image);
void decode_image2_pumpprobe(uint8_t **p_rawdata, uint32_t *p_image,
                             int images_nb, int gap_correction);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // DECODE2B_H_INCLUDED
