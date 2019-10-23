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
    m_dispatcher->setContinuousDeviceEnumeration(_continuous_device_enumeration.get());

    return true;
}
bool CANTask::startHook()
{
    if (! CANTaskBase::startHook()) {
        return false;
    }
    m_dispatcher->resetQueryLogic();
    m_receiver.reset(new Receiver(m_library));

    if (m_dispatcher->getQueryState() == DeviceDispatcher::QUERY_IN_PROGRESS) {
        state(QUERY_IN_PROGRESS);
    }
    return true;
}
void CANTask::updateHook()
{
    auto query_state = m_dispatcher->getQueryState();
    if (query_state == DeviceDispatcher::QUERY_TIMED_OUT) {
        exception(DEVICE_RESOLUTION_FAILED);
    }
    else if (query_state == DeviceDispatcher::QUERY_COMPLETE &&
             state() != QUERY_COMPLETE) {
        state(QUERY_COMPLETE);
    }

    auto query_message = m_dispatcher->getQueryProbeMessage();
    if (query_message.first) {
        _can_out.write(query_message.second.toCAN());
    }

    canbus::Message can;
    while (_can_in.read(can) == RTT::NewData) {
        auto msg = Message::fromCAN(can);
        auto resolver_state = m_receiver->process(msg);
        if (resolver_state.first < Receiver::COMPLETE) {
            continue;
        }
        else if (resolver_state.first == Receiver::COMPLETE_DEVICE_INFO) {
            _device_information.write(m_receiver->getDeviceInformation(msg.source));
        }

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
