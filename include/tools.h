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
                       int *p_nbimages, int *p_rawsize);
void rawdata_to_memory(char *filepath, uint8_t **p_rawbuffer, int line_len);
void save_image16(char *filepath, uint16_t *p_image, int nb_rows, int nb_lines);
void save_image32(char *filepath, uint32_t *p_image, int nb_rows, int nb_lines);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TOOLS_H_INCLUDED
