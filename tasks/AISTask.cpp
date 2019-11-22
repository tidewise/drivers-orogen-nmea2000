/* Generated from orogen/lib/orogen/templates/tasks/Task.cpp */

#include "AISTask.hpp"
#include <nmea2000/PGNs.hpp>

using namespace nmea2000;

AISTask::AISTask(std::string const& name)
    : AISTaskBase(name)
{
}

AISTask::~AISTask()
{
}



/// The following lines are template definitions for the various state machine
// hooks defined by Orocos::RTT. See AISTask.hpp for more detailed
// documentation about them.

bool AISTask::configureHook()
{
    if (! AISTaskBase::configureHook())
        return false;
    return true;
}
bool AISTask::startHook()
{
    if (! AISTaskBase::startHook())
        return false;
    return true;
}
void AISTask::updateHook()
{
    Message msg;
    while (_msg_in.read(msg) == RTT::NewData) {
        if (pgns::AISClassAPositionReport::ID == msg.pgn) {
            pgns::AISClassAPositionReport in =
                pgns::AISClassAPositionReport::fromMessage(msg);
            AISPosition position_out;
            position_out.time = in.time;
            position_out.mmsi = in.user_id;
            position_out.course_over_ground = base::Angle::fromDeg(in.cog);
            position_out.latitude = base::Angle::fromDeg(in.latitude);
            position_out.longitude = base::Angle::fromDeg(in.longitude);
            position_out.status = (NavigationalStatus)in.nav_status;
            position_out.high_accuracy_position = (in.position_accuracy == 1);
            position_out.yaw = base::Angle::fromDeg(in.heading);
            position_out.yaw_velocity = in.rate_of_turn * M_PI / 180;
            position_out.speed_over_ground = in.sog;
            _vessel_position.write(position_out);
        }
        else if (pgns::AISClassBPositionReport::ID == msg.pgn) {
            pgns::AISClassBPositionReport in =
                pgns::AISClassBPositionReport::fromMessage(msg);
            AISPosition position_out;
            position_out.time = in.time;
            position_out.mmsi = in.user_id;
            position_out.course_over_ground = base::Angle::fromDeg(in.cog);
            position_out.latitude = base::Angle::fromDeg(in.latitude);
            position_out.longitude = base::Angle::fromDeg(in.longitude);
            position_out.high_accuracy_position = (in.position_accuracy == 1);
            position_out.yaw = base::Angle::fromDeg(in.heading);
            position_out.speed_over_ground = in.sog;
            _vessel_position.write(position_out);
        }
        else if (pgns::AISClassBExtendedPositionReport::ID == msg.pgn) {
            pgns::AISClassBExtendedPositionReport in =
                pgns::AISClassBExtendedPositionReport::fromMessage(msg);
            AISPosition position_out;
            position_out.time = in.time;
            position_out.mmsi = in.user_id;
            position_out.course_over_ground = base::Angle::fromDeg(in.cog);
            position_out.latitude = base::Angle::fromDeg(in.latitude);
            position_out.longitude = base::Angle::fromDeg(in.longitude);
            position_out.high_accuracy_position = false;
            position_out.yaw = base::Angle::fromDeg(in.true_heading);
            position_out.speed_over_ground = in.sog;
            _vessel_position.write(position_out);

            AISVesselInformation info_out;
            info_out.time = in.time;
            info_out.mmsi = in.user_id;
            info_out.imo = 0;
            info_out.name = in.name;
            info_out.length = in.length;
            info_out.width = in.beam;
            info_out.ship_type = in.type_of_ship;
            info_out.position_device_type = in.gnss_type;
            info_out.reference_position =
                Eigen::Vector3d(in.position_reference_from_bow,
                                -in.position_reference_from_starboard,
                                0);
            _vessel_information.write(info_out);
        }
        else if (pgns::AISClassAStaticAndVoyageRelatedData::ID == msg.pgn) {
            pgns::AISClassAStaticAndVoyageRelatedData in =
                pgns::AISClassAStaticAndVoyageRelatedData::fromMessage(msg);

            AISVesselInformation info_out;
            info_out.time = in.time;
            info_out.mmsi = in.user_id;
            info_out.imo = in.imo_number;
            info_out.name = in.name;
            info_out.call_sign = in.callsign;
            info_out.length = in.length;
            info_out.width = in.beam;
            info_out.draft = in.draft;
            info_out.ship_type = in.type_of_ship;
            info_out.position_device_type = in.gnss_type;
            info_out.reference_position =
                Eigen::Vector3d(in.position_reference_from_bow,
                                -in.position_reference_from_starboard,
                                0);
            _vessel_information.write(info_out);

            AISVoyageInformation voyage_out;
            voyage_out.time = in.time;
            voyage_out.imo = in.imo_number;
            // This does not take into account leap seconds, but given the low
            // criticality of this information, I didn't try to solve that
            // particular hairy problem
            voyage_out.eta = base::Time::fromMilliseconds(
                (static_cast<uint64_t>(in.eta_date) * 3600 * 24 +
                 static_cast<uint64_t>(in.eta_time)) * 1000
            );
            voyage_out.destination = in.destination;
            _voyage_information.write(voyage_out);
        }

    }
    AISTaskBase::updateHook();
}
void AISTask::errorHook()
{
    AISTaskBase::errorHook();
}
void AISTask::stopHook()
{
    AISTaskBase::stopHook();
}
void AISTask::cleanupHook()
{
    AISTaskBase::cleanupHook();
}
