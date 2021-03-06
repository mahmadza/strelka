//
// Strelka - Small Variant Caller
// Copyright (c) 2009-2017 Illumina, Inc.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//

#pragma once

#include "blt_util/PolymorphicObject.hh"

#include <vector>


/// \brief Base for any variant scoring model producing an empirical variant probability given an input
///        array of predictive features
///
struct VariantScoringModelBase : public PolymorphicObject
{
    typedef std::vector<double> featureInput_t;

    virtual
    double
    getProb(const featureInput_t& features) const = 0;
};

