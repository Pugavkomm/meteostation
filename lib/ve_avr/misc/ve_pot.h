/**
 * @file ve_pot.h
 * @brief Potentiometer Helper Functions Header
 * @author andrey
 * @date 11.05.2012
 * @copyright GNU Public License
 */
#ifndef POT_H_
#define POT_H_

/**
 * @defgroup Pot Potentiometer
 * @ingroup Helpers
 * @brief Potentiometer classes
 *
 * @addtogroup Pot
 * @{
 */

/**
 * @brief Potentiometer class
 */
class Pot
{
protected:
	AnalogMux::Channel m_channel;	///< ADC channel
    uint16_t m_minVal;				///< Minimum value
    uint16_t m_dVal;				///< Delta (Maximum - Minimum) value
    ADConv::Prescaler m_presc;		///< ADC Prescaler
    ADConv::TriggerSrc m_tsrc;		///< ADC Trigger Source
public:
    void setup(AnalogMux::Channel channel,
    			uint16_t minVal = 0,
    			uint16_t maxVal = 1024,
    			ADConv::Prescaler presc = ADConv::Default_2,
    			ADConv::TriggerSrc tsrc = ADConv::FreeRunningMode);
    uint16_t getValue();
protected:
    void switchChannel();
};
/**
 * @brief Potentiometer setup
 * @param[in] channel ADC channel
 * @param[in] minVal Mimimum value
 * @param[in] maxVal Maximum value
 * @param[in] presc ADC Prescaler
 * @param[in] tsrc ADC Trigger Source
 */
void Pot::setup(AnalogMux::Channel channel,
				  uint16_t minVal,
				  uint16_t maxVal,
				  ADConv::Prescaler presc,
				  ADConv::TriggerSrc tsrc)
{
    m_channel = channel;
    m_minVal = minVal;
    m_dVal = maxVal - minVal;
    m_presc = presc;
    m_tsrc = tsrc;
    switchChannel();
}
/**
 * @brief Switch ADC channel
 */
void Pot::switchChannel()
{
    DEV_ADC.setLeftAdjustResult(true);
    DEV_ADC.setPrescaler(m_presc);
    DEV_ADC.setTriggerSource(m_tsrc);
    DEV_ADC.autoTriggerEnable();
    DEV_ADC.setChannel(m_channel);
    DEV_ADC.setVoltageReference(AnalogMux::AVCC);
    DEV_ADC.enable();
    DEV_ADC.startConversion();
}
/**
 * @brief Read Potentiometer value
 */
uint16_t Pot::getValue()
{
    if (DEV_ADC.channel() != m_channel)
        switchChannel();
    DEV_ADC.resetIntFlag();
    while (! DEV_ADC.isConversionStarted());
    unsigned long ret = DEV_ADC.result();
    ret *= m_dVal;
    ret >>= 16;
    return ret + m_minVal;
}
/**
 * @}
 */
#endif /* POT_H_ */
