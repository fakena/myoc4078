/*
 * opencog/learning/moses/moses/knobs.h
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
#ifndef _MOSES_KNOBS_H
#define _MOSES_KNOBS_H

#include <opencog/util/tree.h>
#include <opencog/util/exceptions.h>
#include <opencog/util/numeric.h>
#include <opencog/util/based_variant.h>

#include <opencog/comboreduct/reduct/reduct.h>
#include <opencog/comboreduct/combo/complexity.h>
#include <opencog/comboreduct/ant_combo_vocabulary/ant_combo_vocabulary.h>

#include "using.h"
#include <opencog/learning/moses/eda/field_set.h>
#include <bitset>


namespace moses
{

using namespace ant_combo;

//a knob represents a single dimension of variation relative to an exemplar
//program tree. This may be discrete or continuous. In the discrete case, the
//various settings are accessible via turn(0),turn(1),...turn(arity()-1). In
//the continuous case, turn(contin_t) is used.
//
//for example, given the program tree fragment or(0<(*(#1,0.5)),#2), a
//continuous knob might be used to vary the numerical constant. So setting
//this knob to 0.7 would transform the tree fragment to
//or(0<(*(#1,0.7)),#2). A discrete knob with arity()==3 might be used to
//transform the boolean input #2. So setting this knob to 1 might transform
//the tree to or(0<(*(#1,0.7)),not(#2)), and setting it to 2 might remove it
//from the tree (while setting it to 0 would return to the original tree).

struct knob_base {
    knob_base(combo_tree& tr) : _tr(&tr) { }
    virtual ~knob_base() { }

    //is the feature nonzero by default? i.e., is it present in the exemplar?
    virtual bool in_exemplar() const = 0;

    //return the exemplar to its state before the knob was created (deleting
    //any null vertices if present)
    virtual void clear_exemplar() = 0;
protected:
    combo_tree* _tr;
};

struct disc_knob_base : public knob_base {
    disc_knob_base(combo_tree& tr) : knob_base(tr) { }
    virtual ~disc_knob_base() { }

    virtual void turn(int) = 0;
    virtual void disallow(int) = 0;
    virtual void allow(int) = 0;

    //create a spec describing the space spanned by the knob
    virtual eda::field_set::disc_spec spec() const = 0;

    //arity based on whatever knobs are currently allowed
    virtual int arity() const = 0;

    //expected complexity based on whatever the knob is currently turned to
    virtual int complexity_bound() const = 0;
};

struct contin_knob : public knob_base {
    contin_knob(combo_tree& tr, combo_tree::iterator tgt,
                contin_t step_size, contin_t expansion,
                eda::field_set::arity_t depth)
        : knob_base(tr), _tgt(tgt), _spec(combo::get_contin(*tgt),
                                          step_size, expansion, depth) { }

    bool in_exemplar() const {
        return true;
    }
    void clear_exemplar() { }

    void turn(eda::contin_t x) {
        *_tgt = x;
    }

    //create a spec describing the space spanned by the knob
    const eda::field_set::contin_spec& spec() const {
        return _spec;
    }
protected:
    combo_tree::iterator _tgt;
    eda::field_set::contin_spec _spec;
};

template<int MaxArity>
struct knob_with_arity : public disc_knob_base {
    knob_with_arity(combo_tree& tr) : disc_knob_base(tr), _default(0) { }

    void disallow(int idx) {
        _disallowed[idx] = true;
    }
    void allow(int idx) {
        _disallowed[idx] = false;
    }

    int arity() const {
        return MaxArity -_disallowed.count();
    }

    bool in_exemplar() const {
        return (_default != 0);
    }
protected:
    std::bitset<MaxArity> _disallowed;
    int _default;

    int map_idx(int idx) const {
        if (idx == _default)
            idx = 0;
        else if (idx == 0)
            idx = _default;
        return idx + (_disallowed << (MaxArity - idx)).count();
    }
};

//note - children aren't cannonized when parents are called
struct logical_subtree_knob : public knob_with_arity<3> {
    static const int absent = 0;
    static const int present = 1;
    static const int negated = 2;

    logical_subtree_knob(combo_tree& tr, combo_tree::iterator tgt,
                         combo_tree::iterator subtree)
            : knob_with_arity<3>(tr), _current(absent), _loc(tr.end()) {

        //compute the negation of the subtree
        combo_tree negated_subtree(subtree);
        negated_subtree.insert_above(negated_subtree.begin(), id::logical_not);
        reduct::logical_reduce(negated_subtree);

        for (combo_tree::sibling_iterator sib = tgt.begin();
             sib != tgt.end();++sib) {
            if (_tr->equal_subtree(combo_tree::iterator(sib), subtree) ||
                _tr->equal_subtree(combo_tree::iterator(sib),
                                   negated_subtree.begin())) {
                _loc = sib;
                _current = present;
                _default = present;
                return;
            }
        }

        _loc = _tr->append_child(tgt, id::null_vertex);
        _tr->append_child(_loc, subtree);
    }

    int complexity_bound() const {
        return (_current == absent ? 0 : combo::complexity(_loc));
    }

    void clear_exemplar() {
        if (in_exemplar())
            turn(0);
        else
            _tr->erase(_loc);
    }

    void turn(int idx) {
        idx = map_idx(idx);
        OC_ASSERT((idx < 3), "Index greater than 3.");

        if (idx == _current) //already set, nothing to
            return;

        switch (idx) {
        case absent:
            //flag subtree to be ignored with a null_vertex, replace negation if present
            if (_current == negated)
                *_loc = id::null_vertex;
            else
                _loc = _tr->insert_above(_loc, id::null_vertex);
            break;
        case present:
            _loc = _tr->erase(_tr->flatten(_loc));
            break;
        case negated:
            if (_current == present)
                _loc = _tr->insert_above(_loc, id::logical_not);
            else
                *_loc = id::logical_not;
            break;
        }

        _current = idx;
    }

    eda::field_set::disc_spec spec() const {
        return eda::field_set::disc_spec(arity());
    }

protected:
    int _current;
    combo_tree::iterator _loc;
};


#define MAX_PERM_ACTIONS 128

//note - children aren't cannonized when parents are called
struct action_subtree_knob : public knob_with_arity<MAX_PERM_ACTIONS> {

    typedef combo_tree::pre_order_iterator pre_it;

    action_subtree_knob(combo_tree& tr, combo_tree::iterator tgt,
                        vector<combo_tree>& perms)
        : knob_with_arity<MAX_PERM_ACTIONS>(tr), _current(0), _loc(tr.end()), _perms(perms) {

        OC_ASSERT(((int)_perms.size() < MAX_PERM_ACTIONS),
                         "Too many perms.");

        for (int i = _perms.size() + 1;i < MAX_PERM_ACTIONS;i++)
            disallow(i);

        _default = 0;
        _current = _default;
        _loc = _tr->append_child(tgt, id::null_vertex);
    }


    int complexity_bound() const {
        return combo::complexity(_loc);
    }

    void clear_exemplar() {
        if (in_exemplar())
            turn(0);
        else
            _tr->erase(_loc);
    }


    void turn(int idx) {
        idx = map_idx(idx);
        OC_ASSERT((idx <= (int)_perms.size()),
                         "Index too big.");

        if (idx == _current) //already set, nothing to
            return;

        if (idx == 0) {
            if (_current != 0) {
                combo_tree t(id::null_vertex);
                _loc = _tr->replace(_loc, t.begin());
            }
        } else {
            pre_it ite = (_perms[idx-1]).begin();
            _loc = _tr->replace(_loc, ite);
        }
        _current = idx;
    }


    eda::field_set::disc_spec spec() const {
        return eda::field_set::disc_spec(arity());
    }

protected:
    int _current;
    combo_tree::iterator _loc;
    const vector<combo_tree> _perms;
};



//note - children aren't cannonized when parents are called
struct ant_action_subtree_knob : public knob_with_arity<4> {
    static const int none    = 0;
    static const int forward = 1;
    static const int rleft   = 2;
    static const int rright  = 3;

    ant_action_subtree_knob(combo_tree& tr, combo_tree::iterator tgt,
                            combo_tree::iterator subtree)
            : knob_with_arity<4>(tr), _current(none), _loc(tr.end()) {

        _default = none;
        _current = _default;

        _loc = _tr->append_child(tgt, id::null_vertex);
        _tr->append_child(_loc, subtree);
    }

    int complexity_bound() const {
        return combo::complexity(_loc);
    }

    void clear_exemplar() {
        if (in_exemplar())
            turn(0);
        else
            _tr->erase(_loc);
    }

    void turn(int idx) {
        idx = map_idx(idx);
        OC_ASSERT((idx < 4), "Index greater than 3.");

        if (idx == _current) //already set, nothing to
            return;

        if (idx == none) {
            if (_current != none)
                _loc = _tr->insert_above(_loc, id::null_vertex);
        } else {
            if (_current == none)
                _tr->erase_children(_loc);

            switch (idx) {
            case forward:
                *_loc = instance(id::move_forward);
                break;

            case rleft:
                *_loc = instance(id::turn_left);
                break;

            case rright:
                *_loc = instance(id::turn_right);
                break;

            default:
                break;
            }
        }

        _current = idx;
    }

    eda::field_set::disc_spec spec() const {
        return eda::field_set::disc_spec(arity());
    }

protected:
    int _current;
    combo_tree::iterator _loc;
};



struct simple_action_subtree_knob : public knob_with_arity<2> {
    static const int present = 0;
    static const int absent = 1;

    simple_action_subtree_knob(combo_tree& tr, combo_tree::iterator tgt)
            : knob_with_arity<2>(tr), _loc(tgt) {

        t = combo_tree(tgt);

        _current = present;
        _default = present;
    }

    int complexity_bound() const {
        return (_current == absent ? 0 : combo::complexity(_loc));
    }

    void clear_exemplar() {
//      if (in_exemplar())
        turn(0);
//      else
// _tr->erase(_loc);
    }

    void turn(int idx) {
        idx = map_idx(idx);
        OC_ASSERT((idx < 2), "Index greater than 2.");

        if (idx == _current) //already set, nothing to
            return;

        switch (idx) {
        case present:
            _loc = _tr->erase(_tr->flatten(_loc));
            break;
        case absent:
            _loc = _tr->insert_above(_loc, id::null_vertex);
            break;
        }

        _current = idx;
    }

    eda::field_set::disc_spec spec() const {
        return eda::field_set::disc_spec(arity());
    }

protected:
    int _current;
    combo_tree::iterator _loc;
    combo_tree t;

};



typedef opencog::based_variant < boost::variant<logical_subtree_knob, action_subtree_knob, simple_action_subtree_knob>,
disc_knob_base > disc_knob;
} //~namespace moses

#endif

