/* Generated from orogen/lib/orogen/templates/tasks/Task.cpp */

#include "AirmarDST800Task.hpp"
#include <nmea2000/PGNs.hpp>

using namespace nmea2000;

AirmarDST800Task::AirmarDST800Task(std::string const& name)
    : AirmarDST800TaskBase(name) {
}

AirmarDST800Task::~AirmarDST800Task() {
}

/// The following lines are template definitions for the various state machine
// hooks defined by Orocos::RTT. See AirmarDST800Task.hpp for more detailed
// documentation about them.

bool AirmarDST800Task::configureHook()
{
    if (! AirmarDST800TaskBase::configureHook()) {
        return false;
    }
    return true;
}
bool AirmarDST800Task::startHook()
{
    if (! AirmarDST800TaskBase::startHook()) {
        return false;
    }

    mRange = _range.get();
    return true;
}
void AirmarDST800Task::updateHook()
{
    Message msg;
    while (_msg_in.read(msg) == RTT::NewData) {
        if (msg.pgn == pgns::WaterDepth::ID) {
            pgns::WaterDepth in =
                pgns::WaterDepth::fromMessage(msg);
            base::samples::RigidBodyState out;
            out.time = in.time;

            double range = base::isUnknown(mRange) ? in.range : mRange;
            if (in.depth < range) {
                out.position.z() = in.depth;
            }

            out.sourceFrame = _ground_frame.get();
            out.targetFrame = _sensor_frame.get();
            _depth_samples.write(out);
        }
        else if (msg.pgn == pgns::EnvironmentalParametersExt::ID) {
            pgns::EnvironmentalParametersExt in =
                pgns::EnvironmentalParametersExt::fromMessage(msg);

            base::Temperature temperature =
                base::Temperature::fromCelsius(in.temperature);
            _temperature_samples.write(temperature);
        }
        else if (msg.pgn == pgns::Speed::ID) {
            pgns::Speed in = pgns::Speed::fromMessage(msg);

            base::samples::RigidBodyState speed;
            speed.time = in.time;
            speed.velocity.x() = in.speed_water_referenced;
            speed.sourceFrame = _water_frame.get();
            speed.targetFrame = _sensor_frame.get();
            _speed_samples.write(speed);
        }
    }

    AirmarDST800TaskBase::updateHook();
}
void AirmarDST800Task::errorHook()
{
    AirmarDST800TaskBase::errorHook();
}
void AirmarDST800Task::stopHook()
{
    AirmarDST800TaskBase::stopHook();
}
void AirmarDST800Task::cleanupHook()
{
    AirmarDST800TaskBase::cleanupHook();
}
