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
        switch (msg.pgn) {
            case pgns::TemperatureExtendedRange::ID: {
                pgns::TemperatureExtendedRange in =
                    pgns::TemperatureExtendedRange::fromMessage(msg);

                if (in.temperature_source == _id) {
                    base::samples::Temperature temperature_out =
                        base::samples::Temperature::fromCelsius(in.time,
                            in.actual_temperature);
                    _temperature.write(temperature_out);
                }
            } break;

            case pgns::Humidity::ID: {
                pgns::Humidity in = pgns::Humidity::fromMessage(msg);

                if (in.humidity_source == _id) {
                    Humidity humidity_out;
                    humidity_out.time = in.time;
                    humidity_out.value = in.actual_humidity / 100.0;
                    _relative_humidity.write(humidity_out);
                }
            } break;
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
