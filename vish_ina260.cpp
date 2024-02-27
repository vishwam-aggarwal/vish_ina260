#include "vish_ina260.h"

vish_INA260::vish_INA260(void) {}

/*!
 *    @brief  Sets up the HW
 *    @param  i2c_addr
 *            The I2C address to be used.
 *    @param  i2c_port
 *            The Wire object to be used for I2C connections.
 *    @return True if initialization was successful, otherwise false.
 */
bool vish_INA260::begin(
    uint8_t i2c_addr,
    i2c_inst_t* i2c_port,
    uint speed,
    uint8_t I2C_SDA,
    uint8_t I2C_SCL,
    bool PullUp_Enable)
{
    _i2c_addr = i2c_addr;
    _speed = speed;
    _I2C_SDA = I2C_SDA;
    _I2C_SCL = I2C_SCL;
    _PullUp_Enable = PullUp_Enable;

    INA260_I2C = new vish_i2cdev;
    INA260_I2C->begin(&_i2c_addr, i2c_port, &_speed, &_I2C_SDA, &_I2C_SCL, &_PullUp_Enable);

  vish_BusIO_Register *die_register = new vish_BusIO_Register(INA260_I2C, INA260_REG_DIE_UID, 2, MSBFIRST);
  vish_BusIO_Register *mfg_register = new vish_BusIO_Register(INA260_I2C, INA260_REG_MFG_UID, 2, MSBFIRST);
  vish_BusIO_RegisterBits *device_id = new vish_BusIO_RegisterBits(die_register, 12, 4);

  // make sure we're talking to the right chip
  if ((mfg_register->read() != 0x5449) || (device_id->read() != 0x227)) {
    return false;
  }

  Config = new vish_BusIO_Register(INA260_I2C, INA260_REG_CONFIG, 2, MSBFIRST, 1, LSBFIRST);
  MaskEnable = new vish_BusIO_Register(INA260_I2C, INA260_REG_MASK_ENABLE, 2, MSBFIRST);
  AlertLimit = new vish_BusIO_Register(INA260_I2C, INA260_REG_ALERT_LIMIT, 2, MSBFIRST);

  reset();
  sleep_ms(2); // delay 2ms to give time for first measurement to finish
  return true;
}
/**************************************************************************/
/*!
    @brief Resets the harware. All registers are set to default values,
    the same as a power-on reset.
*/
/**************************************************************************/
void vish_INA260::reset(void) {
  vish_BusIO_RegisterBits reset = vish_BusIO_RegisterBits(Config, 1, 15);
  reset.write(1);
}
/**************************************************************************/
/*!
    @brief Reads and scales the current value of the Current register.
    @return The current current measurement in mA
*/
/**************************************************************************/
float vish_INA260::readCurrent(void) {
  vish_BusIO_Register current = vish_BusIO_Register(INA260_I2C, INA260_REG_CURRENT, 2, MSBFIRST);
  return (int16_t)current.read() * 1.25;
}
/**************************************************************************/
/*!
    @brief Reads and scales the current value of the Bus Voltage register.
    @return The current bus voltage measurement in mV
*/
/**************************************************************************/
float vish_INA260::readBusVoltage(void) {
  vish_BusIO_Register bus_voltage = vish_BusIO_Register(INA260_I2C, INA260_REG_BUSVOLTAGE, 2, MSBFIRST);
  return bus_voltage.read() * 1.25;
}
/**************************************************************************/
/*!
    @brief Reads and scales the current value of the Power register.
    @return The current Power calculation in mW
*/
/**************************************************************************/
float vish_INA260::readPower(void) {
  vish_BusIO_Register power = vish_BusIO_Register(INA260_I2C, INA260_REG_POWER, 2, MSBFIRST);
  return power.read() * 10;
}
/**************************************************************************/
/*!
    @brief Returns the current measurement mode
    @return The current mode
*/
/**************************************************************************/
INA260_MeasurementMode vish_INA260::getMode(void) {
  vish_BusIO_RegisterBits mode = vish_BusIO_RegisterBits(Config, 3, 0);
  return (INA260_MeasurementMode)mode.read();
}
/**************************************************************************/
/*!
    @brief Sets a new measurement mode
    @param new_mode
           The new mode to be set
*/
/**************************************************************************/
void vish_INA260::setMode(INA260_MeasurementMode new_mode) {
  vish_BusIO_RegisterBits mode = vish_BusIO_RegisterBits(Config, 3, 0);
  mode.write(new_mode);
}
/**************************************************************************/
/*!
    @brief Reads the current number of averaging samples
    @return The current number of averaging samples
*/
/**************************************************************************/
INA260_AveragingCount vish_INA260::getAveragingCount(void) {
  vish_BusIO_RegisterBits averaging_count = vish_BusIO_RegisterBits(Config, 3, 9);
  return (INA260_AveragingCount)averaging_count.read();
}
/**************************************************************************/
/*!
    @brief Sets the number of averaging samples
    @param count
           The number of samples to be averaged
*/
/**************************************************************************/
void vish_INA260::setAveragingCount(INA260_AveragingCount count) {
  vish_BusIO_RegisterBits averaging_count = vish_BusIO_RegisterBits(Config, 3, 9);
  averaging_count.write(count);
}
/**************************************************************************/
/*!
    @brief Reads the current current conversion time
    @return The current current conversion time
*/
/**************************************************************************/
INA260_ConversionTime vish_INA260::getCurrentConversionTime(void) {
  vish_BusIO_RegisterBits current_conversion_time = vish_BusIO_RegisterBits(Config, 3, 3);
  return (INA260_ConversionTime)current_conversion_time.read();
}
/**************************************************************************/
/*!
    @brief Sets the current conversion time
    @param time
           The new current conversion time
*/
/**************************************************************************/
void vish_INA260::setCurrentConversionTime(INA260_ConversionTime time) {
  vish_BusIO_RegisterBits current_conversion_time = vish_BusIO_RegisterBits(Config, 3, 3);
  current_conversion_time.write(time);
}
/**************************************************************************/
/*!
    @brief Reads the current bus voltage conversion time
    @return The current bus voltage conversion time
*/
/**************************************************************************/
INA260_ConversionTime vish_INA260::getVoltageConversionTime(void) {
  vish_BusIO_RegisterBits voltage_conversion_time = vish_BusIO_RegisterBits(Config, 3, 6);
  return (INA260_ConversionTime)voltage_conversion_time.read();
}
/**************************************************************************/
/*!
    @brief Sets the bus voltage conversion time
    @param time
           The new bus voltage conversion time
*/
/**************************************************************************/
void vish_INA260::setVoltageConversionTime(INA260_ConversionTime time) {
  vish_BusIO_RegisterBits voltage_conversion_time = vish_BusIO_RegisterBits(Config, 3, 6);
  voltage_conversion_time.write(time);
}
/**************************************************************************/
/*!
    @brief Checks if the most recent one shot measurement has completed
    @return true if the conversion has completed
*/
/**************************************************************************/
bool vish_INA260::conversionReady(void) {
  vish_BusIO_RegisterBits conversion_ready = vish_BusIO_RegisterBits(MaskEnable, 1, 3);
  return conversion_ready.read();
}
/**************************************************************************/
/*!
    @brief Reads the current parameter that asserts the ALERT pin
    @return The current parameter that asserts the ALERT PIN
*/
/**************************************************************************/
INA260_AlertType vish_INA260::getAlertType(void) {
  vish_BusIO_RegisterBits alert_type = vish_BusIO_RegisterBits(MaskEnable, 6, 10);
  return (INA260_AlertType)alert_type.read();
}
/**************************************************************************/
/*!
    @brief Sets which parameter asserts the ALERT pin
    @param alert
           The parameter which asserts the ALERT pin
*/
/**************************************************************************/
void vish_INA260::setAlertType(INA260_AlertType alert) {
  vish_BusIO_RegisterBits alert_type = vish_BusIO_RegisterBits(MaskEnable, 6, 10);
  alert_type.write(alert);
}
/**************************************************************************/
/*!
    @brief Reads the current alert limit setting
    @return The current bus alert limit setting
*/
/**************************************************************************/
float vish_INA260::getAlertLimit(void) {
  vish_BusIO_RegisterBits alert_limit = vish_BusIO_RegisterBits(AlertLimit, 16, 0);
  return (float)alert_limit.read() * 1.25;
}
/**************************************************************************/
/*!
    @brief Sets the Alert Limit
    @param limit
           The new limit that triggers the alert
*/
/**************************************************************************/
void vish_INA260::setAlertLimit(float limit) {
  vish_BusIO_RegisterBits alert_limit = vish_BusIO_RegisterBits(AlertLimit, 16, 0);
  alert_limit.write((int16_t)(limit / 1.25));
}
/**************************************************************************/
/*!
    @brief Reads the current alert polarity setting
    @return The current bus alert polarity setting
*/
/**************************************************************************/
INA260_AlertPolarity vish_INA260::getAlertPolarity(void) {
  vish_BusIO_RegisterBits alert_polarity = vish_BusIO_RegisterBits(MaskEnable, 1, 1);
  return (INA260_AlertPolarity)alert_polarity.read();
}
/**************************************************************************/
/*!
    @brief Sets Alert Polarity Bit
    @param polarity
           The polarity of the alert pin
*/
/**************************************************************************/
void vish_INA260::setAlertPolarity(INA260_AlertPolarity polarity) {
  vish_BusIO_RegisterBits alert_polarity = vish_BusIO_RegisterBits(MaskEnable, 1, 1);
  alert_polarity.write(polarity);
}
/**************************************************************************/
/*!
    @brief Reads the current alert latch setting
    @return The current bus alert latch setting
*/
/**************************************************************************/
INA260_AlertLatch vish_INA260::getAlertLatch(void) {
  vish_BusIO_RegisterBits alert_latch = vish_BusIO_RegisterBits(MaskEnable, 1, 0);
  return (INA260_AlertLatch)alert_latch.read();
}
/**************************************************************************/
/*!
    @brief Sets Alert Latch Bit
    @param state
           The parameter which asserts the ALERT pin
*/
/**************************************************************************/
void vish_INA260::setAlertLatch(INA260_AlertLatch state) {
  vish_BusIO_RegisterBits alert_latch = vish_BusIO_RegisterBits(MaskEnable, 1, 0);
  alert_latch.write(state);
}
/**************************************************************************/
/*!
    @brief Checks if the Alert Flag is set
    @return true if the flag is set
*/
/**************************************************************************/
bool vish_INA260::alertFunctionFlag(void) {
  vish_BusIO_RegisterBits alert_function_flag = vish_BusIO_RegisterBits(MaskEnable, 1, 4);
  return alert_function_flag.read();
}