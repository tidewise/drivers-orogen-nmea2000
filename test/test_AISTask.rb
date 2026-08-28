# frozen_string_literal: true

using_task_library "nmea2000"

describe OroGen.nmea2000.AISTask do
    run_live

    attr_reader :task

    before do
        @task = syskit_deploy(
            OroGen.nmea2000.AISTask
                  .deployed_as("task_under_test")
        )
    end

    it "outputs the class A position report" do
        syskit_configure_and_start(@task)
        payload = [
            1,
            189,
            216,
            87,
            42,
            187,
            62,
            21,
            227,
            158,
            173,
            204,
            240,
            123,
            255,
            255,
            5,
            0,
            126,
            132,
            0,
            255,
            255,
            255,
            127,
            192,
            248,
            255
        ]

        output = expect_execution { nmea2000_write pgn: 129_038, payload: payload }
                 .to { have_one_new_sample task.vessel_position_port }

        assert_equal(710_400_189, output.mmsi)
        assert_in_delta(-0.846741149, output.longitude.rad, 1e-6)
        assert_in_delta(-0.445097, output.latitude.rad, 1e-6)
        assert(output.high_accuracy_position)
        assert(output.raim)
        assert(output.course_over_ground.rad.nan?)
        assert_in_delta(0.05, output.speed_over_ground, 1e-6)
        assert(output.yaw.rad.nan?)
        assert_in_delta(3.2767, output.yaw_velocity, 1e-6)
        assert_equal(:STATUS_NOT_DEFINED, output.status)
    end

    it "outputs the class B position report" do
        syskit_configure_and_start(@task)
        payload = [
            18,
            164,
            219,
            87,
            42,
            205,
            5,
            24,
            227,
            0,
            217,
            202,
            240,
            124,
            255,
            255,
            0,
            0,
            0,
            0,
            32,
            255,
            255,
            0,
            112,
            254
        ]

        output = expect_execution { nmea2000_write pgn: 129_039, payload: payload }
                 .to { have_one_new_sample task.vessel_position_port }

        assert_equal(710_400_932, output.mmsi)
        assert_in_delta(-0.84642343, output.longitude.rad, 1e-6)
        assert_in_delta(-0.445306, output.latitude.rad, 1e-6)
        assert_equal(0, output.high_accuracy_position)
        assert_equal(0, output.raim)
        assert(output.course_over_ground.rad.nan?)
        assert_equal(0.0, output.speed_over_ground)
        assert(output.yaw.rad.nan?)
    end

    it "outputs the class B extended report" do
        syskit_configure_and_start(@task)
        payload = [
            19,
            218,
            63,
            82,
            42,
            205,
            5,
            24,
            227,
            0,
            217,
            202,
            240,
            124,
            255,
            255,
            0,
            0,
            0,
            0,
            52,
            255,
            255,
            16,
            64,
            1,
            120,
            0,
            60,
            0,
            120,
            0,
            83,
            65,
            65,
            77,
            32,
            69,
            76,
            69,
            67,
            84,
            82,
            65,
            65,
            64,
            64,
            64,
            64,
            64,
            64,
            64,
            2,
            225
        ]

        vessel_pos, vessel_info =
            expect_execution { nmea2000_write pgn: 129_040, payload: payload }
            .to do
                [have_one_new_sample(task.vessel_position_port),
                 have_one_new_sample(task.vessel_information_port)]
            end

        assert_equal(710_033_370, vessel_pos.mmsi)
        assert_in_delta(-0.84642343, vessel_pos.longitude.rad, 1e-6)
        assert_in_delta(-0.445306, vessel_pos.latitude.rad, 1e-6)
        assert_equal(0, vessel_pos.high_accuracy_position)
        assert(vessel_pos.course_over_ground.rad.nan?)
        assert_equal(0.0, vessel_pos.speed_over_ground)
        assert(vessel_pos.yaw.rad.nan?)

        assert_equal(710_033_370, vessel_info.mmsi)
        assert_equal(0, vessel_info.imo)
        assert_equal("SAAM ELECTRAA", vessel_info.name)
        assert_in_delta(32, vessel_info.length, 1e-6)
        assert_in_delta(12, vessel_info.width, 1e-6)
        assert_in_delta(12, vessel_info.reference_position.x, 1e-6)
        assert_in_delta(-6, vessel_info.reference_position.y, 1e-6)
        assert_equal(:SHIP_TYPE_TUG, vessel_info.ship_type)
        assert_equal(:EPFD_GPS, vessel_info.epfd_fix)
    end

    it "outputs the class A static and voyage related data" do
        syskit_configure_and_start(@task)
        payload = [
            5,
            218,
            63,
            82,
            42,
            106,
            181,
            150,
            0,
            80,
            85,
            53,
            54,
            57,
            57,
            64,
            83,
            65,
            65,
            77,
            32,
            69,
            76,
            69,
            67,
            84,
            82,
            65,
            65,
            64,
            64,
            64,
            64,
            64,
            64,
            64,
            52,
            64,
            1,
            120,
            0,
            60,
            0,
            120,
            0,
            215,
            80,
            0,
            204,
            191,
            25,
            78,
            2,
            80,
            65,
            82,
            65,
            78,
            65,
            71,
            85,
            65,
            32,
            80,
            82,
            64,
            64,
            64,
            64,
            64,
            64,
            64,
            64,
            2,
            225
        ]

        voyage_information, vessel_info =
            expect_execution { nmea2000_write pgn: 129_794, payload: payload }
            .to do
                [have_one_new_sample(task.voyage_information_port),
                 have_one_new_sample(task.vessel_information_port)]
            end

        assert_equal(710_033_370, vessel_info.mmsi)
        assert_equal(9_876_842, vessel_info.imo)
        assert_equal("SAAM ELECTRAA", vessel_info.name)
        assert_equal("PU5699", vessel_info.call_sign)
        assert_in_delta(32, vessel_info.length, 1e-6)
        assert_in_delta(12, vessel_info.width, 1e-6)
        assert_in_delta(12, vessel_info.reference_position.x, 1e-6)
        assert_in_delta(-6, vessel_info.reference_position.y, 1e-6)
        assert_in_delta(5.9, vessel_info.draft, 1e-6)
        assert_equal(:SHIP_TYPE_TUG, vessel_info.ship_type)
        assert_equal(:EPFD_UNDEFINED, vessel_info.epfd_fix)

        assert_equal(9_876_842, voyage_information.imo)

        assert_equal(1_788_091_200_000, voyage_information.eta.to_r * 1e3)
        assert_equal("PARANAGUA PR", voyage_information.destination)
    end

    def nmea2000_write(time: Time.now, priority: 0, destination: 0xff,
        source: 0, pgn:, payload:)
        message = Types.nmea2000.Message.new(
            time: time,
            priority: priority,
            destination: destination,
            source: source,
            pgn: pgn,
            size: 223,
            payload: payload + [0] * (223 - payload.size)
        )
        syskit_write task.msg_in_port, message
    end
end
