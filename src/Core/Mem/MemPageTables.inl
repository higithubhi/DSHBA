#ifndef INLINED_INCLUDES
#include "Mem.h"
class Mem_INL : Mem {
#endif
    std::shared_ptr<std::array<u8*, (0x1'0000'0000ULL >> 10)>> _ReadPageTablePtr = [this] {
        auto tablePtr = std::make_shared<std::array<u8*, (0x1'0000'0000ULL >> 10)>>();
        auto& table = *tablePtr;
        for (u32 i = 0; i < 0x0100'0000; i += 0x400) {
            table[((static_cast<u32>(MemoryRegion::eWRAM) << 24) + i) >> 10] = &eWRAM[i & 0x7'ffff];
        }

        for (u32 i = 0; i < 0x0100'0000; i += 0x400) {
            table[((static_cast<u32>(MemoryRegion::iWRAM) << 24) + i) >> 10] = &iWRAM[i & 0x7fff];
        }

        for (u32 i = 0; i < 0x0100'0000; i += 0x400) {
            table[((static_cast<u32>(MemoryRegion::PAL) << 24) + i) >> 10] = &PAL[i & 0x3ff];
        }

        for (u32 i = 0; i < 0x0100'0000; i += 0x400) {
            table[((static_cast<u32>(MemoryRegion::VRAM) << 24) + i) >> 10] = &VRAM[MaskVRAMAddress(i)];
        }

        for (u32 i = 0; i < 0x0100'0000; i += 0x400) {
            table[((static_cast<u32>(MemoryRegion::OAM) << 24) + i) >> 10] = &OAM[i & 0x3ff];
        }

        // ROM is spread out over a way larger area
        for (u32 i = 0; i < 0x0600'0000; i += 0x400) {
            table[((static_cast<u32>(MemoryRegion::ROM_L) << 24) + i) >> 10] = &ROM[i & 0x01ff'ffff];
        }

        return tablePtr;
        }();
    std::array<u8*, (0x1'0000'0000ULL >> 10)>& ReadPageTable = *_ReadPageTablePtr;
    void SetPageTableEEPROM() {
        for (u32 i = 0; i < 0x0600'0000; i += 0x400) {
            // also accounts for ROMSize
            if (IsEEPROMAccess(((static_cast<u32>(MemoryRegion::ROM_L) << 24) + i))) {
                ReadPageTable[((static_cast<u32>(MemoryRegion::ROM_L) << 24) + i) >> 10] = nullptr;
            }
        }
    }

    void SetPageTableNoEEPROM() {
        for (u32 i = 0; i < 0x0600'0000; i += 0x400) {
            ReadPageTable[((static_cast<u32>(MemoryRegion::ROM_L) << 24) + i) >> 10] = &ROM[i & 0x01ff'ffff];
        }
    }

#ifndef INLINED_INCLUDES
};
#endif