/*
 * SPDX-License-Identifier: GPL-3.0-only
 * MuseScore-CLA-applies
 *
 * MuseScore
 * Music Composition & Notation
 *
 * Copyright (C) 2021 MuseScore BVBA and others
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef MU_IMPORTEXPORT_MUSICXMLWRITER_H
#define MU_IMPORTEXPORT_MUSICXMLWRITER_H

#include "project/inotationwriter.h"

namespace mu::iex::musicxml {
class MusicXmlWriter : public project::INotationWriter
{
public:

    std::vector<UnitType> supportedUnitTypes() const override;
    bool supportsUnitType(UnitType unitType) const override;

    Ret write(notation::INotationPtr notation, io::IODevice& dstDevice, const Options& options = Options()) override;
    Ret writeList(const notation::INotationPtrList& notations, io::IODevice& dstDevice, const Options& options = Options()) override;
};
}

#endif // MU_IMPORTEXPORT_MUSICXMLWRITER_H
