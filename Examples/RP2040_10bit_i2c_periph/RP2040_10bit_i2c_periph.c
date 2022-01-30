#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/irq.h"

#define I2C_PORT i2c0
#define I2C_SDA 8
#define I2C_SCL 9

uint8_t i2cResponse[3] = {0x5A, 0x00, 0x00};
uint8_t i = 0;

void i2c_irq_handler(void){
    uint32_t status = 0;
    uint8_t recdByte = 0;
    uint8_t nextByte = 0;
    uint8_t numToCharMSB;
    uint8_t numToCharLSB;
    uint8_t divisorMSB;
    uint8_t divisorLSB;

    status = i2c0->hw->intr_stat;
    if (status & (I2C_IC_INTR_STAT_R_RX_FULL_BITS | I2C_IC_INTR_STAT_R_RD_REQ_BITS)){
        recdByte = i2c0->hw->data_cmd;
        switch(recdByte){
            case 0xA5:
                while(!i2c_get_read_available(i2c0)){
                    printf("...........\n");
                }
                nextByte = i2c0->hw->data_cmd;
                if(nextByte == 0x5A){
                    i++;
                }
                divisorMSB = ((i >> 4) & 0xFF)%16;
                divisorLSB = ((i) & 0xFF)%16;
                if(divisorMSB >= 10){
                    numToCharMSB = divisorMSB + 0x37;
                }
                else{
                    numToCharMSB = divisorMSB + 0x30;
                }
                if(divisorLSB >= 10){
                    numToCharLSB = divisorLSB + 0x37;
                }
                else{
                    numToCharLSB = divisorLSB + 0x30;
                }
                i2cResponse[1] = numToCharMSB;
                i2cResponse[2] = numToCharLSB;
                //i2c_write_blocking works in slave mode because the TAR and SAR are separate.
                i2c_write_blocking_10bit(i2c0, 0x02CF, i2cResponse, 3, false);
        }
        i2c0->hw->clr_rd_req;
    }
}

int main()
{
    stdio_init_all();

    // I2C Initialisation. Using it at 100Khz with 10-bit addressing and sets the address to 0x02CF
    i2c_init_10bit(I2C_PORT, 100*1000);
    i2c_set_slave_mode_10bit(I2C_PORT, true, 0x02CF);

    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    I2C_PORT->hw->intr_mask = (I2C_IC_INTR_MASK_M_RD_REQ_BITS | I2C_IC_INTR_MASK_M_RX_FULL_BITS);
    irq_set_exclusive_handler(I2C0_IRQ, i2c_irq_handler);
    irq_set_enabled(I2C0_IRQ, true);

    while(1){

    }

    return 0;
}
