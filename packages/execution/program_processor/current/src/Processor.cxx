/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:36 CEST 2004  Processor.cxx

                        Processor.cxx -  description
                           -------------------
    begin                : Mon May 10 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be

 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/
#include "execution/Processor.hpp"

#include <boost/bind.hpp>

namespace ORO_Execution
{

    using boost::bind;
    using namespace std;

    Processor::Processor()
        :command(0)
    {
    }

    Processor::~Processor()
    {
    }

    bool program_lookup( const Processor::ProgramInfo& pi, const std::string& name)
    {
        return (pi.name == name);
    }

    bool state_lookup( const Processor::StateInfo& si, const std::string& name)
    {
        return (si.name == name);
    }

	bool Processor::loadProgram(ProgramInterface* pi)
    {
        program_iter it =
            find_if(programs.begin(), programs.end(), bind(program_lookup, _1, pi->getName() ) );
        if ( it != programs.end() )
            return false;
        programs.push_back( Processor::ProgramInfo(pi->getName(), pi) );
        pi->reset();
        return true;
    }

	bool Processor::resetProgram(const std::string& name)
    {
        program_iter it =
            find_if(programs.begin(), programs.end(), bind(program_lookup, _1, name) );
        if ( it != programs.end() && it->running == false && it->stepping == false)
            {
                it->program->reset();
                return true;
            }
        return false;
    }

	bool Processor::startProgram(const std::string& name)
    {
        program_iter it =
            find_if(programs.begin(), programs.end(), bind(program_lookup, _1, name) );
        if ( it != programs.end() )
            it->running = true;
        return it != programs.end();
    }

	bool Processor::isProgramRunning(const std::string& name) const
    {
        cprogram_iter it =
            find_if(programs.begin(), programs.end(), bind(program_lookup, _1, name) );
        if ( it != programs.end() )
            return it->running;
        return false;
    }

	bool Processor::startStepping(const std::string& name)
    {
        program_iter it =
            find_if(programs.begin(), programs.end(), bind(program_lookup, _1, name) );
        if ( it != programs.end() )
            it->stepping = true;
        return it != programs.end();
    }

	bool Processor::stopProgram(const std::string& name)
    {
        program_iter it =
            find_if(programs.begin(), programs.end(), bind(program_lookup, _1, name) );
        if ( it != programs.end() )
            it->running = false;
        return it != programs.end();
    }

	bool Processor::deleteProgram(const std::string& name)
    {
        program_iter it =
            find_if(programs.begin(), programs.end(), bind(program_lookup, _1, name) );
        if ( it != programs.end() && it->running == false && it->stepping == false)
            {
                delete it->program;
                programs.erase(it);
                return true;
            }
        return false;
    }


	bool Processor::loadStateContext(const std::string& name, StateContext* sc)
    {
        state_iter it =
            find_if(states.begin(), states.end(), bind(state_lookup, _1, name) );
        if ( it != states.end() )
            return false;
        states.push_back(Processor::StateInfo(name, sc));
        return true;
    }

	bool Processor::startStateContext(const std::string& name)
    {
        state_iter it =
            find_if(states.begin(), states.end(), bind(state_lookup, _1, name) );
        if ( it != states.end() )
          it->running = true;
        return it != states.end();
    }

	bool Processor::steppedStateContext(const std::string& name)
    {
        state_iter it =
            find_if(states.begin(), states.end(), bind(state_lookup, _1, name) );
        if ( it != states.end() )
            {
                it->stepping = true;
            }
        return it != states.end();
    }

	bool Processor::continuousStateContext(const std::string& name)
    {
        state_iter it =
            find_if(states.begin(), states.end(), bind(state_lookup, _1, name) );
        if ( it != states.end() )
            {
                it->stepping = false;
            }
        return it != states.end();
    }

	bool Processor::isStateContextRunning(const std::string& name) const
    {
        cstate_iter it =
            find_if(states.begin(), states.end(), bind(state_lookup, _1, name) );
        if ( it != states.end() )
            return it->running;
        return false;
    }

	bool Processor::isStateContextStepped(const std::string& name) const
    {
        cstate_iter it =
            find_if(states.begin(), states.end(), bind(state_lookup, _1, name) );
        if ( it != states.end() )
            return it->stepping;
        return false;
    }

	bool Processor::stopStateContext(const std::string& name)
    {
        state_iter it =
            find_if(states.begin(), states.end(), bind(state_lookup, _1, name) );
        if ( it != states.end() )
            {
                it->running = false;
                it->final   = true;
            }
        return it != states.end();
    }

	bool Processor::resetStateContext(const std::string& name)
    {
        // We can only reset when stopped.
        state_iter it =
            find_if(states.begin(), states.end(), bind(state_lookup, _1, name) );
        if ( it != states.end() && it->running == false )
            {
                it->init   = true;
            }
        return it != states.end();
    }

    bool Processor::unloadStateContext( const std::string& name )
    {
        // this does the same as deleteStateContext, except for deleting
        // the unloaded context..
        state_iter it =
            find_if(states.begin(), states.end(), bind(state_lookup, _1, name) );
        if ( it != states.end() && it->running == false )
        {
            states.erase(it);
            return true;
        }
        return false;
    }

	bool Processor::deleteStateContext(const std::string& name)
    {
        // this is not yet 100% thread safe. we should wait for the state
        // to agree with deletion and then delete it, instead of just checking
        // this flag.
        state_iter it =
            find_if(states.begin(), states.end(), bind(state_lookup, _1, name) );
        if ( it != states.end() && it->running == false )
            {
                states.erase(it);
                delete it->state;
                return true;
            }
        return false;
    }

    void executeState( Processor::StateInfo& s)
    {
        // This peace of code is written in such a way that if you
        // request stop/reset/start very fast, it will still behave
        // as expected. So do not change the order of checking flags.
        if (s.final)
            {
                // this always succeeds
                s.state->requestFinalState();
                s.final = false;
                s.state->stopRunning();
            }
        else if (s.init)
            {
                s.state->startRunning();
                // on failure, request next state anyway.
                if ( !s.state->requestInitialState() )
                    s.state->requestNextState();
                s.init = false;
            }
        else if (s.running)
            {
                if (s.stepping)
                    s.state->requestNextState(); // one state at a time
                else {
                    ORO_CoreLib::StateInterface* cur_s = s.state->currentState();
                    while ( cur_s != s.state->requestNextState() ) // go until no transition found.
                        cur_s = s.state->currentState();
                }
            }
    }

    void executeProgram( Processor::ProgramInfo& p)
    {
        if (p.running)
            p.program->execute();
    }

    void stepProgram( Processor::ProgramInfo& p)
    {
        if (p.stepping)
            {
                p.program->execute();
                p.stepping=false;
            }
    }

	void Processor::doStep()
    {
        // Evaluate all states.
        for_each(states.begin(), states.end(), executeState);

        // Execute any additional (deferred/external) command.
        if ( command )
            {
                command->execute();
                command = 0;
                // should we allow the system context to check validity ??
                // external commands can gravely interfere with the
                // system...
                // for_each(states.begin(), states.end(), executeState);
            }

        //Execute all normal programs.
        for_each(programs.begin(), programs.end(), executeProgram);

		//Execute all programs in Stepping mode.
        for_each(programs.begin(), programs.end(), stepProgram );
    }


    bool Processor::nextStep(const std::string& name)
    {
        program_iter it =
            find_if(programs.begin(), programs.end(), bind(program_lookup, _1, name) );
        if ( it != programs.end() )
            it->stepping = true;
        return it != programs.end();
    }


    bool Processor::process( CommandInterface* c)
    {
        if (command != 0)
            return false;
        command = c;
        return true;
    }

}

