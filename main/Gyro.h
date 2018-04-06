#ifndef GYRO_H
#define GYRO_H


typedef struct
{
	float Yaw;
	float Pitch;
	float Roll;
} GyroState;


void Gyro_Initialize();

void Gyro_UpdateState();


#endif