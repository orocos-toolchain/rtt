#ifndef ORO_COMMANDBINARY_HPP
#define ORO_COMMANDBINARY_HPP

#include <corelib/CommandInterface.hpp>

namespace ORO_Execution
{
    using ORO_CoreLib::CommandInterface;

    struct CommandBinary : public CommandInterface
    {
        CommandInterface* _f;
        CommandInterface* _s;
        CommandBinary( CommandInterface* f, CommandInterface* s)
            : _f(f), _s(s) {}
        virtual ~CommandBinary() {
            delete _f;
            delete _s;
        }
        virtual bool execute() {
            return _f->execute() && _s->execute();
        }
        virtual void reset() {
            _f->reset();
            _s->reset();
        }
        virtual CommandInterface* clone() const {
            return new CommandBinary( _f->clone(), _s->clone() );
        }
        virtual CommandInterface* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const {
            return new CommandBinary( _f->copy( alreadyCloned ), _s->copy( alreadyCloned ) );
        }
    };

}

#endif
