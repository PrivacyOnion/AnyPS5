#pragma once

#include "SceTypes.hpp"

extern "C" {

int APS5_VABI sceDeci4hClose();
int APS5_VABI sceDeci4hCreateHostProcess();
int APS5_VABI sceDeci4hCreateHostProcessAndWait();
int APS5_VABI sceDeci4hGetStatus();
int APS5_VABI sceDeci4hOpen();
int APS5_VABI sceDeci4hRead();
int APS5_VABI sceDeci4hWrite();

}
