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

/* Function to return time difference in ms
 */
double time_diff(clock_t time_start, clock_t time_stop){
  return (double)(time_stop - time_start)*1000/CLOCKS_PER_SEC;
}

/* Function to check the folder and to get number of files.
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
      if(entry->d_name[0] != '.'){  // ignore all files that starts with a dot
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
int count_file_values(char *filepath){
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
int count_file_lines(char *filepath){
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


/* Function to read raw data file and store it in the preallocated
 */
void rawdata_to_memory(char *filepath, uint8_t *p_rawbuffer){
  FILE *p_rawfile;
  int tmp = 0;

  p_rawfile = fopen(filepath, "r");

  while(fscanf(p_rawfile, "%d", &tmp)==1){
    *p_rawbuffer = (uint8_t)tmp;
    p_rawbuffer++;
  }
  fclose(p_rawfile);
}

/*
 *  Function to read a raw file to extract acquisition parameters .
 */
void scan_rawdata_file(char *rawfile, char *rawdir, int *p_acqmode,
                       int *p_nbimages, int *p_rawsize){
  char fname[1024];
  int imgsize;

  // get the file path
  printf("-> Enter the file name: ");
  scanf("%s", fname);

  // build path
  sprintf(rawfile, "%s%s%s", rawdir, PATHSEP, fname);
  printf("\n");
  printf("> Reading file:\n");
  printf(">  %s\n", rawfile);

  // analyze the raw file in order to extract acquisition parameters
  *p_rawsize = count_file_values(rawfile);
  *p_nbimages = count_file_lines(rawfile); // divide by two in order
  imgsize = *p_rawsize/(*p_nbimages);

  if(imgsize == RAW_SIZE_2BITS){
    *p_acqmode = ACQMODE_2BITS;
  }
  else if(imgsize == RAW_SIZE_14BITS){
    *p_acqmode = ACQMODE_14BITS;
  }
  else{
    *p_acqmode = -1;
  }
  printf("\n");
  printf("> Detected parameters:\n");
  printf(">  raw data size = %d Bytes\n", *p_rawsize);
  printf(">  acquisition mode = %d\n", *p_acqmode);
  printf(">  number of images = %d\n", *p_nbimages);
}


void add_image(uint16_t *result, uint8_t *image2b, int nb_values){
  int i = 0;

  for(i=0; i<nb_values; i++){
    *(result+i) = *(result+i) + *(image2b+i);
  }
}


/*  Function to save decoded a 14 bits image to a text file
 */
void save_image(char *filepath, uint16_t *p_imgbuffer){
  FILE *p_imgfile;
  int i = 0;
  int j = 0;
  p_imgfile = fopen(filepath, "w");
  for(i=0; i<DET_Y; i++){
    for(j=0; j<DET_X; j++){
      fprintf(p_imgfile, "%d ", *p_imgbuffer);
      p_imgbuffer++;
    }
    fprintf(p_imgfile, "\n");
  }

  fclose(p_imgfile);
}


/*  Function to save decoded a 14 bits image to a text file
 */
void save_image_twocnts(char *filepath, uint16_t *p_imgbuffer){
  FILE *p_imgfile;
  int i = 0;
  int j = 0;

  p_imgfile = fopen(filepath, "w");
  for(i=0; i<DET_Y; i++){
    for(j=0; j<2*DET_X; j++){
      fprintf(p_imgfile, "%d ", *p_imgbuffer);
      p_imgbuffer++;
    }
    fprintf(p_imgfile, "\n");
  }

  fclose(p_imgfile);
}
