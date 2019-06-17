#ifndef DECODE2B_H_INCLUDED
#define DECODE2B_H_INCLUDED

#ifdef __cplusplus
extern "C"
{
#endif
void update_pixcnt2(uint8_t *p_pixcnt2, uint8_t rawbit, int bit_pos);
void correct_pixcnt2(uint8_t *p_pixcnt2);
void correct_image2(uint8_t *p_imgdata2, int imgsize);
void decode_onechip_pixcnt2(uint8_t *p_rawdata, uint8_t *p_imgdata2,
                            int chip_index);
void decode_image2(uint8_t *p_rawdata, uint8_t *p_imgdata2);
void decode_onechip_cntsel_pixcnt2(uint8_t *p_rawdata, uint8_t *p_imgdata2,
                                    int chip_index, int counter_index);
void decode_image2_twocnts(uint8_t *p_rawdata_low, uint8_t *p_rawdata_high,
                            uint8_t *p_imgdata2);
#ifdef __cplusplus
}
#endif // __cplusplus

#endif // DECODE2B_H_INCLUDED
