/* Generated from orogen/lib/orogen/templates/tasks/Task.cpp */

#include "CANTask.hpp"
#include "DeviceDispatcher.hpp"
#include <nmea2000/PGNs.hpp>
#include <nmea2000/Receiver.hpp>

using namespace nmea2000;

CANTask::CANTask(std::string const& name)
    : CANTaskBase(name)
    , m_library(pgns::getLibrary()) {
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
    m_receiver.reset(new Receiver(m_library));
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
        auto resolver_state = m_receiver->process(msg);
        if (resolver_state.first < Receiver::COMPLETE) {
            continue;
        }
        auto resolved_devices = m_dispatcher->process(msg);
        auto resolved_devices = m_dispatcher->process(resolver_state.second);
        for (auto const& d : resolved_devices) {
            _resolved_devices.write(d);
        }
        _messages.write(resolver_state.second);
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
