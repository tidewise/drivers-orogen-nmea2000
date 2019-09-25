/* Generated from orogen/lib/orogen/templates/tasks/Task.cpp */

#include "AirmarDepthSounder.hpp"

using namespace nmea2000;

AirmarDepthSounder::AirmarDepthSounder(std::string const& name)
    : AirmarDepthSounderBase(name)
{
}

AirmarDepthSounder::~AirmarDepthSounder()
{
}



/// The following lines are template definitions for the various state machine
// hooks defined by Orocos::RTT. See AirmarDepthSounder.hpp for more detailed
// documentation about them.

bool AirmarDepthSounder::configureHook()
{
    if (! AirmarDepthSounderBase::configureHook())
        return false;
    return true;
}
bool AirmarDepthSounder::startHook()
{
    if (! AirmarDepthSounderBase::startHook())
        return false;
    return true;
}
void AirmarDepthSounder::updateHook()
{
    AirmarDepthSounderBase::updateHook();
}
void AirmarDepthSounder::errorHook()
{
    AirmarDepthSounderBase::errorHook();
}
void AirmarDepthSounder::stopHook()
{
    AirmarDepthSounderBase::stopHook();
}
void AirmarDepthSounder::cleanupHook()
{
    AirmarDepthSounderBase::cleanupHook();
}
