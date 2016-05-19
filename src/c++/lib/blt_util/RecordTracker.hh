// -*- mode: c++; indent-tabs-mode: nil; -*-
//
// Strelka - Small Variant Caller
// Copyright (c) 2009-2016 Illumina, Inc.
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

///
/// \author Mitch Bekritsky
///

#pragma once

#include "blt_util/known_pos_range2.hh"
#include "blt_util/blt_types.hh"

#include "htsapi/vcf_record.hh"

#include "starling_common/indel_core.hh"
#include "starling_common/starling_diploid_indel.hh"
#include "starling_common/indel_key.hh"
#include "starling_common/indel_data.hh"

#include "boost/icl/discrete_interval.hpp"
#include "boost/icl/interval_map.hpp"


#include <iosfwd>
#include <set>

struct IndelVariant
{
    INDEL::index_t type;
    unsigned length;
    std::string insert_sequence;
    std::string alt_string;
    std::string ref_string;
};

std::ostream&
operator<<(
    std::ostream& os,
    const IndelVariant& var);

struct IndelVariantSort
{
    bool
    operator()(
        const IndelVariant& lhs,
        const IndelVariant& rhs)
    {
        // respect the enum ordering in indel_core
        if (lhs.type < rhs.type) return true;
        if (lhs.type == rhs.type)
        {
            // if both indels have the same type, the shorter one
            // is less than the larger one
            if (lhs.length < rhs.length) return true;
        }

        return false;
    }
};


struct IndelGenotype
{
    // std::set<IndelVariant, IndelVariantSort> variants;
    bool
    operator==(const IndelGenotype& rhs) const
    {
        if (vcfr.pos == rhs.vcfr.pos && genotype == rhs.genotype) return true;
        return false;
    }

    bool altMatch(
        const indel_key& ik,
        const indel_data& id) const;

    vcf_record vcfr;
    pos_t pos;
    std::set<IndelVariant, IndelVariantSort> alts;
    STAR_DIINDEL::index_t genotype = STAR_DIINDEL::NOINDEL;
};

std::ostream&
operator<<(
    std::ostream& os,
    const IndelGenotype& gt);

struct IndelGenotypeSort
{
    bool
    operator()(
        const IndelGenotype& lhs,
        const IndelGenotype& rhs)
    {

        if (lhs.vcfr.pos < rhs.vcfr.pos) return true;
        if (lhs.vcfr.pos == rhs.vcfr.pos)
        {
            // respect STAR_DIINDEL genotype ordering
            if (lhs.genotype < rhs.genotype) return true;
        }

        // I'm assuming that VCF records beginning at the same position
        // do not have the same genotype, i.e. there's no such thing as
        // two records starting at the same site with the same genotype
        return false;
    }
};


struct RecordTracker
{
    typedef std::set<IndelGenotype, IndelGenotypeSort> indel_value_t;

    bool
    empty() const
    {
        return _records.empty();
    }

    void
    clear()
    {
        _records.clear();
    }

    bool
    intersectingRecord(
        const pos_t pos,
        indel_value_t& records) const
    {
        return intersectingRecordImpl(pos, pos + 1, records);
    }

    bool
    intersectingRecord(
        const known_pos_range2 range,
        indel_value_t& records) const
    {
        return intersectingRecordImpl(range.begin_pos(), range.end_pos(), records);
    }

    void
    addVcfRecord(
        const vcf_record& vcfRecord);

    void
    dump(
        std::ostream& os) const;

    unsigned
    size() const
    {
        return _records.size();
    }

private:
    typedef boost::icl::interval_map<pos_t, indel_value_t> indel_record_t;
    typedef boost::icl::discrete_interval<pos_t> interval_t;

    bool
    intersectingRecordImpl(
        const pos_t beginPos,
        const pos_t endPos,
        RecordTracker::indel_value_t& records) const;

    indel_record_t _records;
};