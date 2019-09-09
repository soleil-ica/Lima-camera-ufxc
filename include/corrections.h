#ifndef CORRECTIONS_H_INCLUDED
#define CORRECTIONS_H_INCLUDED

#ifdef __cplusplus
extern "C"
{
#endif

void  virtual_columns_correction_image16  (uint16_t   *p_image,
                                           int        *p_virtual_columns,
                                           int         virtual_columns_nb,
                                           int         img_width,
                                           int         img_height);

void  virtual_columns_correction_image32  (uint32_t   *p_image,
                                           int        *p_virtual_columns,
                                           int         virtual_columns_nb,
                                           int         img_width,
                                           int         img_height);

void  sum_images32                        (uint32_t   *p_image_sum,
                                           uint32_t   *p_image,
                                           int         image_size);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // CORRECTIONS_H_INCLUDED
