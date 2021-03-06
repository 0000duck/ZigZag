#pragma once

#include <chrono>
#include <cstdint>



using namespace std::literals;


class FpsMonitor
{

public:

    FpsMonitor(std::chrono::milliseconds measurement_interval_ms)
        : measurement_interval(measurement_interval_ms)
    {
        last_measurement = clock.now();
    }


    void frame()
    {
        auto now = clock.now();
        ++current_frame_count;
        ++every_frame_count;

        if (now > last_measurement + measurement_interval)
        {
            auto time_passed_micro = (now - last_measurement).count() / 1000;
            auto time_passed = time_passed_micro / 1'000'000.0;
            framerate = static_cast<double>(current_frame_count) / time_passed;

            current_frame_count = 0;
            last_measurement = now;
        }
    }


    unsigned long long frame_count() const
    {
        return every_frame_count;
    }


    double fps() const
    {
        return framerate;
    }



private:

    double framerate = 0.0;
    unsigned long long current_frame_count = 0;
    unsigned long long every_frame_count = 0;

    std::chrono::milliseconds measurement_interval;
    std::chrono::steady_clock::time_point last_measurement;

    std::chrono::steady_clock clock;

};
