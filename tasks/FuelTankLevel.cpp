/* Generated from orogen/lib/orogen/templates/tasks/Task.cpp */

#include "FuelTankLevel.hpp"

using namespace nmea2000;

FuelTankLevel::FuelTankLevel(std::string const& name)
    : FuelTankLevelBase(name)
{
}

FuelTankLevel::~FuelTankLevel()
{
}



/// The following lines are template definitions for the various state machine
// hooks defined by Orocos::RTT. See FuelTankLevel.hpp for more detailed
// documentation about them.

bool FuelTankLevel::configureHook()
{
    if (! FuelTankLevelBase::configureHook())
        return false;
    return true;
}
bool FuelTankLevel::startHook()
{
    if (! FuelTankLevelBase::startHook())
        return false;
    return true;
}
void FuelTankLevel::updateHook()
{
    FuelTankLevelBase::updateHook();
}
void FuelTankLevel::errorHook()
{
    FuelTankLevelBase::errorHook();
}
void FuelTankLevel::stopHook()
{
    FuelTankLevelBase::stopHook();
}
void FuelTankLevel::cleanupHook()
{
    FuelTankLevelBase::cleanupHook();
}
