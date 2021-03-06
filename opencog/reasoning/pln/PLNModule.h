/*
 * opencog/reasoning/pln/PLNModule.h
 *
 * Copyright (C) 2008 by Singularity Institute for Artificial Intelligence
 * All Rights Reserved
 *
 * Written by Jared Wigmore <jared.wigmore@gmail.com>
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

#ifndef _OPENCOG_PLN_MODULE_H
#define _OPENCOG_PLN_MODULE_H

#include <string>

#include <opencog/server/CogServer.h>
#include <opencog/server/Factory.h>
#include <opencog/server/Module.h>
#include <opencog/server/Request.h>

#include "BackInferenceTreeNode.h"
#include "BackChainingAgent.h"
#include "ForwardChainingAgent.h"
#include "FitnessEvaluator.h"

namespace opencog {

class CogServer;

class PLNModule : public Module
{
private:
    static const char* usageInfo;

    DECLARE_CMD_REQUEST(PLNModule, "pln", do_pln, 
        "Run a PLN command",usageInfo, false); 

    Factory<BackChainingAgent, Agent> backChainingFactory;
    Factory<ForwardChainingAgent, Agent> forwardChainingFactory;

    const std::string* DEFAULT()
    {
        static const std::string defaultConfig[] = {
            "PLN_RECORD_TRAILS",     "true",
            "PLN_LOG_LEVEL",         "2",
            "PLN_FW_VARS_IN_ATOMSPACE", "true",
            "PLN_PRINT_REAL_ATOMS",  "true",
            "PLN_FITNESS_EVALUATOR", "best",
            "",                      ""
        };
        return defaultConfig;
    }

    void setParameters(const std::string* params);

    /**
     * That method is used to wrapped in a scheme function.
     * It calls infer, returns the Handle of the resulting atom if the
     * inference is successful, or the undefined Handle otherwise.
     */
    Handle pln_bc(Handle h, int steps);

    /**
     * That method is used to wrapped in a scheme function.
     * It calls applyRule
     */
    Handle pln_ar(const std::string& ruleName, const HandleSeq& premises);

public:

    virtual const ClassInfo& classinfo() const { return info(); }
    static const ClassInfo& info() {
        static const ClassInfo _ci("opencog::PLNModule");
        return _ci;
    }

    static inline const char* id();

    bool recordingTrails;
    
    PLNModule();
    ~PLNModule();
    void init();

    /** Process command request from do_pln
     *
     * @todo Need to add commands for exploring the AtomSpace as viewed
     * through the AtomSpaceWrapper.
     * @todo Need to add commands for controlling multiple roots/targets.
     */
    std::string runCommand(std::list<std::string> args);

    FitnessEvaluatorT fitnessEvaluator;

}; // class

#if 0
/** Takes a real handle, and sets the backward-chaining target to that atom.
 */
void setTarget(Handle h);
#endif

namespace pln {

/** Does steps inference steps on target h. Optionally sets the target used in
 * the PLN cogserver commands.
 * Intended for use by the pln-bc Scheme command; this is currently set up for
 * practical use rather than testing use, and as such will not stop at the
 * first result it finds.
 *
 * @param h The handle to do inference on (a normal handle, not a PLN handle)
 * @param steps Takes the maximum number of steps allowed, and stores the
 * number of steps performed
 * @param setTarget If true, the BIT created here will be/replace the one
 * used with the 'pln' cogserver command.
 * @return The (normal) handle of the result.
 */
Handle infer(Handle h, int &steps, bool setTarget);

/**
 * that function apply a PLN inference rule given its name and its premises
 * to produce the conclusion. The set of rules is taken from
 * DefaultVariableRuleProvider.
 * If the name rule does not correspond to any declared rules
 * then the Handle returned is UNDEFINED_HANDLE
 *
 * @param rule_name the name of PLN rule to apply
 * @param premises the list of Handle premises
 *
 * @return the Handle of the conclusion. Handle::UNDEFINED if no rule
 *         corresponds to ruleName
 */
Handle applyRule(const std::string& ruleName, const HandleSeq& premises);

}} // ~namespace opencog::pln


#endif // _OPENCOG_PLN_MODULE_H

