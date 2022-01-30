#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c0
#define I2C_SDA 8
#define I2C_SCL 9



int main()
{
    uint8_t messageToTransmit[2] = {0xA5, 0x5A};
    char receivedMessage[3];
    stdio_init_all();

    // I2C Initialisation. Using it at 100Khz with 10-bit addressing
    i2c_init_10bit(I2C_PORT, 100*1000);

    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    printf("Hello!!\n");
    sleep_ms(1000);
    while(1){
        printf("Transmitting..\n");
        i2c_write_blocking_10bit(I2C_PORT, 0x02CF, messageToTransmit, 2, true);
        printf("Receiving..\n");
        i2c_read_blocking_10bit(I2C_PORT, 0x02CF, receivedMessage, 3, false);

        printf("Received: %s\n", receivedMessage);
        sleep_ms(1000);
    }

    return 0;
}
