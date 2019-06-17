#ifndef CONSTANTS_H_INCLUDED
#define CONSTANTS_H_INCLUDED

#ifdef WIN32
  #define PATHSEP "\\"
#else
  #define PATHSEP "/"
#endif

// acquisition modes
#define ACQMODE_14BITS        0
#define ACQMODE_2BITS         1

// data size for different acquisition modes
#define RAW_SIZE_14BITS       114688
#define RAW_SIZE_2BITS        16384

// raw data size - single chips
#define CHIP_RAW_SIZE_14BITS  57344
#define CHIP_RAW_SIZE_2BITS   8192

// detector size
#define CHIP_X                128
#define CHIP_Y                256
#define CHIPS_N               2
#define CHIP_SIZE_PIX         CHIP_X*CHIP_Y
#define DET_X                 256
#define DET_Y                 256
#define DET_SIZE_PIX          CHIP_SIZE_PIX*CHIPS_N

// detector readout parameters
#define SOUT_LEN_BYTES        16 // number of Bytes for a single chip Sout line
#define PIXCNT_MASK_14BITS    0x3FFF

#endif // CONSTANTS_H_INCLUDED
