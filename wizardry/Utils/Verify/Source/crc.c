#include "common-chax.h"
#include "save_core.h"
#include "verification.h"

bool CheckCRC_CHAX(void const * data, int size, u32 crc)
{
    return crc == Checksum16(data, size);
}

void WriteCRC_CHAX(void const * data, int size, u32 * crc)
{
    *crc = Checksum16(data, size);
}
