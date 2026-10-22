// Compile the upstream accessor unchanged, without registering its write-only native.
#include "game/Transform.h"
#undef SCAN_NATIVE
#define SCAN_NATIVE(...)
#include "game/Transform.cpp"
