/**
 * @file ve_rc.h
 * @brief RC Oscillator Control Header
 * @author andrey
 * @date 07.04.2016
 * @copyright GNU Public License
 */
#ifndef VE_AVR_VE_RC_H_
#define VE_AVR_VE_RC_H_

/**
 * @brief RC Oscillator Control class
 * @ingroup Clock
 */
class OscControl : public AVR_RC
{
public:
	/**
	 * @brief RC Oscillator Frequency Select type
	 */
	typedef enum {
		Freq_8MHz = 0,		///< 8 MHz
		Freq_1MHz			///< 1 MHz
	} RCFreq;
public:
	RCFreq rcFrequency() const;
	void setRcFrequency(RCFreq val);
};

/**
 * @brief Return RC Oscillator Frequency Select value
 * @return RC Oscillator Frequency Select value
 */
inline OscControl::RCFreq OscControl::rcFrequency() const
{
	return (RCFreq) REG_(rcctrl).rcfreq;
}
/**
 * @brief Set RC Oscillator Frequency Select value
 * @param[in] val New RC Oscillator Frequency Select value
 */
inline void OscControl::setRcFrequency(RCFreq val)
{
	REG_(rcctrl).rcfreq = val;
}

#endif /* VE_AVR_VE_RC_H_ */
