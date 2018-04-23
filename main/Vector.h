#ifndef VECTOR_H
#define VECTOR_H


#include <stdint.h>


typedef struct
{
	float X;
	float Y;
	float Z;
} Vector3;


typedef struct
{
	int16_t X;
	int16_t Y;
	int16_t Z;
} I16Vector3;


#endif