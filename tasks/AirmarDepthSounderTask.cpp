/* Generated from orogen/lib/orogen/templates/tasks/Task.cpp */

#include "AirmarDepthSounderTask.hpp"
#include <nmea2000/PGNs.hpp>

using namespace nmea2000;

AirmarDepthSounderTask::AirmarDepthSounderTask(std::string const& name)
    : AirmarDepthSounderTaskBase(name)
{
}

AirmarDepthSounderTask::~AirmarDepthSounderTask()
{
}



/// The following lines are template definitions for the various state machine
// hooks defined by Orocos::RTT. See AirmarDepthSounderTask.hpp for more detailed
// documentation about them.

bool AirmarDepthSounderTask::configureHook()
{
    if (! AirmarDepthSounderTaskBase::configureHook())
        return false;
    return true;
}
bool AirmarDepthSounderTask::startHook()
{
    if (! AirmarDepthSounderTaskBase::startHook())
        return false;
    return true;
}
void AirmarDepthSounderTask::updateHook()
{
    Message msg;
    while (_msg_in.read(msg) == RTT::NewData) {
        if (msg.pgn == pgns::AirmarDepthQualityFactor::ID) {
            pgns::AirmarDepthQualityFactor in =
                pgns::AirmarDepthQualityFactor::fromMessage(msg);
            AirmarAltitudeQuality out;
            out.time = in.time;
            out.quality_factor = in.depth_quality_factor;
            _quality_samples.write(out);
        }
        else if (msg.pgn == pgns::WaterDepth::ID) {
            pgns::WaterDepth in =
                pgns::WaterDepth::fromMessage(msg);
            base::samples::RigidBodyState out;
            out.time = in.time;
            out.position.z() = in.depth;
            _altitude_samples.write(out);
        }
    }

    AirmarDepthSounderTaskBase::updateHook();
}
void AirmarDepthSounderTask::errorHook()
{
    AirmarDepthSounderTaskBase::errorHook();
}
void AirmarDepthSounderTask::stopHook()
{
    AirmarDepthSounderTaskBase::stopHook();
}
void AirmarDepthSounderTask::cleanupHook()
{
    AirmarDepthSounderTaskBase::cleanupHook();
}
