/*
 * opencog/atomspace/SimpleTruthValue.cc
 *
 * Copyright (C) 2002-2007 Novamente LLC
 * All Rights Reserved
 *
 * Written by Welter Silva <welter@vettalabs.com>
 *            Guilherme Lamacie

 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License v3 as
 * published by the Free Software Foundation and including the exceptions
 * at http://opencog.org/wiki/Licenses
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program; if not, write to:
 * Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#include "SimpleTruthValue.h"

#include <math.h>

#include <opencog/util/platform.h>
#include <opencog/util/exceptions.h>

using namespace opencog;

#define KKK 800.0

SimpleTruthValue::SimpleTruthValue(strength_t m, count_t c)
{
    mean = m;
    count = c;
}

SimpleTruthValue::SimpleTruthValue(const TruthValue& source)
{
    mean = source.getMean();
    count = source.getCount();
}
SimpleTruthValue::SimpleTruthValue(SimpleTruthValue const& source)
{
    mean = source.mean;
    count = source.count;
}

void SimpleTruthValue::setMean(strength_t m)
{
    mean = m;
}

void SimpleTruthValue::setCount(count_t c)
{
    count = c;
}

void SimpleTruthValue::setConfidence(confidence_t c)
{
    count = confidenceToCount(c);
}

strength_t SimpleTruthValue::getMean() const
{
    return mean;
}

count_t SimpleTruthValue::getCount() const
{
    return count;
}

confidence_t SimpleTruthValue::getConfidence() const
{
    return countToConfidence(count);
}

float SimpleTruthValue::toFloat() const
{
    return static_cast<float>(getMean());
}

std::string SimpleTruthValue::toString() const
{
    char buf[1024];
    // TODO: confidence is not needed for Saving&Loading.
    // (Only count is saved). So, for saving memory space
    // in dump files, it should be removed. However, toString
    // is being used for debug purposes, so both need to be shown...
    sprintf(buf, "[%f,%f=%f]",
            static_cast<float>(getMean()),
            static_cast<float>(getCount()),
            static_cast<float>(getConfidence()));
    return buf;
}

SimpleTruthValue* SimpleTruthValue::clone() const
{
    return new SimpleTruthValue(*this);
}

SimpleTruthValue& SimpleTruthValue::operator=(const TruthValue & rhs)
    throw (RuntimeException)
{
    const SimpleTruthValue* tv = dynamic_cast<const SimpleTruthValue*>(&rhs);
    if (tv) {
        if (tv != this) { // check if this is the same object first.
            mean = tv->mean;
            count = tv->count;
        }
    } else {
#ifndef WIN32
        // The following line was causing a compilation error on MSVC...
        throw RuntimeException(TRACE_INFO,
              "Cannot assign a TV of type '%s' to one of type '%s'\n",
              typeid(rhs).name(), typeid(*this).name());
#else
        throw RuntimeException(TRACE_INFO,
              "SimpleTruthValue - Invalid assignment of a SimpleTV object.");
#endif

    }
    return *this;
}

bool SimpleTruthValue::operator==(const TruthValue& rhs) const
{
    const SimpleTruthValue *stv = dynamic_cast<const SimpleTruthValue *>(&rhs);
    if (NULL == stv) return false;
    if (mean != stv->mean) return false;
    if (count != stv->count) return false;
    return true;
}

TruthValueType SimpleTruthValue::getType() const
{
    return SIMPLE_TRUTH_VALUE;
}

float SimpleTruthValue::confidenceToCount(confidence_t c)
{
    c = std::min(c, 0.9999999f);
    return static_cast<count_t>(KKK * c / (1.0 - c));
}

float SimpleTruthValue::countToConfidence(count_t c)
{
    return static_cast<confidence_t>(c / (c + KKK));
}

SimpleTruthValue* SimpleTruthValue::fromString(const char* tvStr)
{
    float mean, count, conf;
    // TODO: confidence is not needed for Saving&Loading.
    // (Only count is saved). So, for saving memory space
    // in dump files, it should be removed. However, toString
    // is being used for debug purposes, so both need to be shown...
    sscanf(tvStr, "[%f,%f=%f]", &mean, &count, &conf);
    //printf("SimpleTruthValue::fromString(%s) => mean = %f, count = %f, conf = %f\n", tvStr, mean, count, conf);
    return new SimpleTruthValue(static_cast<strength_t>(mean),
                                static_cast<count_t>(count));
}
