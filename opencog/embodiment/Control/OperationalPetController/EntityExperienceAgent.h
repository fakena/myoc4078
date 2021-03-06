/*
 * opencog/embodiment/Control/OperationalPetController/EntityExperienceAgent.h
 *
 * Copyright (C) 2009 Novamente LLC
 * All Rights Reserved
 * Author(s): Samir Araujo
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

#ifndef ENTITYEXPERIENCEAGENT_H
#define ENTITYEXPERIENCEAGENT_H

#include <opencog/server/Agent.h>

namespace OperationalPetController
{

    /**
     * This agent mind computes the importance of the experience
     * acquired by the agent when it sees an entity inside the 
     * physical world. Each entity has a SemeNode. That node
     * identifies the entity by connecting it to a basic
     * hierarchy of entity types. Besides, each entity can
     * belong to a class of objects. So, the SemeNode inherits
     * from a concept of the objects hierarchy and refers to
     * a class, for instance:
     *
     * lets suppose that there is an entity ball_01. 
     * A ball is an Acessory and belongs to the class "ball".
     *
     * InheritanceLink
     *    SemeNode "ball_01"
     *    ConceptNode "Accessory"
     *
     * ReferenceLink
     *    ConceptNode "ball"
     *    SemeNode "ball_01"
     *
     *
     * Each time the agent sees a ball, the truth values
     * of the involved nodes will be something like:
     *
     * 
     * |Moment|       Node            |Strength|Count|
     * |1     |SemeNode "ball_01"     |1.0     |1    |  
     * |1     |ConceptNode "ball"     |1.0     |1    |  
     * |1     |ConceptNode "Accessory"|1.0     |1    |  
     * |2     |SemeNode "ball_01"     |1.0     |2    |  
     * |2     |ConceptNode "ball"     |1.0     |2    |  
     * |2     |ConceptNode "Accessory"|1.0     |2    |  
     *
     * now suppose the agent lost the ball_01, but 
     * sees another accessory: stick_01
     *
     * |Moment|       Node            |Strength|Count|
     * |3     |SemeNode "ball_01"     |0.666   |3    |  
     * |3     |ConceptNode "ball"     |0.666   |3    |  
     * |3     |SemeNode "stick_01"    |1.0     |1    |  
     * |3     |ConceptNode "stick"    |1.0     |1    |  
     * |3     |ConceptNode "Accessory"|1.0     |3    |       
     *
     */
    class EntityExperienceAgent : public opencog::Agent
    {
    private:
        count_t elapsedMoments;
 
    public:
        EntityExperienceAgent();
        virtual ~EntityExperienceAgent();

        virtual const ClassInfo& classinfo() const {
            return info();
        }

        static const ClassInfo& info() {
            static const ClassInfo _ci(
                "OperationalPetController::EntityExperienceAgent");
            return _ci;
        }
        
        void run(opencog::CogServer *server);        
        
    };

}; // OperationalPetController

#endif //  ENTITYEXPERIENCEAGENT_H
