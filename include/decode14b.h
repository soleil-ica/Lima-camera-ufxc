#ifndef DECODE14B_H_INCLUDED
#define DECODE14B_H_INCLUDED

#ifdef __cplusplus
extern "C"
{
#endif

void update_bit_pixcnt14(uint16_t *p_pixcnt14, uint8_t rawbit, int bit_pos);
void correct_pixcnt14(uint16_t *p_pixcnt14);
void correct_image14(uint16_t *p_image, int imgsize);
// functions to decode single counter data into separated images
void decode_onechip_pixcnt14(uint8_t *p_rawdata, uint16_t *p_image,
                             int chip_index);
void decode_image14(uint8_t *p_rawdata, uint16_t *p_image);
// functions to decode two counter data (low and high) into single image
void decode_onechip_pixcnt14_imgsel(uint8_t *p_rawdata, uint16_t *p_image,
                                    int chip_index, int img_rowlen,
                                    int img_x0, int img_y0);
void decode_image14_twocnts(uint8_t *p_rawdata_low, uint8_t *p_rawdata_high,
                            uint16_t *p_image);
//void geomcorr_image14(uint16_t *p_image, uint16_t *p_corr_image,
//                      int imgsize_x, int imgsize_y);

void geomcorr_image14(int imgsize_x, int imgsize_y);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // DECODE14B_H_INCLUDED
