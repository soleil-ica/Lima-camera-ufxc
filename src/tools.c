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

#include "../include/constants.h"

/* Function to calculate the time difference between two timestamps in ms
 */
double time_diff(clock_t time_start, clock_t time_stop){
  return (double)(time_stop - time_start)*1000/CLOCKS_PER_SEC;
}


/* Function to check the folder path and print all files in the folder.
 */
void list_dir(char *dirpath){
  DIR *dir;  // DIR type pointer
  struct dirent *entry;  // pointer to the directory entry
  int cnt = 0;  // counter of detected files

  // open folder and list all files
  dir = opendir(dirpath);
  if(dir == NULL){
    printf("  Cannot open the directory (%s).\n", dirpath);
  }
  else{
    printf("%s:\n", dirpath);
    // First read is only to get number of files
    while((entry=readdir(dir)) != NULL){
      // ignore all files that starts with a dot
      if(entry->d_name[0] != '.'){
        printf("  %s\n", entry->d_name);
        cnt++;
      }
    }
    closedir(dir);
    printf("Total files: %d\n", cnt);
  }
}


/* Function to get number of values in the data file.
 */
int file_count_values(char *filepath){
  FILE *p_file;
  int size = 0;
  int tmp = 0;

  p_file = fopen(filepath, "r");

  while(fscanf(p_file, "%d", &tmp)==1){
    size++;
  }
  fclose(p_file);
  return size;
}

/* Function to get number of lines a file.
 */
int file_count_lines(char *filepath){
  FILE *p_file;
  int lines = 0;
  char c;

  p_file = fopen(filepath, "r");

  while((c=getc(p_file)) != EOF){
    if(c == '\n')
      lines++;
  }
  fclose(p_file);
  return lines;
}


/* Function to read a raw file to extract acquisition parameters .
 */
void scan_rawdata_file(char *rawfile, char *rawdir, int *p_acqmode,
                       int *p_nbimages, int *p_rawsize){
  char fname[1024];
  int data_size;
  clock_t t0, t1;

  // get the file path
  printf("-> Enter the file name: ");
  scanf("%s", fname);

  t0 = clock();
  // build path
  sprintf(rawfile, "%s%s%s", rawdir, PATHSEP, fname);
  printf("\n");
  printf("> Scanning file:\n");
  printf(">  %s\n", rawfile);

  // analyze the raw file in order to extract acquisition parameters
  data_size = file_count_values(rawfile);
  *p_nbimages = file_count_lines(rawfile); // divide by two in order
  *p_rawsize = data_size/(*p_nbimages);

  if(*p_rawsize == RAW_SIZE_14BITS){
    *p_acqmode = ACQMODE_14BITS;
  }
  else if(*p_rawsize == RAW_SIZE_2BITS){
    *p_acqmode = ACQMODE_2BITS;
  }
  else{
    *p_acqmode = -1;
  }
  t1 = clock();
  printf("\n");
  printf("> Detected acquisition parameters in %.1f ms\n", time_diff(t0, t1));
  printf(">  raw data size = %d Bytes\n", *p_rawsize);
  printf(">  acquisition mode = %d\n", *p_acqmode);
  printf(">  number of images = %d\n", *p_nbimages);
}


/* Function to read raw data file and store it in the allocated buffers
 */
void rawdata_to_memory(char *filepath, uint8_t **p_rawbuffer, int line_len){
  FILE *p_rawfile;
  int line_cnt = 0;
  int cnt = 0;
  int tmp = 0; // read value
  uint8_t *addr; //current buffer address (current line/image)

  // get first buffer address
  addr = *(p_rawbuffer+line_cnt);

  // open file
  p_rawfile = fopen(filepath, "r");

  //    printf("%d\n", line_cnt);
  // scan values
  while(fscanf(p_rawfile, "%d", &tmp)!=EOF){
    *(addr+cnt) = (uint8_t)tmp;

    if (cnt == line_len-1){
      line_cnt++;
      addr = *(p_rawbuffer+line_cnt);
      cnt = 0;
      //printf("%d\n", line_cnt);
    }
    else
      cnt++;
  }
  fclose(p_rawfile);
}


/*  Function to save decoded 16-bits image to a text file
 */
void save_image16(char *filepath, uint16_t *p_image, int nb_rows, int nb_lines){
  FILE *p_imgfile;
  int row = 0;
  int col = 0;

  p_imgfile = fopen(filepath, "w");
  for(row=0; row<nb_rows; row++){
    for(col=0; col<nb_lines; col++){
      fprintf(p_imgfile, "%d ", *p_image);
      p_image++;
    }
    fprintf(p_imgfile, "\n");
  }

  fclose(p_imgfile);
}


/*  Function to save decoded 32-bits image to a text file
 */
void save_image32(char *filepath, uint32_t *p_image, int nb_rows, int nb_lines){
  FILE *p_imgfile;
  int row = 0;
  int col = 0;

  p_imgfile = fopen(filepath, "w");
  for(row=0; row<nb_rows; row++){
    for(col=0; col<nb_lines; col++){
      fprintf(p_imgfile, "%d ", *p_image);
      p_image++;
    }
    fprintf(p_imgfile, "\n");
  }

  fclose(p_imgfile);
}
