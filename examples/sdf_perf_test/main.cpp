/*
 * Copyright (c) 2011, The DART development contributors
 * All rights reserved.
 *
 * This file is provided under the "BSD-style" License.
 */

#include <dart/dart.hpp>
#include <dart/utils/sdf/SdfParser.hpp>

#include <chrono>
#include <filesystem>
#include <iostream>
#include <string>

void printUsage(const std::string& programName)
{
  std::cout << "Usage: " << programName << " <sdf_file_path> [num_steps]\n"
            << "  <sdf_file_path>: Path to the Gazebo SDFormat (.sdf) world file.\n"
            << "  [num_steps]:     Optional number of simulation steps to run (default: 1000).\n";
}

int main(int argc, char* argv[])
{
  if (argc < 2 || argc > 3)
  {
    printUsage(argv[0]);
    return 1;
  }

  std::string sdfPath = argv[1];
  std::filesystem::path p(sdfPath);
  if (!p.is_absolute())
  {
    p = std::filesystem::absolute(p);
  }
  std::string absoluteSdfPath = p.string();

  std::size_t numSteps = 1000;
  if (argc == 3)
  {
    try
    {
      numSteps = std::stoul(argv[2]);
    }
    catch (const std::exception& e)
    {
      std::cerr << "Invalid number of steps: " << argv[2] << "\n";
      printUsage(argv[0]);
      return 1;
    }
  }

  std::cout << "Loading SDF world file: " << absoluteSdfPath << "\n";
  dart::simulation::WorldPtr world = dart::utils::SdfParser::readWorld(absoluteSdfPath);

  if (!world)
  {
    std::cerr << "Failed to load SDF world file: " << absoluteSdfPath << "\n";
    return 1;
  }

  std::cout << "World loaded successfully.\n";
  std::cout << "  World Name: " << world->getName() << "\n";
  std::cout << "  Gravity: [" << world->getGravity().transpose() << "] m/s^2\n";
  std::cout << "  Time Step: " << world->getTimeStep() << " s\n";
  std::cout << "  Number of Skeletons (Objects): " << world->getNumSkeletons() << "\n";

  // Check how many DOFs / bodies we have in total
  std::size_t totalDofs = 0;
  std::size_t totalBodyNodes = 0;
  for (std::size_t i = 0; i < world->getNumSkeletons(); ++i)
  {
    auto skel = world->getSkeleton(i);
    if (skel)
    {
      totalDofs += skel->getNumDofs();
      totalBodyNodes += skel->getNumBodyNodes();
    }
  }
  std::cout << "  Total Bodies: " << totalBodyNodes << "\n";
  std::cout << "  Total Degrees of Freedom (DOFs): " << totalDofs << "\n";

  std::cout << "\nSimulating " << numSteps << " steps...\n";

  const double timeStep = world->getTimeStep();
  auto startTime = std::chrono::high_resolution_clock::now();

  for (std::size_t step = 0; step < numSteps; ++step)
  {
    world->step();
  }

  auto endTime = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> wallTime = endTime - startTime;

  double simTime = numSteps * timeStep;
  double rtf = simTime / wallTime.count();
  double avgStepTimeMs = (wallTime.count() * 1000.0) / numSteps;

  std::cout << "\n--- Performance Results ---\n";
  std::cout << "Simulated Time:     " << simTime << " s\n";
  std::cout << "Wall-Clock Time:    " << wallTime.count() << " s\n";
  std::cout << "Real-Time Factor:   " << rtf << "\n";
  std::cout << "Avg Step Time:      " << avgStepTimeMs << " ms/step\n";

  return 0;
}
