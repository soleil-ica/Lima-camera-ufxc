#ifndef DECODE14B_H_INCLUDED
#define DECODE14B_H_INCLUDED

#ifdef __cplusplus
extern "C"
{
#endif

void decode_chip_pixcnt14(uint8_t *p_rawdata, uint16_t *p_image, int chip_index,
                          int img_x0, int img_y0, int size_x);
void correct_value_image14(uint16_t *p_image, int image_size);
void correct_gaps_image14(uint16_t *p_image, uint16_t *p_corrected_image,
                          int *gaps_cols, int gaps_nb, int size_x, int size_y);
void decode_image14_onecnt(uint8_t *p_rawdata, uint16_t *p_image,
                           int gap_correction);
void decode_image14_twocnt(uint8_t *p_rawdata_low, uint8_t *p_rawdata_high,
                           uint16_t *p_image, int gap_correction);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // DECODE14B_H_INCLUDED
