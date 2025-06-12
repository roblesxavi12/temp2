// https://github.com/sparkfun/SparkFun_APDS-9960_Sensor_Arduino_Library/blob/master/src/SparkFun_APDS9960.cpp
#include "APDS9960.h"
#include "math.h"

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

    /* Set default values for gesture sense registers */
    if( !setGestureEnterThresh(DEFAULT_GPENTH) ) {
        return false;
    }
    if( !setGestureExitThresh(DEFAULT_GEXTH) ) {
        return false;
    }
    if( !BSP_I2C_WriteRegister((uint8_t)APDS9960_GCONF1, DEFAULT_GCONF1) ) {
        return false;
    }
    if( !setGestureGain(DEFAULT_GGAIN) ) {
        return false;
    }
    if( !setGestureLEDDrive(DEFAULT_GLDRIVE) ) {
        return false;
    }
    if( !setGestureWaitTime(DEFAULT_GWTIME) ) {
        return false;
    }
    if( !BSP_I2C_WriteRegister((uint8_t)APDS9960_GOFFSET_U, DEFAULT_GOFFSET) ) {
        return false;
    }
    if( !BSP_I2C_WriteRegister((uint8_t)APDS9960_GOFFSET_D, DEFAULT_GOFFSET) ) {
        return false;
    }
    if( !BSP_I2C_WriteRegister((uint8_t)APDS9960_GOFFSET_L, DEFAULT_GOFFSET) ) {
        return false;
    }
    if( !BSP_I2C_WriteRegister((uint8_t)APDS9960_GOFFSET_R, DEFAULT_GOFFSET) ) {
        return false;
    }
    if( !BSP_I2C_WriteRegister((uint8_t)APDS9960_GPULSE, DEFAULT_GPULSE) ) {
        return false;
    }
    if( !BSP_I2C_WriteRegister((uint8_t)APDS9960_GCONF3, DEFAULT_GCONF3) ) {
        return false;
    }
    if( !BSP_I2C_WriteRegister((uint8_t)DEFAULT_GIEN) ) {
        return false;
    }
    
#if 0
    /* Gesture config register dump */
    uint8_t reg;
    uint8_t val;
  
    for(reg = 0x80; reg <= 0xAF; reg++) {
        if( (reg != 0x82) && \
            (reg != 0x8A) && \
            (reg != 0x91) && \
            (reg != 0xA8) && \
            (reg != 0xAC) && \
            (reg != 0xAD) )
        {
            BSP_I2C_ReadRegister((uint8_t)reg, val);
            // Serial.print(reg, HEX);
            // Serial.print(": 0x");
            // Serial.println(val, HEX);
        }
    }

    for(reg = 0xE4; reg <= 0xE7; reg++) {
        BSP_I2C_ReadRegister((uint8_t)reg, val);
        // Serial.print(reg, HEX);
        // Serial.print(": 0x");
        // Serial.println(val, HEX);
    }
#endif

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

/* --- A PARTIR DE AQUI ES SENSOR DE GESTOS --- */

void resetGestureParameters()
{
    gesture_data_.index = 0;
    gesture_data_.total_gestures = 0;
    
    gesture_ud_delta_ = 0;
    gesture_lr_delta_ = 0;
    
    gesture_ud_count_ = 0;
    gesture_lr_count_ = 0;
    
    gesture_near_count_ = 0;
    gesture_far_count_ = 0;
    
    gesture_state_ = 0;
    gesture_motion_ = DIR_NONE;
}

bool enableGestureSensor(bool interrupts)
{
    
    /* Enable gesture mode
       Set ENABLE to 0 (power off)
       Set WTIME to 0xFF
       Set AUX to LED_BOOST_300
       Enable PON, WEN, PEN, GEN in ENABLE 
    */
    resetGestureParameters();
    if( !BSP_I2C_WriteRegister((uint8_t)APDS9960_WTIME, 0xFF) ) { //BSP_I2C_WriteRegister
        return false;
    }
    if( !BSP_I2C_WriteRegister((uint8_t)APDS9960_PPULSE, DEFAULT_GESTURE_PPULSE) ) { // BSP_I2C_WriteRegister
        return false;
    }
    if( !setLEDBoost(LED_BOOST_300) ) {
        return false;
    }
    if( interrupts ) {
        if( !setGestureIntEnable(1) ) {
            return false;
        }
    } else {
        if( !setGestureIntEnable(0) ) {
            return false;
        }
    }
    if( !setGestureMode(1) ) {
        return false;
    }
    if( !enablePower() ){
        return false;
    }
    if( !setMode(WAIT, 1) ) {
        return false;
    }
    if( !setMode(PROXIMITY, 1) ) {
        return false;
    }
    if( !setMode(GESTURE, 1) ) {
        return false;
    }
    
    return true;
}

bool setLEDBoost(uint8_t boost)
{
    uint8_t val;
    
    /* Read value from CONFIG2 register */
    if( !BSP_I2C_ReadRegister((uint8_t)APDS9960_CONFIG2, val) ) {
        return false;
    }
    
    /* Set bits in register to given value */
    boost &= 0b00000011;
    boost = boost << 4;
    val &= 0b11001111;
    val |= boost;
    
    /* Write register value back into CONFIG2 register */
    if( !BSP_I2C_WriteRegister((uint8_t)APDS9960_CONFIG2, val) ) {
        return false;
    }
    
    return true;
}

bool setGestureIntEnable(uint8_t enable)
{
    uint8_t val;
    
    /* Read value from GCONF4 register */
    if( !BSP_I2C_ReadRegister((uint8_t)APDS9960_GCONF4, val) ) {
        return false;
    }
    
    /* Set bits in register to given value */
    enable &= 0b00000001;
    enable = enable << 1;
    val &= 0b11111101;
    val |= enable;
    
    /* Write register value back into GCONF4 register */
    if( !BSP_I2C_WriteRegister((uint8_t)APDS9960_GCONF4, val) ) {
        return false;
    }
    
    return true;
}

bool setGestureMode(uint8_t mode)
{
    uint8_t val;
    
    /* Read value from GCONF4 register */
    if( !BSP_I2C_ReadRegister((uint8_t)APDS9960_GCONF4, val) ) {
        return false;
    }
    
    /* Set bits in register to given value */
    mode &= 0b00000001;
    val &= 0b11111110;
    val |= mode;
    
    /* Write register value back into GCONF4 register */
    if( !BSP_I2C_WriteRegister((uint8_t)APDS9960_GCONF4, val) ) {
        return false;
    }
    
    return true;
}

bool setGestureEnterThresh(uint8_t threshold)
{
    if( !BSP_I2C_WriteRegister((uint8_t)APDS9960_GPENTH, threshold) ) {
        return false;
    }
    
    return true;
}

bool setGestureExitThresh(uint8_t threshold)
{
    if( !BSP_I2C_WriteRegister((uint8_t)APDS9960_GEXTH, threshold) ) {
        return false;
    }
    
    return true;
}

bool setGestureGain(uint8_t gain)
{
    uint8_t val;
    
    /* Read value from GCONF2 register */
    if( !BSP_I2C_ReadRegister((uint8_t)APDS9960_GCONF2, val) ) {
        return false;
    }
    
    /* Set bits in register to given value */
    gain &= 0b00000011;
    gain = gain << 5;
    val &= 0b10011111;
    val |= gain;
    
    /* Write register value back into GCONF2 register */
    if( !BSP_I2C_WriteRegister((uint8_t)APDS9960_GCONF2, val) ) {
        return false;
    }
    
    return true;
}

bool setGestureLEDDrive(uint8_t drive)
{
    uint8_t val;
    
    /* Read value from GCONF2 register */
    if( !BSP_I2C_ReadRegister((uint8_t)APDS9960_GCONF2, val) ) {
        return false;
    }
    
    /* Set bits in register to given value */
    drive &= 0b00000011;
    drive = drive << 3;
    val &= 0b11100111;
    val |= drive;
    
    /* Write register value back into GCONF2 register */
    if( !BSP_I2C_WriteRegister((uint8_t)APDS9960_GCONF2, val) ) {
        return false;
    }
    
    return true;
}

bool setGestureWaitTime(uint8_t time)
{
    uint8_t val;
    
    /* Read value from GCONF2 register */
    if( !BSP_I2C_ReadRegister((uint8_t)APDS9960_GCONF2, val) ) {
        return false;
    }
    
    /* Set bits in register to given value */
    time &= 0b00000111;
    val &= 0b11111000;
    val |= time;
    
    /* Write register value back into GCONF2 register */
    if( !BSP_I2C_WriteRegister((uint8_t)APDS9960_GCONF2, val) ) {
        return false;
    }
    
    return true;
}

bool isGestureAvailable()
{
    uint8_t val;
    
    /* Read value from GSTATUS register */
    if( !BSP_I2C_ReadRegister((uint8_t)APDS9960_GSTATUS, val) ) {
        return ERROR;
    }
    
    /* Shift and mask out GVALID bit */
    val &= APDS9960_GVALID;
    
    /* Return true/false based on GVALID bit */
    if( val == 1) {
        return true;
    } else {
        return false;
    }
}

int readGesture()
{
    uint8_t fifo_level = 0;
    uint8_t bytes_read = 0;
    uint8_t fifo_data[128];
    uint8_t gstatus;
    int motion;
    int i;
    
    /* Make sure that power and gesture is on and data is valid */
    if( !isGestureAvailable() || !(getMode() & 0b01000001) ) {
        return DIR_NONE;
    }
    
    /* Keep looping as long as gesture data is valid */
    while(1) {
    
        /* Wait some time to collect next batch of FIFO data */
        // delay(FIFO_PAUSE_TIME);
        
        volatile long i;
        int miliseconds = FIFO_PAUSE_TIME;
        while (milliseconds-- > 0)
            for (i = 0; i < 1000; i++);
        
        /* Get the contents of the STATUS register. Is data still valid? */
        if( !BSP_I2C_ReadRegister((uint8_t)APDS9960_GSTATUS, gstatus) ) {
            return ERROR;
        }
        
        /* If we have valid data, read in FIFO */
        if( (gstatus & APDS9960_GVALID) == APDS9960_GVALID ) {
        
            /* Read the current FIFO level */
            if( !BSP_I2C_ReadRegister((uint8_t)APDS9960_GFLVL, fifo_level) ) {
                return ERROR;
            }

#if DEBUG
            /* me da palo traducir los prints
            
            Serial.print("FIFO Level: ");
            Serial.println(fifo_level);
            */
#endif

            /* If there's stuff in the FIFO, read it into our data block */
            if( fifo_level > 0) {
                bytes_read = BSP_I2C_ReadBlock(  APDS9960_GFIFO_U, 
                                                (uint8_t*)fifo_data, 
                                                (fifo_level * 4) );
                if( bytes_read == -1 ) {
                    return ERROR;
                }
#if DEBUG
                /* Me da palo traducir los prints

                Serial.print("FIFO Dump: ");
                for ( i = 0; i < bytes_read; i++ ) {
                    Serial.print(fifo_data[i]);
                    Serial.print(" ");
                }
                Serial.println();
                */
#endif

                /* If at least 1 set of data, sort the data into U/D/L/R */
                if( bytes_read >= 4 ) {
                    for( i = 0; i < bytes_read; i += 4 ) {
                        gesture_data_.u_data[gesture_data_.index] = \
                                                            fifo_data[i + 0];
                        gesture_data_.d_data[gesture_data_.index] = \
                                                            fifo_data[i + 1];
                        gesture_data_.l_data[gesture_data_.index] = \
                                                            fifo_data[i + 2];
                        gesture_data_.r_data[gesture_data_.index] = \
                                                            fifo_data[i + 3];
                        gesture_data_.index++;
                        gesture_data_.total_gestures++;
                    }
                    
#if DEBUG
                /* me da palo traducir los prints
                
                Serial.print("Up Data: ");
                for ( i = 0; i < gesture_data_.total_gestures; i++ ) {
                    Serial.print(gesture_data_.u_data[i]);
                    Serial.print(" ");
                }
                Serial.println();
                */
#endif

                    /* Filter and process gesture data. Decode near/far state */
                    if( processGestureData() ) {
                        if( decodeGesture() ) {
                            //***TODO: U-Turn Gestures
#if DEBUG
                            //Serial.println(gesture_motion_);
#endif
                        }
                    }
                    
                    /* Reset data */
                    gesture_data_.index = 0;
                    gesture_data_.total_gestures = 0;
                }
            }
        } else {
    
            /* Determine best guessed gesture and clean up */
            // delay(FIFO_PAUSE_TIME);
            volatile long i;
            int miliseconds = FIFO_PAUSE_TIME;
            while (milliseconds-- > 0)
                for (i = 0; i < 1000; i++);
            decodeGesture();
            motion = gesture_motion_;
#if DEBUG
            /* me da palo traducir los prints
            
            Serial.print("END: ");
            Serial.println(gesture_motion_);
            */
#endif
            resetGestureParameters();
            return motion;
        }
    }
}

bool processGestureData()
{
    uint8_t u_first = 0;
    uint8_t d_first = 0;
    uint8_t l_first = 0;
    uint8_t r_first = 0;
    uint8_t u_last = 0;
    uint8_t d_last = 0;
    uint8_t l_last = 0;
    uint8_t r_last = 0;
    int ud_ratio_first;
    int lr_ratio_first;
    int ud_ratio_last;
    int lr_ratio_last;
    int ud_delta;
    int lr_delta;
    int i;

    /* If we have less than 4 total gestures, that's not enough */
    if( gesture_data_.total_gestures <= 4 ) {
        return false;
    }
    
    /* Check to make sure our data isn't out of bounds */
    if( (gesture_data_.total_gestures <= 32) && \
        (gesture_data_.total_gestures > 0) ) {
        
        /* Find the first value in U/D/L/R above the threshold */
        for( i = 0; i < gesture_data_.total_gestures; i++ ) {
            if( (gesture_data_.u_data[i] > GESTURE_THRESHOLD_OUT) &&
                (gesture_data_.d_data[i] > GESTURE_THRESHOLD_OUT) &&
                (gesture_data_.l_data[i] > GESTURE_THRESHOLD_OUT) &&
                (gesture_data_.r_data[i] > GESTURE_THRESHOLD_OUT) ) {
                
                u_first = gesture_data_.u_data[i];
                d_first = gesture_data_.d_data[i];
                l_first = gesture_data_.l_data[i];
                r_first = gesture_data_.r_data[i];
                break;
            }
        }
        
        /* If one of the _first values is 0, then there is no good data */
        if( (u_first == 0) || (d_first == 0) || \
            (l_first == 0) || (r_first == 0) ) {
            
            return false;
        }
        /* Find the last value in U/D/L/R above the threshold */
        for( i = gesture_data_.total_gestures - 1; i >= 0; i-- ) {
#if DEBUG
            /* me da palo convertir los prints

            Serial.print(F("Finding last: "));
            Serial.print(F("U:"));
            Serial.print(gesture_data_.u_data[i]);
            Serial.print(F(" D:"));
            Serial.print(gesture_data_.d_data[i]);
            Serial.print(F(" L:"));
            Serial.print(gesture_data_.l_data[i]);
            Serial.print(F(" R:"));
            Serial.println(gesture_data_.r_data[i]);
            */
#endif
            if( (gesture_data_.u_data[i] > GESTURE_THRESHOLD_OUT) &&
                (gesture_data_.d_data[i] > GESTURE_THRESHOLD_OUT) &&
                (gesture_data_.l_data[i] > GESTURE_THRESHOLD_OUT) &&
                (gesture_data_.r_data[i] > GESTURE_THRESHOLD_OUT) ) {
                
                u_last = gesture_data_.u_data[i];
                d_last = gesture_data_.d_data[i];
                l_last = gesture_data_.l_data[i];
                r_last = gesture_data_.r_data[i];
                break;
            }
        }
    }
    
    /* Calculate the first vs. last ratio of up/down and left/right */
    ud_ratio_first = ((u_first - d_first) * 100) / (u_first + d_first);
    lr_ratio_first = ((l_first - r_first) * 100) / (l_first + r_first);
    ud_ratio_last = ((u_last - d_last) * 100) / (u_last + d_last);
    lr_ratio_last = ((l_last - r_last) * 100) / (l_last + r_last);
       
#if DEBUG
    /* me da palo convertir los prints

    Serial.print(F("Last Values: "));
    Serial.print(F("U:"));
    Serial.print(u_last);
    Serial.print(F(" D:"));
    Serial.print(d_last);
    Serial.print(F(" L:"));
    Serial.print(l_last);
    Serial.print(F(" R:"));
    Serial.println(r_last);
    
    Serial.print(F("Ratios: "));
    Serial.print(F("UD Fi: "));
    Serial.print(ud_ratio_first);
    Serial.print(F(" UD La: "));
    Serial.print(ud_ratio_last);
    Serial.print(F(" LR Fi: "));
    Serial.print(lr_ratio_first);
    Serial.print(F(" LR La: "));
    Serial.println(lr_ratio_last);
    */
#endif
       
    /* Determine the difference between the first and last ratios */
    ud_delta = ud_ratio_last - ud_ratio_first;
    lr_delta = lr_ratio_last - lr_ratio_first;
    
#if DEBUG
    /* me da palo convertir los prints
    
    Serial.print("Deltas: ");
    Serial.print("UD: ");
    Serial.print(ud_delta);
    Serial.print(" LR: ");
    Serial.println(lr_delta);
    */
#endif

    /* Accumulate the UD and LR delta values */
    gesture_ud_delta_ += ud_delta;
    gesture_lr_delta_ += lr_delta;
    
#if DEBUG
    /* Me da palo convertir los prints

    Serial.print("Accumulations: ");
    Serial.print("UD: ");
    Serial.print(gesture_ud_delta_);
    Serial.print(" LR: ");
    Serial.println(gesture_lr_delta_);
    */
#endif
    
    /* Determine U/D gesture */
    if( gesture_ud_delta_ >= GESTURE_SENSITIVITY_1 ) {
        gesture_ud_count_ = 1;
    } else if( gesture_ud_delta_ <= -GESTURE_SENSITIVITY_1 ) {
        gesture_ud_count_ = -1;
    } else {
        gesture_ud_count_ = 0;
    }
    
    /* Determine L/R gesture */
    if( gesture_lr_delta_ >= GESTURE_SENSITIVITY_1 ) {
        gesture_lr_count_ = 1;
    } else if( gesture_lr_delta_ <= -GESTURE_SENSITIVITY_1 ) {
        gesture_lr_count_ = -1;
    } else {
        gesture_lr_count_ = 0;
    }
    
    /* Determine Near/Far gesture */
    if( (gesture_ud_count_ == 0) && (gesture_lr_count_ == 0) ) {
        if( (abs(ud_delta) < GESTURE_SENSITIVITY_2) && \
            (abs(lr_delta) < GESTURE_SENSITIVITY_2) ) {
            
            if( (ud_delta == 0) && (lr_delta == 0) ) {
                gesture_near_count_++;
            } else if( (ud_delta != 0) || (lr_delta != 0) ) {
                gesture_far_count_++;
            }
            
            if( (gesture_near_count_ >= 10) && (gesture_far_count_ >= 2) ) {
                if( (ud_delta == 0) && (lr_delta == 0) ) {
                    gesture_state_ = NEAR_STATE;
                } else if( (ud_delta != 0) && (lr_delta != 0) ) {
                    gesture_state_ = FAR_STATE;
                }
                return true;
            }
        }
    } else {
        if( (abs(ud_delta) < GESTURE_SENSITIVITY_2) && \
            (abs(lr_delta) < GESTURE_SENSITIVITY_2) ) {
                
            if( (ud_delta == 0) && (lr_delta == 0) ) {
                gesture_near_count_++;
            }
            
            if( gesture_near_count_ >= 10 ) {
                gesture_ud_count_ = 0;
                gesture_lr_count_ = 0;
                gesture_ud_delta_ = 0;
                gesture_lr_delta_ = 0;
            }
        }
    }
    
#if DEBUG
    /* me da palo convertir los prints
    
    Serial.print("UD_CT: ");
    Serial.print(gesture_ud_count_);
    Serial.print(" LR_CT: ");
    Serial.print(gesture_lr_count_);
    Serial.print(" NEAR_CT: ");
    Serial.print(gesture_near_count_);
    Serial.print(" FAR_CT: ");
    Serial.println(gesture_far_count_);
    Serial.println("----------");
    */
#endif
    
    return false;
}

bool decodeGesture()
{
    /* Return if near or far event is detected */
    if( gesture_state_ == NEAR_STATE ) {
        gesture_motion_ = DIR_NEAR;
        return true;
    } else if ( gesture_state_ == FAR_STATE ) {
        gesture_motion_ = DIR_FAR;
        return true;
    }
    
    /* Determine swipe direction */
    if( (gesture_ud_count_ == -1) && (gesture_lr_count_ == 0) ) {
        gesture_motion_ = DIR_UP;
    } else if( (gesture_ud_count_ == 1) && (gesture_lr_count_ == 0) ) {
        gesture_motion_ = DIR_DOWN;
    } else if( (gesture_ud_count_ == 0) && (gesture_lr_count_ == 1) ) {
        gesture_motion_ = DIR_RIGHT;
    } else if( (gesture_ud_count_ == 0) && (gesture_lr_count_ == -1) ) {
        gesture_motion_ = DIR_LEFT;
    } else if( (gesture_ud_count_ == -1) && (gesture_lr_count_ == 1) ) {
        if( abs(gesture_ud_delta_) > abs(gesture_lr_delta_) ) {
            gesture_motion_ = DIR_UP;
        } else {
            gesture_motion_ = DIR_RIGHT;
        }
    } else if( (gesture_ud_count_ == 1) && (gesture_lr_count_ == -1) ) {
        if( abs(gesture_ud_delta_) > abs(gesture_lr_delta_) ) {
            gesture_motion_ = DIR_DOWN;
        } else {
            gesture_motion_ = DIR_LEFT;
        }
    } else if( (gesture_ud_count_ == -1) && (gesture_lr_count_ == -1) ) {
        if( abs(gesture_ud_delta_) > abs(gesture_lr_delta_) ) {
            gesture_motion_ = DIR_UP;
        } else {
            gesture_motion_ = DIR_LEFT;
        }
    } else if( (gesture_ud_count_ == 1) && (gesture_lr_count_ == 1) ) {
        if( abs(gesture_ud_delta_) > abs(gesture_lr_delta_) ) {
            gesture_motion_ = DIR_DOWN;
        } else {
            gesture_motion_ = DIR_RIGHT;
        }
    } else {
        return false;
    }
    
    return true;
}

/* --- A PARTIR DE AQUI ES SENSOR DE PROXIMIDAD --- */

bool setProximityGain(uint8_t drive)
{
    uint8_t val;
    
    /* Read value from CONTROL register */
    if( !BSP_I2C_ReadRegister((uint8_t)APDS9960_CONTROL, val) ) {
        return false;
    }
    
    /* Set bits in register to given value */
    drive &= 0b00000011;
    drive = drive << 2;
    val &= 0b11110011;
    val |= drive;
    
    /* Write register value back into CONTROL register */
    if( !BSP_I2C_WriteRegister((uint8_t)APDS9960_CONTROL, val) ) {
        return false;
    }
    
    return true;
}

bool enableProximitySensor(bool interrupts)
{
    /* Set default gain, LED, interrupts, enable power, and enable sensor */
    if( !setProximityGain(DEFAULT_PGAIN) ) {
        return false;
    }
    if( !setLEDDrive(DEFAULT_LDRIVE) ) {
        return false;
    }
    if( interrupts ) {
        if( !setProximityIntEnable(1) ) {
            return false;
        }
    } else {
        if( !setProximityIntEnable(0) ) {
            return false;
        }
    }
    if( !enablePower() ){
        return false;
    }
    if( !setMode(PROXIMITY, 1) ) {
        return false;
    }
    
    return true;
}

bool setProximityIntEnable(uint8_t enable)
{
    uint8_t val;
    
    /* Read value from ENABLE register */
    if( !BSP_I2C_ReadRegister((uint8_t)APDS9960_ENABLE, val) ) {
        return false;
    }
    
    /* Set bits in register to given value */
    enable &= 0b00000001;
    enable = enable << 5;
    val &= 0b11011111;
    val |= enable;
    
    /* Write register value back into ENABLE register */
    if( !BSP_I2C_WriteRegister((uint8_t)APDS9960_ENABLE, val) ) {
        return false;
    }
    
    return true;
}

bool readProximity(uint8_t &val)
{
    val = 0;
    
    /* Read value from proximity data register */
    if( !BSP_I2C_ReadRegister((uint8_t)APDS9960_PDATA, val) ) {
        return false;
    }
    
    return true;
}
