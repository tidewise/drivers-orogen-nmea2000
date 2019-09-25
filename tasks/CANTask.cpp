/* Generated from orogen/lib/orogen/templates/tasks/Task.cpp */

#include "CANTask.hpp"

using namespace nmea2000;

CANTask::CANTask(std::string const& name)
    : CANTaskBase(name)
{
}

CANTask::~CANTask()
{
}



/// The following lines are template definitions for the various state machine
// hooks defined by Orocos::RTT. See CANTask.hpp for more detailed
// documentation about them.

bool CANTask::configureHook()
{
    if (! CANTaskBase::configureHook())
        return false;
    return true;
}
bool CANTask::startHook()
{
    if (! CANTaskBase::startHook())
        return false;
    return true;
}
void CANTask::updateHook()
{
    CANTaskBase::updateHook();
}
void CANTask::errorHook()
{
    CANTaskBase::errorHook();
}
void CANTask::stopHook()
{
    CANTaskBase::stopHook();
}
void CANTask::cleanupHook()
{
    CANTaskBase::cleanupHook();
}
