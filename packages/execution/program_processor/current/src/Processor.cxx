#include "execution/Processor.hpp"

#include <boost/bind.hpp>

namespace ORO_Execution
{

    using boost::bind;
    using namespace std;

    Processor::Processor()
        :command(0)
    {
//         currentState = new ProcessorStateInit(this);
    }

    Processor::~Processor()
    {
//         delete currentState;
    }

//     void Processor::changeState(ProcessorState* newState)
//     {
//         delete currentState;
//     	currentState = newState;
//     }

//     void Processor::resetState()
//     {
//         delete currentState;
//     	currentState = new ProcessorStateInit( this );
//     }

	//forward every action to its state
//  bool Processor::startConfiguration(){ return currentState->startConfiguration();}
// 	bool Processor::abort(){ return currentState->abort();}
// 	bool Processor::endConfiguration(){return currentState->endConfiguration();}

	bool Processor::loadProgram(const std::string& name, ProgramInterface* pi)
    {
        if ( states.empty() )
            return false;
        programs.push_back( Processor::ProgramInfo(name, pi) );
        return true;
    }

    bool program_lookup( const Processor::ProgramInfo& pi, const std::string& name)
    {
        return (pi.name == name);
    }

	bool Processor::resetProgram(const std::string& name)
    {
        program_iter it =
            find_if(programs.begin(), programs.end(), bind(program_lookup, _1, name) );
        if ( it != programs.end() && it->running == false )
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
        if ( it != programs.end() && it->running == false )
            {
                delete it->program;
                programs.erase(it);
                return true;
            }
        return false;
    }


    bool state_lookup( const Processor::StateInfo& si, const std::string& name)
    {
        return (si.name == name);
    }

	bool Processor::loadStateContext(const std::string& name, StateContext* sc)
    {
        states.push_back(Processor::StateInfo(name, sc));
        return true;
    }

	bool Processor::startStateContext(const std::string& name)
    {
        state_iter it =
            find_if(states.begin(), states.end(), bind(state_lookup, _1, name) );
        if ( it != states.end() )
            {
                it->running = true;
            }
        return it != states.end();
    }


	bool Processor::stopStateContext(const std::string& name)
    {
        state_iter it =
            find_if(states.begin(), states.end(), bind(state_lookup, _1, name) );
        if ( it != states.end() )
            {
                it->final   = true;
                it->running = false;
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
            }
        else if (s.init)
            {
                // on failure, request next state anyway.
                if ( !s.state->requestInitialState() )
                    s.state->requestNextState();
                s.init = false;
            }
        else if (s.running)
            s.state->requestNextState();
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
        if (states.empty() || programs.empty())
            return;

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

