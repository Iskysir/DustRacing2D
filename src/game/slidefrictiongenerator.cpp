// This file is part of Dust Racing (DustRAC).
// Copyright (C) 2011 Jussi Lind <jussi.lind@iki.fi>
//
// DustRAC is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// DustRAC is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with DustRAC. If not, see <http://www.gnu.org/licenses/>.

#include "slidefrictiongenerator.h"
#include "MiniCore/Core/MCObject"
#include "MiniCore/Core/MCMathUtil"
#include "MiniCore/Core/MCTrigonom"

namespace
{
    const MCFloat FRICTION_SPEED_TH = 0.001f;
}

SlideFrictionGenerator::SlideFrictionGenerator(
    MCFloat coeff, MCFloat gravity)
: MCFrictionGenerator(coeff, gravity)
{
}

void SlideFrictionGenerator::updateForce(MCObject * p)
{
    const MCUint bodyNormalAngle = p->angle() + 90;
    const MCVector2d<MCFloat> n(
        MCTrigonom::cos(bodyNormalAngle),
        MCTrigonom::sin(bodyNormalAngle));
    const MCVector2d<MCFloat> & v = p->velocity();
    MCVector2d<MCFloat> s = MCMathUtil::projection(v, n);

    if (s.lengthFast() > FRICTION_SPEED_TH)
    {
        p->addForce(-s * coeff() * gravity() * p->mass());
    }
}

SlideFrictionGenerator::~SlideFrictionGenerator()
{
}