#pragma once
struct FlightData
{
	float pitch;	// angle of aircraft's nose, degrees, positive = nose up
	float roll;		// angle of aircraft's wing relative to the horizon , degrees, positive = right wing down
	float heading;	// compass direction of aircraft's nose , degrees, 0-360
	float airspeed; // speed of aircraft relative to air around it, knots
	float altitude; // height above sea level, feet
	float vspeed;   // how fast air craft is climbing (rate of change of altitude), feet per minute
	float track;    // actual direction of movement over ground, degrees, 0-360
};