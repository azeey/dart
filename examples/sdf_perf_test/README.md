This project is dependent on DART. Please make sure a proper version of DART is 
installed before building this project.

## Description

This utility loads a custom Gazebo SDFormat (`.sdf`) world file directly into DART, runs a headless simulation loop for a specified number of steps, and measures the Real-Time Factor (RTF) and average step time. This is useful for profiling DART's physics simulation performance independently from Gazebo or any GUI rendering.

## Build Instructions

From the root DART directory:

    $ pixi run config
    $ cmake --build build/default/cpp/Release --target sdf_perf_test

Alternatively, to build it standalone from this directory:

    $ mkdir build
    $ cd build
    $ cmake ..
    $ make

## Execute Instructions

Run the compiled executable with the path to the SDF world file and (optionally) the number of simulation steps to run:

    $ ./sdf_perf_test <sdf_file_path> [num_steps]

Example:

    $ ./sdf_perf_test data/sdf/double_pendulum.world 5000
