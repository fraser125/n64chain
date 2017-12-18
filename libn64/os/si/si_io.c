//
// libn64/include/rcp/vi.h: VI helper functions.
//
// n64chain: A (free) open-source N64 development toolchain.
// Copyright 2014-16 Tyler J. Stachecki <stachecki.tyler@gmail.com>
//
// This file is subject to the terms and conditions defined in
// 'LICENSE', which is part of this source code package.
//
#include <libn64.h>
#include <stdint.h>
#include "si.h"

// Support Serial Accessories that transfer data on an as needed basis instead of every frame.
// ex. Controller Pak (Game Saves), Transfer Pak (GB/GBC), VRU/VRS, EEPROM, GBA?, 64DD RTC
