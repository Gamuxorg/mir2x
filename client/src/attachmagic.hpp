#pragma once
#include <cmath>
#include <string_view>
#include "fflerror.hpp"
#include "magicbase.hpp"
#include "magicrecord.hpp"

class AttachMagic: public MagicBase
{
    public:
        AttachMagic(const char8_t *magicName, const char8_t *magicStage, int gfxDirIndex = 0)
            : MagicBase(magicName, magicStage, gfxDirIndex)
        {
            fflassert(m_gfxEntry.checkType(u8"附着"));
        }

    public:
        virtual void drawShift(int, int, uint32_t) const;
};

class Thunderbolt: public AttachMagic
{
    public:
        Thunderbolt()
            : AttachMagic(u8"雷电术", u8"运行")
        {}

    public:
        void drawShift(int, int, uint32_t) const override;
};

class TaoYellowBlueRing: public AttachMagic
{
    public:
        TaoYellowBlueRing()
            : AttachMagic(u8"阴阳法环", u8"运行")
        {}

    public:
        void drawShift(int, int, uint32_t) const;
};
