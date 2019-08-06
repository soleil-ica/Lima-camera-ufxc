#ifndef TOOLS_H_INCLUDED
#define TOOLS_H_INCLUDED

/*
 * Functions
 */

#ifdef __cplusplus
extern "C"
{
#endif

double time_diff(clock_t time_start, clock_t time_stop);
void list_dir(char *dirpath);
int file_count_values(char *filepath);
int file_count_lines(char *filepath);
void scan_rawdata_file(char *rawfile, char *rawdir, int *p_acqmode,
                       int *p_images_nb, int *p_rawsize);
void rawdata_to_memory(char *filepath, uint8_t **p_rawbuffer, int line_len);
void save_image16(char *filepath, uint16_t *p_image, int size_x, int size_y);
void save_image32(char *filepath, uint32_t *p_image, int size_x, int size_y);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TOOLS_H_INCLUDED
