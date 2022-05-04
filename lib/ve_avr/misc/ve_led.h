/**
 * @file ve_led.h
 * @brief LED Helper Functions Header
 * @author andrey
 * @date 11.05.2012
 * @copyright GNU Public License
 */
#ifndef LED_H_
#define LED_H_

/**
 * @defgroup LED LED
 * @ingroup Helpers
 * @brief LED classes
 *
 * @addtogroup LED
 * @{
 */

/**
 * @brief LED functions
 */
namespace led
{

void setup(GPIO& gpio, uint8_t pin);
void on(GPIO& gpio, uint8_t pin, uint16_t delay = 0);
void off(GPIO& gpio, uint8_t pin, uint16_t delay = 0);
bool isLit(const GPIO& gpio, uint8_t pin);

}
/**
 * @brief Setup LED
 * @param[in] gpio Reference to GPIO
 * @param[in] pin Pin number
 */
inline void led::setup(GPIO& gpio, uint8_t pin)
{
    setModeOutput(gpio, pin);
}
/**
 * @brief Turn LED on and wait for a delay time out
 * @param[in] gpio Reference to GPIO
 * @param[in] pin Pin number
 * @param[in] delay Delay time in milliseconds
 */
inline void led::on(GPIO& gpio, uint8_t pin, uint16_t delay)
{
    setHigh(gpio, pin);
    _delay_ms(delay);
}
/**
 * @brief Turn LED off and wait for a delay time out
 * @param[in] gpio Reference to GPIO
 * @param[in] pin Pin number
 * @param[in] delay Delay time in milliseconds
 */
inline void led::off(GPIO& gpio, uint8_t pin, uint16_t delay)
{
    setLow(gpio, pin);
    _delay_ms(delay);
}
/**
 * @brief Return true, if LED is lit
 * @param[in] gpio Reference to GPIO
 * @param[in] pin Pin number
 * @return True, if LED is lit
 */
bool led::isLit(const GPIO& gpio, uint8_t pin)
{
	return (REG(gpio.ddr) & _BV(pin));
}
/**
 * @}
 */
#endif /* LED_H_ */
