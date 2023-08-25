/* Generated from orogen/lib/orogen/templates/tasks/Task.cpp */

#include "YatchYDHS01Task.hpp"
#include <nmea2000/PGNs.hpp>

using namespace nmea2000;

YatchYDHS01Task::YatchYDHS01Task(std::string const& name)
    : YatchYDHS01TaskBase(name)
{
}

YatchYDHS01Task::~YatchYDHS01Task()
{
}

/// The following lines are template definitions for the various state machine
// hooks defined by Orocos::RTT. See YatchYDHS01Task.hpp for more detailed
// documentation about them.

bool YatchYDHS01Task::configureHook()
{
    if (!YatchYDHS01TaskBase::configureHook())
        return false;
    return true;
}
bool YatchYDHS01Task::startHook()
{
    if (!YatchYDHS01TaskBase::startHook())
        return false;
    return true;
}
void YatchYDHS01Task::updateHook()
{
    YatchYDHS01TaskBase::updateHook();

    Message msg;
    while (_msg_in.read(msg) == RTT::NewData) {
        if (msg.pgn == pgns::EnvironmentalParametersExt::ID) {
            pgns::EnvironmentalParametersExt in =
                pgns::EnvironmentalParametersExt::fromMessage(msg);

            base::samples::Temperature temperature_out =
                base::samples::Temperature::fromCelsius(in.time, in.temperature);
            _temperature.write(temperature_out);
            Humidity humidity_out;
            humidity_out.time = in.time;
            humidity_out.value = in.humidity / 100.0;
            _relative_humidity.write(humidity_out);
        }
    }
}
void YatchYDHS01Task::errorHook()
{
    YatchYDHS01TaskBase::errorHook();
}
void YatchYDHS01Task::stopHook()
{
    YatchYDHS01TaskBase::stopHook();
}
void YatchYDHS01Task::cleanupHook()
{
    YatchYDHS01TaskBase::cleanupHook();
}
