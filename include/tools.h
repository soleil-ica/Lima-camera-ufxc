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
int count_file_values(char *filepath);
int count_file_lines(char *filepath);
void rawdata_to_memory(char *filepath, uint8_t *p_rawbuffer);
void scan_rawdata_file(char *rawfile, char *rawdir, int *p_acqmode, int *p_nbimages, int *p_rawsize);
void add_image(uint16_t *result, uint8_t *image2b, int nb_values);
void save_image(char *filepath, uint16_t *p_imgbuffer);
void save_image_twocnts(char *filepath, uint16_t *p_imgbuffer);
#ifdef __cplusplus
}
#endif // __cplusplus

#endif // TOOLS_H_INCLUDED
