/**
 *  \file ConfigDetector.h
 *  \brief header file of ConfigDetector class
 *  \author Ayoub GHAMMOURI
 *  \version 0.1
 *  \date November 29 2018
 *  Created on: July 30 2018
 */

#ifndef UFXCLIB_CONFIGDETECTOR_H_
#define UFXCLIB_CONFIGDETECTOR_H_

#include "ufxc/ConfigPortInterface.h"
#include "ufxc/UFXCLibTypesAndConsts.h"

/**
 * \namespace ufxclib
 */
namespace ufxclib
{
/**
 *  \class ConfigDetector
 *  \brief This class contains the necessary functions for the detector configuration.
 *
*/
class ConfigDetector
{
public:

    /**
    * \fn ConfigDetector(ConfigPortInterface * config_port_interface)
    * \brief constructor
    * \param config_port_interface : ConfigPortInterface object for all TCP communications with DAQ.
	* \return none
    */
    ConfigDetector(ConfigPortInterface * config_port_interface);

    /**
    * \fn virtual ~ConfigDetector()
    * \brief destructor
    * \param none
	* \return none
    */
    virtual ~ConfigDetector();

    /**
    * \fn void set_detector_registers_names(std::map<T_DetectorConfigKey, std::string> map)
    * \brief Set detector registers names
    * \param map : it is a map contain the list of registers names for the detector config
	* \return void
    */
    void set_detector_registers_names(std::map<T_DetectorConfigKey, std::string> map);

    /**
    * \fn bool get_CSA_polarity()
    * \brief Get CSA polarity/charge collection mode, common to all pixels
    * \param none
	* \return CSA polarity/charge collection mode, common to all pixels
    */
    bool get_CSA_polarity();

    /**
    * \fn void set_CSA_polarity(bool polarity)
    * \brief Set CSA polarity/charge collection mode, common to all pixels
    * \param polarity : CSA polarity/charge collection mode, common to all pixels
	* \return void
    */
    void set_CSA_polarity(bool polarity);

    /**
    * \fn bool get_shaper_feedback()
    * \brief Get shaper feedback control, common to all pixels
    * \param none
	* \return Shaper feedback control, common to all pixels
    */
    bool get_shaper_feedback();

    /**
    * \fn void set_shaper_feedback(bool feedback)
    * \brief Set shaper feedback control, common to all pixels
    * \param feedback : Shaper feedback control, common to all pixels
	* \return void
    */
    void set_shaper_feedback(bool feedback);

    /**
    * \fn yat::uint32 get_BCAS()
    * \brief Get CSA current in input transistor, common to all pixels
    * \param none
	* \return CSA current in input transistor, common to all pixels
    */
    yat::uint32 get_BCAS();

    /**
    * \fn void set_BCAS(yat::uint32 bcas)
    * \brief Set CSA current in input transistor, common to all pixels
    * \param bcas : CSA current in input transistor, common to all pixels
	* \return void
    */
    void set_BCAS(yat::uint32 bcas);

    /**
    * \fn yat::uint32 get_BKRUM()
    * \brief Get CSA feedback, common to all pixels
    * \param none
	* \return CSA feedback, common to all pixels
    */
    yat::uint32 get_BKRUM();

    /**
    * \fn void set_BKRUM(yat::uint32 bkrum)
    * \brief Set CSA feedback, common to all pixels
    * \param bkrum : CSA feedback, common to all pixels
	* \return void
    */
    void set_BKRUM(yat::uint32 bkrum);

    /**
    * \fn yat::uint32 get_BTRIM()
    * \brief Get trim DAC current, common to all pixels
    * \param none
	* \return Trim DAC current, common to all pixels
    */
    yat::uint32 get_BTRIM();

    /**
    * \fn void set_BTRIM(yat::uint32 btrim)
    * \brief Set trim DAC current, common to all pixels
    * \param btrim : Trim DAC current, common to all pixels
	* \return void
    */
    void set_BTRIM(yat::uint32 btrim);

    /**
    * \fn yat::uint32 get_BREF()
    * \brief Get CSA current, common to all pixels
    * \param none
	* \return CSA current, common to all pixels
    */
    yat::uint32 get_BREF();

    /**
    * \fn void set_BREF(yat::uint32 bref)
    * \brief Set CSA current, common to all pixels
    * \param bref : CSA current, common to all pixels
	* \return void
    */
    void set_BREF(yat::uint32 bref);

    /**
    * \fn yat::uint32 get_BSH()
    * \brief Get shaper current, common to all pixels
    * \param none
	* \return Shaper current, common to all pixels
    */
    yat::uint32 get_BSH();

    /**
    * \fn void set_BSH(yat::uint32 bsh)
    * \brief Set shaper current, common to all pixels
    * \param bsh : shaper current, common to all pixels
	* \return void
    */
    void set_BSH(yat::uint32 bsh);

    /**
    * \fn yat::uint32 get_BDIS()
    * \brief Get discriminator current, common to all pixels
    * \param none
	* \return Discriminator current, common to all pixels
    */
    yat::uint32 get_BDIS();

    /**
    * \fn void set_BDIS(yat::uint32 bdis)
    * \brief Set discriminator current, common to all pixels
    * \param bdis : Discriminator current, common to all pixels
	* \return void
    */
    void set_BDIS(yat::uint32 bdis);

    /**
    * \fn yat::uint32 get_BGSH()
    * \brief Get shaper feedback control in case DET_GLB_FS=0, common to all pixels
    * \param none
	* \return Shaper feedback control in case DET_GLB_FS=0, common to all pixels
    */
    yat::uint32 get_BGSH();

    /**
    * \fn void set_BGSH(yat::uint32 bgsh)
    * \brief Set shaper feedback control in case DET_GLB_FS=0, common to all pixels
    * \param bgsh : shaper feedback control in case DET_GLB_FS=0, common to all pixels
	* \return void
    */
    void set_BGSH(yat::uint32 bgsh);

    /**
    * \fn yat::uint32 get_BR()
    * \brief Get reference level DAC, common to all pixels
    * \param none
	* \return Reference level DAC, common to all pixels
    */
    yat::uint32 get_BR();

    /**
    * \fn void set_BR(yat::uint32 br)
    * \brief Set reference level DAC, common to all pixels
    * \param br : reference level DAC, common to all pixels
	* \return void
    */
    void set_BR(yat::uint32 br);

    /**
    * \fn void apply_detector_config()
    * \brief Function to configure all detectors global and local configuration registers located inside the detector readout circuits.
    *  The global registers define all common parameters of the pixel analog fronted such as: sensor polarity, internal DACs reference signal, fronted speed and others.
    *  The local configuration registers are unique for every pixel and are necessary to correct for fabrication mismatch between pixels during fabrication process, such as DC offset and gain, and supplementary fronted configuration, i.e. gain.
    *  For each detector configuration a calibration file will be delivered with all local and global registers.    *
    * \param none
	* \return void
    */
    void apply_detector_config();

    /**
    * \fn void set_pixel_config(std::vector<std::vector<std::uint32_t>> pixels)
    * \brief After reading the PIXCONFIG from the config file, we set the config pixels in a vector and we pass this array as parameters to the set_pixel_config function.
    * This table consists of 7168 rows and 8 columns. The columns are ordered from left to right as follows: 96B, 64B, 32B, 0B, 96A, 64A, 32A, and 0A.
    * each value in the array is 32-bit encoded.
    * In this function, the values of the array are sent to DAQ in a specific order.
    * \param pixels: pixels config vector
	* \return void
    */
    void set_pixel_config(std::vector<std::vector<std::uint32_t>> pixels);

private:
    std::map<T_DetectorConfigKey, std::string> m_detector_registers;
    ConfigPortInterface * m_config_port_interface;
};
} /// namespace ufxclib

#endif //// UFXCLIB_CONFIGDETECTOR_H_
