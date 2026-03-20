/* Generated from orogen/lib/orogen/templates/tasks/Task.cpp */

#include "FluidLevelTask.hpp"
#include <nmea2000/PGNs.hpp>

using namespace nmea2000;

FluidLevelTask::FluidLevelTask(std::string const& name)
    : FluidLevelTaskBase(name)
{
}

FluidLevelTask::~FluidLevelTask()
{
}

/// The following lines are template definitions for the various state machine
// hooks defined by Orocos::RTT. See FluidLevelTask.hpp for more detailed
// documentation about them.

bool FluidLevelTask::configureHook()
{
    if (!FluidLevelTaskBase::configureHook()) {
        return false;
    }
    m_instance_id = _instance_id.get();
    return true;
}
bool FluidLevelTask::startHook()
{
    if (!FluidLevelTaskBase::startHook()) {
        return false;
    }
    return true;
}
void FluidLevelTask::updateHook()
{
    float capacity = _capacity.get();

    nmea2000::Message msg;
    while (_msg_in.read(msg) == RTT::NewData) {
        if (msg.pgn == pgns::FluidLevel::ID) {
            auto in = pgns::FluidLevel::fromMessage(msg);
            if (m_instance_id == ANY_INSTANCE_ID || in.instance == m_instance_id) {
                tank_base::FluidLevel out;
                out.time = in.time;
                float const total_capacity = (capacity ? capacity : in.capacity);
                if (total_capacity > 0) {
                    out.currentLevel = in.level / total_capacity;
                    _level_out.write(out);
                }
            }
        }
    }
    FluidLevelTaskBase::updateHook();
}
void FluidLevelTask::errorHook()
{
    FluidLevelTaskBase::errorHook();
}
void FluidLevelTask::stopHook()
{
    FluidLevelTaskBase::stopHook();
}
void FluidLevelTask::cleanupHook()
{
    FluidLevelTaskBase::cleanupHook();
}
