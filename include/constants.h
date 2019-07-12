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
#define DET_SIZE_PIX          DET_X*DET_Y
#define DET_X_CORR            257
#define DET_Y_CORR            256
#define DET_SIZE_PIX_CORR     DET_X_CORR*DET_Y_CORR

// some constants for images decoding
#define CHIP_0                0
#define CHIP_1                1
#define CNT_L                 0
#define CNT_H                 1
#define PROBE_0_CNT_L         0
#define PROBE_0_CNT_H         1
#define PROBE_1_CNT_L         2
#define PROBE_1_CNT_H         3

// detector readout parameters
#define SOUT_LEN_BYTES        16 // number of Bytes for a single chip Sout line
#define PIXCNT_MASK_14BITS    0x3FFF

#endif // CONSTANTS_H_INCLUDED
