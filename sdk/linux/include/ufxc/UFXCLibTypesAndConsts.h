/**
 *  \file UFXCLibTypesAndConsts.h
 *  \brief Basic types and Constants for UFXC library
 *  \author Ayoub GHAMMOURI
 *  \version 0.1
 *  \date November 29 2018
 *  Created on: July 23 2018
 */

#ifndef UFXCLIB_UFXLIBTYPESANDCONSTS_H_
#define UFXCLIB_UFXLIBTYPESANDCONSTS_H_


#include <iostream>
#include "ufxc/UFXCException.h"

/**
 * \namespace ufxclib
 */
namespace ufxclib
{
#define kUFXC_BOX_WAIT  1010 // ms /// read data timeout_ms
#define kCAPTURE_MAX_BUFF_SIZE 65536 /// Capture max buffer size
#define kCAPTURE_HEADER_MAX_SIZE 4096 /// Capture max header size (32 positions max)
#define UFXC_CFG_GLB_REG_SIZE 128
#define DAQ_CLOCK_PERIOD_NS 1000000
#define SIZE_SOCK_BUF 245760
#define COUNTER_0_MIN_FRAMES_NUMBER 57
#define COUNTER_0_MAX_FRAMES_NUMBER 112
#define COUNTER_1_MIN_FRAMES_NUMBER 1
#define COUNTER_1_MAX_FRAMES_NUMBER 56
#define SFP_NUMBER 3
#define FRAME_SIZE 1030
#define IMAGE_HEADER_SIZE 4
#define CHIP_A_ID  26 /// 0x1A
#define CHIP_B_ID  43 /// 0x2B
#define PACKET_DATA_LENGTH 1024 ///1030 - 6(header size)
#define PACKET_HEADER_SIZE 6
#define COUNTER_NUMBER 2
#define UFXC_LIB_VERSION "0.1" /// the library version
#define IMAGE_WIDTH 256 /// current image width
#define IMAGE_HEIGHT 256  /// current image height
#define CONFIG_FRAMES_NUMBER 224
#define CLOCK_CYCLES 5

/**
* UFXCLIB consts
*/
const std::string kUFXC_OK_REQUEST_STRING = "OK =";
const std::string kUFXC_OK_CMD_STRING = "OK";
const std::string kUFXC_ERROR_STRING = "ERR";
const std::string kUFXC_NO_ERROR_STRING_STR = "Ok";
const std::string kUFXC_NO_ERROR_STRING_NUM = "0";
const std::string kUFXC_ABSOLUTE_COUNTING_ABS = "Absolute";
const std::string kUFXC_ABSOLUTE_COUNTING_REL = "Relative";
const std::string kUFXC_CAPTURE_FRAMED_BLOCK_START = "BIN ";
const std::string kUFXC_CAPTURE_DATA_CAPT_END = "END ";
const std::string kUFXC_CAPTURE_HEADER_START = "missed:";
const std::string kUFXC_CAPTURE_HEADER_REG_LIST = "fields:";
const std::string kUFXC_CAPTURE_HEADER_END = "\n\n";

/**
 * \enum T_MonitoringKey
 * \brief Monitoring keys functions
 */
typedef enum
{
    TEMP_DAQ_PICO,
    TEMP_DAQ_SFP,
    TEMP_DAQ_PSU,
    DETECTOR_STATUS,
    FW_DELAY,
    DELAY_SCAN,
    ALIM_DET_1V2_VDDA_B,
    ALIM_DET_1V2_VDDA_A,
    ALIM_DET_1V2_DISC_B,
    ALIM_DET_1V2_DISC_A,
    ALIM_DET_0V8_VDDM,
    ALIM_DET_1V2_CORE,
    ALIM_DET_P_HV_CHIP,
    ALIM_DET_P_HV_CURRENT,
    TEMP_DET,
    Abortdelay,
	FIRMWARE_VERSION,
	EN_PIXCONF_SCANDELAY_SFP

} T_MonitoringKey;

/**
 * \enum T_DetectorConfigKey
 * \brief Detector keys functions
 */
typedef enum
{
    GLB_POL, /// CSA polarity
    GLB_FS, /// shaper feedback control
    GLB_BCAS, /// CSA current in input transistor
    GLB_BKRUM, /// CSA feedback
    GLB_BTRIM, /// DAC current
    GLB_BREF, /// CSA current
    GLB_BSH, /// shaper current
    GLB_BDIS, /// Discriminatory current
    GLB_BGSH, /// : shaper feedback control in case DET_GLB_FS=0
    GLB_BR, /// reference level DAC
    DetectorConfig, /// apply_detector_config
    PIXCONF_BITLINE_NBR,
    PIXCONF_COL_31_0_A,
    PIXCONF_COL_63_32_A,
    PIXCONF_COL_95_64_A,
    PIXCONF_COL_127_96_A,
    PIXCONF_COL_31_0_B,
    PIXCONF_COL_63_32_B,
    PIXCONF_COL_95_64_B,
    PIXCONF_COL_127_96_B,
    PixLineConfig

} T_DetectorConfigKey;

/**
 * \enum T_AcquisitionConfigKey
 * \brief Acquisition keys functions
 */
typedef enum
{
    DET_THRESHOLD_LOW_1,    /// Low discriminator threshold
    DET_THRESHOLD_LOW_2,    /// Low discriminator threshold
    DET_THRESHOLD_HIGH_1,   /// High discriminator threshold
    DET_THRESHOLD_HIGH_2,   /// High discriminator threshold
    ACQ_MODE,               /// Detector acquisition mode
    ACQ_COUNT_TIME,         /// Detector counting time in µs
    ACQ_WAIT_TIME,          /// Time to wait after counting process in µs
    ACQ_NIMG,               /// Number of images to acquire after each trigger
    ACQ_NTRIG,              /// Number of external triggers during an acquisition.
    StartAcq,               /// Start Acquisition
    AbortAcq,               /// Stop Acquisition
    SFP_SOFT_RESET,         /// Reset SFPs in case of error
    FMC_SOFT_RESET          /// Reset data fifo, image frame count, FMC.DETECTOR_STATUS=E_DET_NOT_CONFIGURED

} T_AcquisitionConfigKey;

/**
 * \enum T_SFPconfigKey
 * \brief SFPs keys functions
 */
typedef enum
{
    SFP_OUR_IP_AD_BYTE1, /// SFP 1 source IP address Byte 1 (byte integer value).
    SFP_OUR_IP_AD_BYTE2, /// SFP 1 source IP address Byte 2 (byte integer value).
    SFP_OUR_IP_AD_BYTE3, /// SFP 1 source IP address Byte 3 (byte integer value).
    SFP_OUR_IP_AD_BYTE4, /// SFP 1 source IP address Byte 4 (byte integer value).
    SFP_OUR_UDP_PORT, /// SFP 1 source UDP Port (16bit integer value).
    SFP_DEST_IP_AD_BYTE1, /// SFP 1 destination IP address Byte 1 (byte integer value).
    SFP_DEST_IP_AD_BYTE2, /// SFP 1 destination IP address Byte 2 (byte integer value).
    SFP_DEST_IP_AD_BYTE3, /// SFP 1 destination IP address Byte 3 (byte integer value).
    SFP_DEST_IP_AD_BYTE4, /// SFP 1 destination IP address Byte 4 (byte integer value).
    SFP_DEST_UDP_PORT, /// SFP 1 destination UDP Port (16bit integer value).
    SFP_COUNT_UDPTX_ERR /// SFP 1 UDP TX error count (example no SFP cable plugged causing internal Fifo to overload).

} T_SFPconfigKey;

/**
 * \enum T_SFPName
 * \brief SFPs names
 */
typedef enum
{
	SFP1 = 1,
	SFP2,
	SFP3

} T_SFPName;

/**
 * \enum T_Mode
 * \brief Acquisition Mode register
 */
typedef enum
{
    software = 0, /// 256 × 256 pixels 2 counters per pixel 14 bits per counter Pixel rearrangement done in DAQ FPGA.
    external, /// 256 × 256 pixels 2 counters per pixel 14 bits per counter Pixel rearrangement done in DAQ FPGA.
    pump_and_probe, /// 256 × 256 pixels 2 counters per pixel 2 bits per counter Pixel rearrangement done in DAQ FPGA.
    software_raw, /// 256 × 256 pixels 2 counters per pixel 14 bits per counter Raw data from detector Sout (no pixel rearrangement).
    external_raw, /// 256 × 256 pixels 2 counters per pixel 14 bits per counter Raw data from detector Sout (no pixel rearrangement).
    pump_and_probe_raw /// 256 × 256 pixels 2 counters per pixel 2 bits per counter Raw data from detector Sout (no pixel rearrangement).

} T_Mode;

/**
 * \enum T_Protocol
 * \brief Protocol type
 */
typedef enum
{
    UDP,
	TCP

} T_Protocol;

/**
 * \enum T_DetectorStatus
 * \brief detector status value
 */
typedef enum T_DetectorStatus
{
    E_DET_READY = 0,  /// detector ready for acquisition
    E_DET_BUSY,   /// acquisition in progress
    E_DET_ERROR, /// acquisition error
    E_DET_DELAY_SCANNING, /// Delay scan in progress
    E_DET_CONFIGURING, /// Detector configuration in progress
    E_DET_NOT_CONFIGURED /// detector not yet configured

} T_DetectorStatus;

/**
 * \struct T_UfxcLibCnx
 * \brief struct collects the parameters needed to connect to the DAQ
 */
typedef struct T_UfxcLibCnx
{
    /// members --------------------

    /// ip address
    std::string ip_address;

    /// configuration port
    unsigned long configuration_port;

    /// socket timeout_ms (ms)
    unsigned long socket_timeout_ms;

    /// Protocol type
    T_Protocol protocol;

    /// default constructor -----------------------
    T_UfxcLibCnx() :
            ip_address("localhost"), configuration_port(8888), socket_timeout_ms(
                    3000), protocol(TCP)
    {
    }

    /// destructor -----------------------
    ~T_UfxcLibCnx()
    {
    }

    //- copy constructor ------------------
    T_UfxcLibCnx(const T_UfxcLibCnx& src)
    {
        *this = src;
    }

    /// operator= ------------------
    const T_UfxcLibCnx & operator=(const T_UfxcLibCnx& src)
    {
        if (this == &src)
        {
            return *this;
        }

        ip_address = src.ip_address;
        configuration_port = src.configuration_port;
        socket_timeout_ms = src.socket_timeout_ms;
        protocol = src.protocol;

        return *this;
    }

    /// dump -----------------------
    void dump() const
    {
        std::cout << "T_UfxcLibCnx::ip_address........." << ip_address
                << std::endl;

        std::cout << "T_UfxcLibCnx::configuration_port........."
                << configuration_port << std::endl;

        std::cout << "T_UfxcLibCnx::socket_timeout_ms........."
                << socket_timeout_ms << std::endl;

        std::cout << "T_UfxcLibCnx::protocol........." << protocol << std::endl;
    }

} T_UfxcLibCnx;

/**
 * \struct T_UDPConfig
 * \brief struct collects the UDP config for each SFP
 */
typedef struct T_UDPConfig
{
    /// members --------------------

    /// output number, from 1 to 3
    unsigned short sfp;

    /// MAC address
    unsigned long long mac_address;

    /// IP destination address
    std::string ip_des_address;

    /// UDP destination port
    yat::uint32 udp_des_port;

    /// SFP internal IP address
    std::string ip_our_address;

    /// SFP internal port
    yat::uint32 udp_our_port;

    /// default constructor -----------------------
    T_UDPConfig() :
            mac_address(12345), ip_des_address("localhost"), udp_des_port(8888), ip_our_address(
                    "localhost"), udp_our_port(8888), sfp(1)
    {
    }

    /// destructor -----------------------
    ~T_UDPConfig()
    {
    }

    //- copy constructor ------------------
    T_UDPConfig(const T_UDPConfig& src)
    {
        *this = src;
    }

    /// operator= ------------------
    const T_UDPConfig & operator=(const T_UDPConfig& src)
    {
        if (this == &src)
        {
            return *this;
        }
        mac_address = src.mac_address;
        ip_des_address = src.ip_des_address;
        udp_des_port = src.udp_des_port;
        ip_our_address = src.ip_our_address;
        udp_our_port = src.udp_our_port;
        sfp = src.sfp;

        return *this;
    }

    /// dump -----------------------
    void dump() const
    {
        std::cout << "T_UDPConfig::mac_address........." << mac_address
                << std::endl;

        std::cout << "T_UDPConfig::ip_des_address........." << ip_des_address
                << std::endl;

        std::cout << "T_UDPConfig::udp_des_port........." << udp_des_port
                << std::endl;

        std::cout << "T_UDPConfig::ip_our_address........." << ip_our_address
                << std::endl;

        std::cout << "T_UDPConfig::udp_our_port........." << udp_our_port
                << std::endl;

        std::cout << "T_UDPConfig::sfp........." << sfp << std::endl;
    }

} T_UDPConfig;

} /// namespace ufxclib

#endif /// UFXCLIB_UFXLIBTYPESANDCONSTS_H_
