
#include <stdint.h>

void i2c_init();
int i2c_start(); ///< Return 1 if found
int i2c_sendAdress(int8_t addr);
int i2c_write(int8_t byte);
int i2c_stop();
