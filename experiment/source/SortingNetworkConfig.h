#ifndef SORTING_NETWORK_CONFIG_H
#define SORTING_NETWORK_CONFIG_H

#include "config/config.h"

EMP_BUILD_CONFIG(SortingNetworkConfig, 
  GROUP(DEFAULT_GROUP, "General settings"),
  VALUE(PLACEHOLDER, size_t, 0, "placeholder settings")
)

#endif