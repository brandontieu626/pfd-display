#pragma once
struct FlightData
{
	float pitch;	// degrees, positive = nose up
	float roll;		// degrees, positive = right wing down
	float heading;	// degrees, 0-360Yes
	float airspeed; // knots
	float altitude; // feet
	float vspeed;   // feet per minute
	float track;    // degrees 0-360, actual direction of movement over ground
};