/*
 * Copyright (C) 2002-2007 Novamente LLC
 * Copyright (C) 2008 by Singularity Institute for Artificial Intelligence
 * All Rights Reserved
 *
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
 */

#ifndef AND2ORRULE_H
#define AND2ORRULE_H

namespace opencog { namespace pln {
class AND2ORRule : public Rule
{
    AND2ORRule(iAtomSpaceWrapper *_asw)
	: Rule(_asw, false, true, "AND2ORRule")
    {
        inputFilter.push_back(Btr<atom>(new atom(__INSTANCEOF_N,
                                                 1, new atom(AndLink))));
    }
    Rule::setOfMPs o2iMetaExtra(meta outh, bool& overrideInputFilter) const
    {
        Btr<MPs> ret(new MPs);
        ret->push_back(Btr<atom>(new atom(neBoundVertexWithNewType(outh,
                                                                   AndLink))));
        return makeSingletonSet(ret);
    }
    
    virtual atom i2oType(Handle* h, const int n) const
    {
        assert(1==n);

        /*return atomWithNewType(h[0], OR_LINK);*/
    }
    virtual bool valid(Handle* h, const int n) const
    {
        return isSubType(h[0], AND_LINK);
    }

    BoundVertex compute(const vector<Vertex>& premiseArray,
                        Handle CX = NULL,
                        bool fresh = true) const
    {
        assert(n==1);
        return AND2ORLink(premiseArray[0]);
    }
};

}} // namespace opencog { namespace pln {
#endif // AND2ORRULE_H
