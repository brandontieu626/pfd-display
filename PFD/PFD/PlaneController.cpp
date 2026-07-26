#include "PlaneController.h"
#include <SFML/Graphics.hpp>
#include <algorithm>

// PITCH CONTROL CONSTANTS
constexpr float PITCH_ACCEL       = 15.f; // deg/sec^2 — rate of pitch rate increase while key held
constexpr float PITCH_MAX_RATE    = 10.f; // deg/sec   — peak pitch rate
constexpr float PITCH_DECAY       = 20.f; // deg/sec^2 — rate of pitch rate decrease when key released
constexpr float PITCH_CENTER_RATE =  5.f; // deg/sec   — speed pitch returns to level when key released

// ROLL CONTROL CONSTANTS
constexpr float ROLL_ACCEL       = 30.f; // deg/sec^2 — rate of roll rate increase while key held
constexpr float ROLL_MAX_RATE    = 25.f; // deg/sec   — peak roll rate
constexpr float ROLL_DECAY       = 35.f; // deg/sec^2 — rate of roll rate decrease when key released
constexpr float ROLL_CENTER_RATE = 10.f; // deg/sec   — speed roll returns to wings-level when key released

// HEADING CONTROL CONSTANTS
constexpr float HEADING_ACCEL    = 45.f; // deg/sec^2 — rate of yaw rate increase while key held
constexpr float HEADING_MAX_RATE = 30.f; // deg/sec   — peak turn rate
constexpr float HEADING_DECAY    = 60.f; // deg/sec^2 — rate of yaw rate decrease when key released

// AIRSPEED CONSTANTS
constexpr float SPEED_STALL = 60.f;     // below = stall (red)
constexpr float SPEED_CAUTION = 250.f;  // above = caution (yellow)
constexpr float SPEED_MAX = 300.f;      // above = overspeed (red)

// VSPEED CONSTANTS
constexpr float VSPEED_PITCH_SCALE = 100.f; // ft/min per degree of pitch
constexpr float VSPEED_RESPONSE    =  0.8f; // how quickly vspeed tracks pitch target

PlaneController::PlaneController()
    : m_plane{ 0.f, 0.f, 0.f, 10.f, 5000.f, 0.f, 0.f }
    , m_pitchRate(0.f)
    , m_rollRate(0.f)
    , m_yawRate(0.f)
    , m_trackOffset(0.f)
{
}

void PlaneController::update(float dt)
{
    // Pitch — Up to pitch up, Down to pitch down
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
        m_pitchRate = std::clamp(m_pitchRate + PITCH_ACCEL * dt, -PITCH_MAX_RATE, PITCH_MAX_RATE);
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
        m_pitchRate = std::clamp(m_pitchRate - PITCH_ACCEL * dt, -PITCH_MAX_RATE, PITCH_MAX_RATE);
    else
    {
        if (m_pitchRate > 0.f)
            m_pitchRate = std::max(0.f, m_pitchRate - PITCH_DECAY * dt);
        else
            m_pitchRate = std::min(0.f, m_pitchRate + PITCH_DECAY * dt);

        if (m_plane.pitch > 0.f)
            m_plane.pitch = std::max(0.f, m_plane.pitch - PITCH_CENTER_RATE * dt);
        else if (m_plane.pitch < 0.f)
            m_plane.pitch = std::min(0.f, m_plane.pitch + PITCH_CENTER_RATE * dt);
    }

    // Roll — Left arrow to roll left, Right arrow to roll right
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
        m_rollRate = std::clamp(m_rollRate + ROLL_ACCEL * dt, -ROLL_MAX_RATE, ROLL_MAX_RATE);
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
        m_rollRate = std::clamp(m_rollRate - ROLL_ACCEL * dt, -ROLL_MAX_RATE, ROLL_MAX_RATE);
    else
    {
        if (m_rollRate > 0.f)
            m_rollRate = std::max(0.f, m_rollRate - ROLL_DECAY * dt);
        else
            m_rollRate = std::min(0.f, m_rollRate + ROLL_DECAY * dt);

        if (m_plane.roll > 0.f)
            m_plane.roll = std::max(0.f, m_plane.roll - ROLL_CENTER_RATE * dt);
        else if (m_plane.roll < 0.f)
            m_plane.roll = std::min(0.f, m_plane.roll + ROLL_CENTER_RATE * dt);
    }

    // Heading — A to turn left, D to turn right
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        m_yawRate = std::clamp(m_yawRate - HEADING_ACCEL * dt, -HEADING_MAX_RATE, HEADING_MAX_RATE);
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        m_yawRate = std::clamp(m_yawRate + HEADING_ACCEL * dt, -HEADING_MAX_RATE, HEADING_MAX_RATE);
    else
    {
        if (m_yawRate > 0.f)
            m_yawRate = std::max(0.f, m_yawRate - HEADING_DECAY * dt);
        else
            m_yawRate = std::min(0.f, m_yawRate + HEADING_DECAY * dt);
    }

    // Airspeed — W to increase, S to decrease
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        m_plane.airspeed = std::clamp(m_plane.airspeed + 0.5f, 0.f, 400.f);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        m_plane.airspeed = std::clamp(m_plane.airspeed - 0.5f, 0.f, 400.f);

    // Crosswind drift — T to drift right, G to drift left
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::T))
        m_trackOffset = std::clamp(m_trackOffset + 0.5f, -30.f, 30.f);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::G))
        m_trackOffset = std::clamp(m_trackOffset - 0.5f, -30.f, 30.f);

    // Integrate pitch and roll
    m_plane.pitch = std::clamp(m_plane.pitch + m_pitchRate * dt, -20.f, 20.f);
    m_plane.roll  = std::clamp(m_plane.roll  + m_rollRate  * dt, -60.f, 60.f);

    // Derive vspeed from pitch via first-order lag
    m_plane.vspeed += (m_plane.pitch * VSPEED_PITCH_SCALE - m_plane.vspeed) * VSPEED_RESPONSE * dt;

    // Integrate altitude from vspeed (ft/min → ft/sec via /60)
    m_plane.altitude += m_plane.vspeed * (dt / 60.f);
    m_plane.altitude = std::clamp(m_plane.altitude, 0.f, 50000.f);

    // Integrate heading and derive track from heading + crosswind offset
    m_plane.heading += m_yawRate * dt;
    while (m_plane.heading <   0.f) m_plane.heading += 360.f;
    while (m_plane.heading >= 360.f) m_plane.heading -= 360.f;
    m_plane.track = m_plane.heading + m_trackOffset;
    while (m_plane.track <   0.f) m_plane.track += 360.f;
    while (m_plane.track >= 360.f) m_plane.track -= 360.f;
}

const FlightData& PlaneController::getPlane() const
{
    return m_plane;
}
