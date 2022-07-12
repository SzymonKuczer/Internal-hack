#pragma once
#include <cstdint>
// Created with ReClass.NET 1.2 by KN4CK3R

class PlayerHealth
{
public:
	int32_t health; //0x0000
	int32_t maxHealth; //0x0004
	int32_t shield; //0x0008
	int32_t shieldType; //0x000C
	char pad_0010[52]; //0x0010
	int32_t posX; //0x0044
	int32_t posY; //0x0048
	int32_t posZ; //0x004C
	char pad_0050[114]; //0x0050
	int32_t N00000069; //0x00C2
	char pad_00C6[876]; //0x00C6
}; //Size: 0x0432