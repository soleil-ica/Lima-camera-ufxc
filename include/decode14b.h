#ifndef DECODE14B_H_INCLUDED
#define DECODE14B_H_INCLUDED

#ifdef __cplusplus
extern "C"
{
#endif
void update_pixcnt14(uint16_t *p_pixcnt14, uint8_t rawbit, int bit_pos);
void correct_pixcnt14(uint16_t *p_pixcnt14);
void correct_image14(uint16_t *p_imgdata14, int imgsize);
void decode_onechip_pixcnt14(uint8_t *p_rawdata, uint16_t *p_imgdata,
                                int chip_index);
void decode_image14(uint8_t *p_rawdata, uint16_t *p_imgdata14);
void decode_onechip_cntsel_pixcnt14(uint8_t *p_rawdata, uint16_t *p_imgdata14,
                                    int chip_index, int counter_index);
void decode_image14_twocnts(uint8_t *p_rawdata_low, uint8_t *p_rawdata_high,
                            uint16_t *p_imgdata14);
#ifdef __cplusplus
}
#endif // __cplusplus

#endif // DECODE14B_H_INCLUDED
