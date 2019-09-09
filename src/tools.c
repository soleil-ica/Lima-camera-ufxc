/******************************************************************************
 * File: files.c
 * Author: Arkadiusz Dawiec (arkadiusz.dawiec@synchrotron-soleil.fr)
 * Date: 28/05/2019
 *
 * Set of functions to read a raw file data and initialize the memory.
 *
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <dirent.h>
#include <time.h>
#include <omp.h>

#include "../include/constants.h"


/*
 * Function to start measurement of the elapsed time.
 *
 * Arguments:
 * - clock_t *timestamp : pointer to the timestamp
 */
void start_timer (clock_t *timestamp)
{
  *timestamp = clock();
}


/*
 * Function to stop measurement of the elapsed time.
 *
 * Arguments:
 * - clock_t *timestamp       : pointer to the timestamp
 *
 * Return:
 * - double   elapsed_time_ms : elapsed time in miliseconds
 */
double stop_timer (clock_t *timestamp)
{
  double elapsed_time_ms;

  elapsed_time_ms = (double) (clock() - *timestamp)*1000/CLOCKS_PER_SEC;

  return elapsed_time_ms;
}


/*
 * Function to start measurement of the elapsed time on the openmp parallel
 * region.
 *
 * Arguments:
 * - double timer : pointer to the openmp timer
 */
void start_timer_omp (double *timer)
{
  *timer = omp_get_wtime();
}


/*
 * Function to start measurement of the elapsed time on the openmp parallel
 * region.
 *
 * Arguments:
 * - double timer           : pointer to the openmp timer
 *
 * Return:
 * - double elapsed_time_ms : elapsed time in miliseconds
 */
double stop_timer_omp (double *timer)
{
  double elapsed_time_ms;

  elapsed_time_ms = 1000*(omp_get_wtime() - *timer);

  return elapsed_time_ms;
}


/*
 * Function to check the folder path and print all files in the folder.
 *
 * Arguments:
 * - char *dirpath : folder path
 */
void list_dir (char *dirpath)
{
  DIR    *dir;
  struct  dirent *entry;
  int     files_cnt = 0;  // counter of detected files

  // open folder and list all files
  dir = opendir(dirpath);
  if (dir == NULL)
    {
      printf("  Cannot open the directory (%s).\n", dirpath);
    }
  else
    {
      printf("%s:\n", dirpath);

      // The first read is only to get number of files
      while ((entry=readdir(dir)) != NULL)
        {
          // ignore all files that starts with a dot
          if(entry->d_name[0] != '.')
            {
              printf("  %s\n", entry->d_name);
              files_cnt++;
            }
        }
      closedir(dir);
      printf("Total files: %d\n", files_cnt);
    }
}


/*
 * Function to get number of numerical values in the data file.
 *
 * Arguments:
 * - char *filepath : path to the data file
 *
 * Return:
 * - int   size     : number of numerical values
 */
int file_count_values (char *filepath)
{
  FILE *p_file;
  int   size = 0;
  int   tmp = 0;

  p_file = fopen(filepath, "r");

  while (fscanf(p_file, "%d", &tmp) == 1)
    {
      size++;
    }

  fclose(p_file);

  return size;
}


/*
 * Function to get number of lines a file.
 *
 * Arguments:
 * - char *filepath : path to the data file
 *
 * Return:
 * - int   lines    : number of lines in the data file
 */
int file_count_lines (char *filepath)
{
  FILE *p_file;
  int   lines = 0;
  char  c;

  p_file = fopen(filepath, "r");

  while ((c=getc(p_file)) != EOF)
    {
      if(c == '\n')
        lines++;
    }
  fclose(p_file);

  return lines;
}


/*
 * Function to read extract acquisition parameters from the raw data file.
 * The extraction of the main parameters is based on the data size, line length
 * and number of lines.
 *
 * Arguments:
 * - char *rawfile     : name of the raw data file
 * - char *rawdir      : path to the folder with the raw data file
 * - int  *p_acqmode   : extracted acquisition mode
 * - int  *p_images_nb : extracted number of images
 * - int  *p_rawsize   : extracted raw data size
 */
void scan_rawdata_file (char *rawfile,
                        char *rawdir,
                        int  *p_acqmode,
                        int  *p_images_nb,
                        int  *p_rawsize)
{
  char    fname[1024];
  int     data_size;
  clock_t timestamp = 0;
  double  elapsed_time;

  // get the file path
  printf("-> Enter the file name: ");
  scanf("%s", fname);

  start_timer(&timestamp);

  // build path
  sprintf(rawfile, "%s%s%s", rawdir, PATHSEP, fname);
  printf("\n");
  printf("> Scanning file:\n");
  printf(">  %s\n", rawfile);

  // analyze the raw file in order to extract acquisition parameters
  data_size = file_count_values(rawfile);
  *p_images_nb = file_count_lines(rawfile);
  *p_rawsize = data_size/(*p_images_nb);

  if (*p_rawsize == RAW_SIZE_14BITS)
    *p_acqmode = ACQMODE_14BITS;
  else if (*p_rawsize == RAW_SIZE_2BITS)
    *p_acqmode = ACQMODE_2BITS;
  else
    *p_acqmode = -1;

  elapsed_time = stop_timer(&timestamp);

  printf("\n");
  printf("> Detected acquisition parameters in %.1f ms\n", elapsed_time);
  printf(">  raw data size = %d Bytes\n", *p_rawsize);
  printf(">  acquisition mode = %d\n", *p_acqmode);
  printf(">  number of images = %d\n", *p_images_nb);
}


/*
 * Function to read raw data file and store it in the allocated buffers.
 * Every line from the raw data file will be stored in a separated buffer.
 *
 * Arguments:
 * - char     *filepath    : path to the raw data file
 * - uint8_t **p_rawbuffer : pointers to the allocated buffers
 * - int       line_len    : line length of the raw data file
 */
void rawdata_to_memory (char     *filepath,
                        uint8_t **p_rawbuffer,
                        int       line_len)
{
  FILE    *p_rawfile;
  int      line_cnt = 0;
  int      cnt = 0;
  int      tmp = 0; // read value
  uint8_t *addr; //current buffer address (current line/image)

  // get first buffer address
  addr = *(p_rawbuffer+line_cnt);

  // open file
  p_rawfile = fopen(filepath, "r");

  // scan values
  while (fscanf(p_rawfile, "%d", &tmp) != EOF)
    {
      *(addr+cnt) = (uint8_t)tmp;

      if (cnt == line_len-1)
        {
          line_cnt++;
          addr = *(p_rawbuffer+line_cnt);
          cnt = 0;
        }
      else
        {
          cnt++;
        }

    }
  fclose(p_rawfile);
}


/*
 * Function to save decoded 16-bits image to a text file.
 *
 * Arguments:
 * - char     *filepath : path to the output text file
 * - uint16_t *p_image  : image data buffer
 * - int       size_x   : length of the line in the output file
 * - int       size_y   : number of lines in the output file
 */
void save_image16 (char     *filepath,
                   uint16_t *p_image,
                   int       size_x,
                   int       size_y)
{
  FILE *p_imgfile;
  int   row = 0;
  int   col = 0;

  p_imgfile = fopen(filepath, "w");

  for (row=0; row<size_y; row++)
    {
      for (col=0; col<size_x; col++)
        {
          fprintf(p_imgfile, "%d ", *p_image);
          p_image++;
        }
      fprintf(p_imgfile, "\n");
    }

  fclose(p_imgfile);
}


/*
 * Function to save decoded 32-bits image to a text file.
 *
 * Parameters:
 * - char     *filepath : path to the output text file
 * - uint32_t *p_image  : image data buffer
 * - int       size_x   : length of the line in the output file
 * - int       size_y   : number of lines in the output file
 */
void save_image32 (char     *filepath,
                   uint32_t *p_image,
                   int       size_x,
                   int       size_y)
{
  FILE *p_imgfile;
  int   row = 0;
  int   col = 0;

  p_imgfile = fopen(filepath, "w");

  for (row=0; row<size_y; row++)
    {
      for (col=0; col<size_x; col++)
        {
          fprintf(p_imgfile, "%d ", *p_image);
          p_image++;
        }
      fprintf(p_imgfile, "\n");
    }

  fclose(p_imgfile);
}
