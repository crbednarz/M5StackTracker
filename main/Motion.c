#include "Motion.h"
#include "driver/i2c.h"
#include "MPU9250.h"
#include "PeripheralConfig.h"
#include <stdint.h>
#include <string.h>


#define MAGNETOMETER_SCALE MFS_16BITS
#define ACCELEROMETER_SCALE AFS_2G
#define Motion_SCALE GFS_250DPS


typedef struct
{
    int16_t X;
    int16_t Y;
    int16_t Z;
} GyroState;


static struct
{
    GyroState GyroState;
} MotionContext;


void Motion_Initialize()
{
	memset(&MotionContext, 0, sizeof(MotionContext));

}

void Motion_UpdateState()
{
	
}