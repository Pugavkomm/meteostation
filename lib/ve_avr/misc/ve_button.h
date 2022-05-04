/**
 * @file ve_button.h
 * @brief Button Helper Functions Header
 * @author andrey
 * @date 11.05.2012
 * @copyright GNU Public License
 */
#ifndef BUTTON_H_
#define BUTTON_H_

/**
 * @defgroup Button Button
 * @ingroup Helpers
 * @brief Button classes
 *
 * @addtogroup Button
 * @{
 */

/**
 * @brief Button Connection type
 */
typedef enum {
    InstantON,							///< A press connects input to GND
    InstantOFF							///< A press connects input to VCC
} button_type_t;
typedef button_type_t* button_type_p;	///< Pointer to Button Connection type

/**
 * @brief Button structure type
 */
typedef struct _BUTTON {
    AVRP_GPIO       m_pGPIO;			///< Pointer to GPIO
    uint8_t         m_pin;				///< Pin number
    button_type_t   m_btnType;			///< Button Connection type
    bool            m_intPullup;		///< Internal pull-up on bit
} BUTTON;
typedef BUTTON* PBUTTON;				///< Pointer to Button structure type

/**
 * @brief Button class
 */
class Button : public BUTTON
{
public:
    void setup(GPIO& gpio, uint8_t pin, button_type_t btnType, bool bIntPullup = true);
    bool isPushed();
    bool isReleased();
    void waitPush();
    void waitRelease();
};
/**
 * @brief Setup Button
 * @param[in] gpio Reference to GPIO
 * @param[in] pin Pin number
 * @param[in] btnType Button Connection type
 * @param[in] bIntPullup Internal pull-up on bit
 */
inline void Button::setup(GPIO& gpio, uint8_t pin, button_type_t btnType, bool bIntPullup)
{
    m_pGPIO = &gpio;
    m_pin = pin;
    m_btnType = btnType;
    m_intPullup = bIntPullup;
    setModeInput(gpio, pin);
    if (m_btnType == InstantON && m_intPullup)
        pullupOn(gpio, pin);
    else
        pullupOff(gpio, pin);
}
/**
 * @brief Return true, if Button is pushed
 * @return True, if Button is pushed
 */
inline bool Button::isPushed()
{
    GPIO& ref = (GPIO&) (*m_pGPIO);
    return (m_btnType == InstantON) ? isLow(ref, m_pin) : isHigh(ref, m_pin);
}
/**
 * @brief Return true, if Button is released
 * @return True, if Button is released
 */
inline bool Button::isReleased()
{
    return ! isPushed();
}
/**
 * @brief Wait for Button push
 */
inline void Button::waitPush()
{
    while (isReleased());
}
/**
 * @brief Wait for Button release
 */
inline void Button::waitRelease()
{
    while (isPushed());
}
/**
 * @}
 */
#endif /* BUTTON_H_ */
