/*
 * opencog/comboreduct/combo/complexity.h
 *
 * Copyright (C) 2002-2008 Novamente LLC
 * All Rights Reserved
 *
 * Written by Moshe Looks
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
#ifndef _COMBO_COMPLEXITY_H
#define _COMBO_COMPLEXITY_H

//various routines dealing with computin the (algorithmic) complexity of
//particular combo programs

#include "vertex.h"

namespace combo {
  typedef int complexity_t ;  //note: complexity is negative with zero being
			      //the best - this allows for simple ordering

  complexity_t complexity(combo_tree::iterator);

} //~namespace combo

#endif
