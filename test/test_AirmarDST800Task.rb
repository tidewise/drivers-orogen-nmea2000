# frozen_string_literal: true

using_task_library "nmea2000"

describe OroGen.nmea2000.AirmarDST800Task do
    run_live

    attr_reader :task

    before do
        @task = syskit_deploy(
            OroGen.nmea2000.AirmarDST800Task
                  .deployed_as("task_under_test")
        )

        @water_depth_pgn = 128_267
        @sequence_id = 0
    end

    it "outputs the converted depth" do
        syskit_configure_and_start(@task)
        payload = [0, 1100, 0, 2].pack("CL<S<C")
        depth = expect_execution { nmea2000_write pgn: 128267, payload: payload }
            .to { have_one_new_sample task.depth_samples_port }

        assert depth.position.x.nan?
        assert depth.position.y.nan?
        assert_equal 11, depth.position.z
    end

    it "validates the depth using on the embedded range" do
        task.properties.range = Float::NAN
        syskit_configure_and_start(task)
        payload = [0, 1100, 0, 1].pack("CL<S<C")
        depth = expect_execution { nmea2000_write pgn: 128267, payload: payload }
            .to { have_one_new_sample task.depth_samples_port }

        assert depth.position.x.nan?
        assert depth.position.y.nan?
        assert depth.position.z.nan?
    end

    it "uses the range property as an override to the embedded range" do
        task.properties.range = 10
        syskit_configure_and_start(task)

        payload = [0, 1100, 0, 2].pack("CL<S<C")
        depth = expect_execution { nmea2000_write pgn: 128267, payload: payload }
            .to { have_one_new_sample task.depth_samples_port }

        assert depth.position.x.nan?
        assert depth.position.y.nan?
        assert depth.position.z.nan?
    end

    def nmea2000_write(time: Time.now, priority: 0, destination: 0xff,
                       source: 0, pgn:, payload:)

        payload = payload.unpack("C*") if payload.respond_to?(:to_str)
        message = Types.nmea2000.Message.new(
            time: time,
            priority: priority,
            destination: destination,
            source: source,
            pgn: pgn,
            payload: payload + [0] * (223 - payload.size)
        )
        syskit_write task.msg_in_port, message
    end
end
