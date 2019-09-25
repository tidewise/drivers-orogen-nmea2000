#ifndef nmea2000_TYPES_HPP
#define nmea2000_TYPES_HPP

#include <base/Time.hpp>
#include <base/Angle.hpp>

namespace nmea2000 {
    struct FluidLevel {
        base::Time time;

        enum Type {
            FUEL = 0,
            WATER = 1,
            GRAY_WATER = 2,
            LIVE_WELL = 3,
            OIL = 4,
            BLACK_WATGER = 5
        };

        Type type;
        float level;
        float capacity;
    };

    enum NAVIGATIONAL_STATUS {
        STATUS_UNDER_WAY_USING_ENGINE = 0,
        STATUS_AT_ANCHOR = 1,
        STATUS_NOT_UNDER_COMMAND = 2,
        STATUS_RESTRICTED_MANOEUVERABILITY = 3,
        STATUS_RESTRICTED_BY_DRAUGHT = 4,
        STATUS_MOORED = 5,
        STATUS_AGROUND = 6,
        STATUS_ENGAGED_IN_FISHING = 7,
        STATUS_UNDER_WAY_SAILING = 8,
        STATUS_RESERVED0 = 9,
        STATUS_RESERVED1 = 10,
        STATUS_RESERVED2 = 11,
        STATUS_RESERVED3 = 12,
        STATUS_RESERVED4 = 13,
        STATUS_AIS_SART_ACTIVE = 14,
        STATUS_NOT_DEFINED = 15
    };

    /** Representation of the data stored in AIS Message 1 to 3 */
    struct AISPosition {
        base::Time time;

        /** The vessel MMSI number */
        int mmsi;

        /** Vessel course over ground */
        base::Angle course_over_ground;
        base::Angle latitude;
        base::Angle longitude;
        NAVIGATIONAL_STATUS status;

        /** Whether the GPS position is using differential (< 10m resolution) or
         * not
         */
        bool high_accuracy_position;
        /** Vessel heading */
        base::Angle yaw;
        /** Velocity in yaw, in rad/s */
        double yaw_velocity;
        /** Speed over ground, in m/s */
        double speed_over_ground;
    };

    /** Representation of the data stored in AIS Message 5 */
    struct AISVesselInformation {
        base::Time time;

        int mmsi;
        int imo;
        std::string call_sign;
        std::string name;
        int ship_type;
        int position_device_type;
        double draught;
        double length;
        double width;
        double gross_weight;
        double dead_weight;
    };

    /** Measurement quality factor reported by Airmar depth sounders */
    struct AirmarAltitudeQuality {
        base::Time time;

        /** The airmar-specific factor
         *
         * Set to base::unknown if there is no bottom lock
         */
        float quality_factor;
    };
}

#endif

