#include "APDS9960.h"

bool Init()
{
    uint8_t id;

    /* Initialize I2C */
    BSP_I2C_Init((uint8_t)APDS9960_I2C_ADDR<<1);
    uint8_t data[2];
    /* Read ID register and check against known values for APDS-9960 */
    if( !BSP_I2C_ReadRegister((uint8_t)0x92, &data[0]) )
        return false;

    if( !(data[0] == APDS9960_ID_1 ) ) //|| data[0] == APDS9960_ID_2) ) {
    	return false;

    // Set ENABLE register to 0 (disable all features)
    if(!setMode(ALL, OFF)) return false;

    /* Set default values for ambient light and proximity registers */
    if( !BSP_I2C_WriteRegister((uint8_t)APDS9960_ATIME, (uint8_t)DEFAULT_ATIME) ) {
        return false;
    }

    if( !BSP_I2C_WriteRegister((uint8_t)APDS9960_WTIME, (uint8_t)DEFAULT_WTIME) ) {
        return false;
    }

    if( !BSP_I2C_WriteRegister((uint8_t)APDS9960_PPULSE, (uint8_t)DEFAULT_PROX_PPULSE) ) {
        return false;
    }

    if( !BSP_I2C_WriteRegister((uint8_t)APDS9960_POFFSET_UR, (uint8_t)DEFAULT_POFFSET_UR) ) {
        return false;
    }

    if( !BSP_I2C_WriteRegister((uint8_t)APDS9960_POFFSET_DL, (uint8_t)DEFAULT_POFFSET_DL) ) {
        return false;
    }

    if( !BSP_I2C_WriteRegister((uint8_t)APDS9960_CONFIG1, (uint8_t)DEFAULT_CONFIG1) ) {
        return false;
    }

    if( !setLEDDrive(DEFAULT_LDRIVE) ) {
        return false;
    }
    if( !setProximityGain(DEFAULT_PGAIN) ) {
        return false;
    }
    if( !setAmbientLightGain(DEFAULT_AGAIN) ) {
        return false;
    }

    if( !BSP_I2C_WriteRegister((uint8_t)APDS9960_PILT, (uint8_t)DEFAULT_PILT) ) {
        return false;
    }

    if( !BSP_I2C_WriteRegister((uint8_t)APDS9960_PIHT, (uint8_t)DEFAULT_PIHT) ) {
        return false;
    }

    if( !setLightIntLowThreshold(DEFAULT_AILT) ) {
        return false;
    }
    if( !setLightIntHighThreshold(DEFAULT_AIHT) ) {
        return false;
    }

    if( !BSP_I2C_WriteRegister((uint8_t)APDS9960_PERS, (uint8_t)DEFAULT_PERS) ) {
        return false;
    }

    if( !BSP_I2C_WriteRegister((uint8_t)APDS9960_CONFIG2, (uint8_t)DEFAULT_CONFIG2) ) {
        return false;
    }

    if( !BSP_I2C_WriteRegister((uint8_t)APDS9960_CONFIG3, (uint8_t)DEFAULT_CONFIG3) ) {
        return false;
    }

    return true;
}

bool enableLightSensor(bool interrupts)
{
    if(!setAmbientLightGain(DEFAULT_AGAIN))
        return false;

    if( interrupts ) {
        if( !setAmbientLightIntEnable(1) )
            return false;
    }
    else {
        if( !setAmbientLightIntEnable(0) )
            return false;
    }
    if(!enablePower())
        return false;


    if(!setMode(AMBIENT_LIGHT, 1))
        return false;

    return true;
}

bool enablePower()
{
    if( !setMode(POWER, 1) ) {
        return false;
    }

    return true;
}

bool setMode(uint8_t mode, uint8_t enable)
{
    uint8_t reg_val;

    /* Read current ENABLE register */
    reg_val = getMode();
    if( reg_val == ERROR ) {
        return false;
    }

    /* Change bit(s) in ENABLE register */
    enable = enable & 0x01;
    if( ( mode >= 0  ) && ( mode <= 6 )) {
        if (enable) {
            reg_val |= (1 << mode);
        } else {
            reg_val &= ~(1 << mode);
        }
    } else if( mode == ALL ) {
        if (enable) {
            reg_val = 0x7F;
        } else {
            reg_val = 0x00;
        }
    }

    /* Write value back to ENABLE register */
    if( !BSP_I2C_WriteRegister(APDS9960_ENABLE, reg_val) ) {
        return false;
    }

    return true;
}

uint8_t getMode()
{
    uint8_t enable_value;

    /* Read current ENABLE register */
    if( !BSP_I2C_ReadRegister(APDS9960_ENABLE, &enable_value) ) {
        return ERROR;
    }

    return enable_value;
}

bool setProximityGain(uint8_t drive)
{
    uint8_t val;

    if( !BSP_I2C_ReadRegister((uint8_t)APDS9960_CONTROL, &val) ) {
        return false;
    }

    /* Set bits in register to given value */
    drive &= 0x03;
    drive = drive << (uint8_t)6;
    val &= 0x3F;
    val |= drive;

    /* Write register value back into CONTROL register */
    if( !BSP_I2C_WriteRegister((uint8_t)APDS9960_CONTROL, (uint8_t)val) ) {
        return false;
    }

    return true;
}

bool setLEDDrive(uint8_t drive)
{
    uint8_t val;

    if( !BSP_I2C_ReadRegister((uint8_t)APDS9960_CONTROL, &val) ) {
        return false;
    }

    /* Set bits in register to given value */
    drive &= 0x03;
    drive = drive << 2;
    val &= 0xF3;
    val |= drive;

    /* Write register value back into CONTROL register */
    if( !BSP_I2C_WriteRegister((uint8_t)APDS9960_CONTROL, (uint8_t)val) ) {
        return false;
    }

    return true;
}

bool setAmbientLightGain(uint8_t drive)
{
    uint8_t val;

    if( !BSP_I2C_ReadRegister((uint8_t)APDS9960_CONTROL, &val) ) {
        return false;
    }

    /* Set bits in register to given value */
    drive = 0x03;
    val = 0xFC;
    val |= drive;

    /* Write register value back into CONTROL register */
    if( !BSP_I2C_WriteRegister((uint8_t)APDS9960_CONTROL, (uint8_t)val) ) {
        return false;
    }

    return true;
}

bool setLightIntLowThreshold(uint16_t threshold)
{
    uint8_t val_low;
    uint8_t val_high;

    uint8_t* temp = (void*)&threshold;

    /* Break 16-bit threshold into 2 8-bit values */
    val_low = temp[1];
    val_high = temp[0];

    /* Write low byte */
    if( !BSP_I2C_WriteRegister((uint8_t)APDS9960_AILTL, (uint8_t)val_low) ) {
		return false;
	}

    /* Write high byte */
    if( !BSP_I2C_WriteRegister((uint8_t)APDS9960_AILTH, (uint8_t)val_high) ) {
		return false;
	}

    return true;
}

bool setLightIntHighThreshold(uint16_t threshold)
{
    uint8_t val_low;
    uint8_t val_high;

    /* Break 16-bit threshold into 2 8-bit values */
    uint8_t* temp = (void*)&threshold;
    val_low = temp[1];
    val_high = temp[0];

    /* Write low byte */
    if( !BSP_I2C_WriteRegister((uint8_t)APDS9960_AIHTL, (uint8_t)val_low) ) {
		return false;
	}

    /* Write high byte */
    if( !BSP_I2C_WriteRegister((uint8_t)APDS9960_AIHTH, (uint8_t)val_high) ) {
		return false;
	}

    return true;
}

bool setAmbientLightIntEnable(uint8_t enable)
{
    uint8_t val;

    /* Read value from ENABLE register */
    if( !BSP_I2C_ReadRegister((uint8_t)APDS9960_ENABLE, &val) ) {
        return false;
    }

    /* Set bits in register to given value */
    enable &= 0x01;
    enable = enable << 4;
    val &= 0xEF;
    val |= enable;

    /* Write register value back into ENABLE register */
    if( !BSP_I2C_WriteRegister((uint8_t)APDS9960_ENABLE, (uint8_t)val) ) {
		return false;
	}

    return true;
}

bool readAmbientLight(uint16_t* val)
{
    uint8_t val_byte;
    val[0] = 0;

    /* Read value from clear channel, low byte register */
    if( !BSP_I2C_ReadRegister((uint8_t)APDS9960_CDATAL, &val_byte) ) {
        return false;
    }
    val[0] = val_byte;

    /* Read value from clear channel, high byte register */
    if( !BSP_I2C_ReadRegister((uint8_t)APDS9960_CDATAH, &val_byte) ) {
        return false;
    }
    val[0] = val[0] + ((uint16_t)val_byte << 8);

    return true;
}

bool readRedLight(uint16_t* val)
{
    uint8_t val_byte;
    val[0] = 0;

    /* Read value from clear channel, low byte register */
    if( !BSP_I2C_ReadRegister((uint8_t)APDS9960_RDATAL, &val_byte) ) {
        return false;
    }
    val[0] = val_byte;

    /* Read value from clear channel, high byte register */
    if( !BSP_I2C_ReadRegister((uint8_t)APDS9960_RDATAH, &val_byte) ) {
        return false;
    }
    val[0] = val[0] + ((uint16_t)val_byte << 8);

    return true;
}

bool readGreenLight(uint16_t* val)
{
    uint8_t val_byte;
    val[0] = 0;

    /* Read value from clear channel, low byte register */
    if( !BSP_I2C_ReadRegister((uint8_t)APDS9960_GDATAL, &val_byte) ) {
        return false;
    }
    val[0] = val_byte;

    /* Read value from clear channel, high byte register */
    if( !BSP_I2C_ReadRegister((uint8_t)APDS9960_GDATAH, &val_byte) ) {
        return false;
    }
    val[0] = val[0] + ((uint16_t)val_byte << 8);

    return true;
}

bool readBlueLight(uint16_t* val)
{
    uint8_t val_byte;
    val[0] = 0;

    /* Read value from clear channel, low byte register */
    if( !BSP_I2C_ReadRegister((uint8_t)APDS9960_BDATAL, &val_byte) ) {
        return false;
    }
    val[0] = val_byte;

    /* Read value from clear channel, high byte register */
    if( !BSP_I2C_ReadRegister((uint8_t)APDS9960_BDATAH, &val_byte) ) {
        return false;
    }
    val[0] = val[0] + ((uint16_t)val_byte << 8);

    return true;
}

/*
bool readRedLight(uint16_t val)
{
    uint8_t val_byte;
    val = 0;

    if( !wireReadDataByte(APDS9960_RDATAL, val_byte) ) {
        return false;
    }
    val = val_byte;

    if( !wireReadDataByte(APDS9960_RDATAH, val_byte) ) {
        return false;
    }
    val = val + ((uint16_t)val_byte << 8);

    return true;
}
 */
