/* Generated from orogen/lib/orogen/templates/tasks/Task.cpp */

#include "ActisenseTask.hpp"

using namespace nmea2000;

ActisenseTask::ActisenseTask(std::string const& name)
    : ActisenseTaskBase(name)
{
}

ActisenseTask::~ActisenseTask()
{
}



/// The following lines are template definitions for the various state machine
// hooks defined by Orocos::RTT. See ActisenseTask.hpp for more detailed
// documentation about them.

bool ActisenseTask::configureHook()
{
    if (! ActisenseTaskBase::configureHook())
        return false;
    return true;
}
bool ActisenseTask::startHook()
{
    if (! ActisenseTaskBase::startHook())
        return false;
    return true;
}
void ActisenseTask::updateHook()
{
    ActisenseTaskBase::updateHook();
}
void ActisenseTask::errorHook()
{
    ActisenseTaskBase::errorHook();
}
void ActisenseTask::stopHook()
{
    ActisenseTaskBase::stopHook();
}
void ActisenseTask::cleanupHook()
{
    ActisenseTaskBase::cleanupHook();
}
