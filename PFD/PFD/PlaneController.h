#pragma once
#include "FlightData.h"

class PlaneController
{
public:
    PlaneController();
    void update(float dt);
    const FlightData& getPlane() const;

private:
    FlightData m_plane;
    float m_pitchRate;
    float m_rollRate;
    float m_yawRate;
    float m_trackOffset;
};
