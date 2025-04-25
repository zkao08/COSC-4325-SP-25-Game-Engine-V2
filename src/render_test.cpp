#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <vector>
#include <random>
#include <memory>
#include "Application.h"

int main()
{
    int status = 0;

    // Initialize the GUI and renderer
    std::unique_ptr<Application> app = std::make_unique<Application>();
    status = app->Execute();

    return status;
}