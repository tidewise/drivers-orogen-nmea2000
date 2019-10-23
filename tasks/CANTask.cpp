/* Generated from orogen/lib/orogen/templates/tasks/Task.cpp */

#include "CANTask.hpp"
#include "DeviceDispatcher.hpp"

using namespace nmea2000;

CANTask::CANTask(std::string const& name)
    : CANTaskBase(name)
{
}

CANTask::~CANTask()
{
}

bool CANTask::configureHook()
{
    if (! CANTaskBase::configureHook()) {
        return false;
    }

    m_dispatcher.reset(new DeviceDispatcher(
        _devices.get(), _resolution_timeout.get(),
        _enumeration_ack_timeout.get()
    ));

    return true;
}
bool CANTask::startHook()
{
    if (! CANTaskBase::startHook()) {
        return false;
    }
    m_dispatcher->resetQueryLogic();
    return true;
}
void CANTask::updateHook()
{
    auto state = m_dispatcher->getQueryState();
    if (state.first == DeviceDispatcher::QUERY_TIMED_OUT) {
        exception(DEVICE_RESOLUTION_FAILED);
    }
    else if (state.first == DeviceDispatcher::QUERY_SEND_PROBE) {
        _can_out.write(state.second.toCAN());
    }

    canbus::Message can;
    while (_can_in.read(can) == RTT::NewData) {
        auto msg = Message::fromCAN(can);
        auto resolved_devices = m_dispatcher->process(msg);
        for (auto const& d : resolved_devices) {
            _resolved_devices.write(d);
        }
        _messages.write(msg);
    }

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
