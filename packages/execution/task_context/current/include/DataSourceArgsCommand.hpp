#ifndef ORO_DATASOURCE_ARGS_COMMAND_HPP
#define ORO_DATASOURCE_ARGS_COMMAND_HPP

#include "CommandFunctors.hpp"
#include "CommandProcessor.hpp"
#include "CommandDSFunctors.hpp"
#include "DispatchInterface.hpp"

namespace RTT
{
    namespace detail
    {
        /**
         * A Command which dispatches locally.
         * The arguments are fetched from DataSources.
         */
        template<class CommandT,
                 class CommandF = detail::Functor<CommandT> >
        class DataSourceArgsCommand
            : public DispatchInterface
        {
            CommandF mcom;
            mutable CommandF mcon;
            CommandProcessor* mcp;
            bool minvoked, maccept, mvalid, mexec, minvert;
        public:
            typedef boost::function_traits<CommandT> traits;
            typedef CommandT Signature;
            typedef bool result_type;

            DataSourceArgsCommand(boost::function<CommandT> com, boost::function<CommandT> con, CommandProcessor* cp, bool inverted)
                : mcom( com ), mcon( con ),
                  mcp( cp ),
                  minvoked(false), maccept(false),
                  mvalid(false), mexec(false), minvert(inverted)
            {
            }

            DataSourceArgsCommand(CommandF com, CommandF con, CommandProcessor* cp, bool inverted)
                : mcom( com ), mcon( con ),
                  mcp( cp ),
                  minvoked(false), maccept(false),
                  mvalid(false), mexec(false), minvert(inverted)
            {
            }

            DataSourceArgsCommand<CommandT,CommandF>* create() const
            {
                return clone();
            }

            template<class Arg1T>
            DataSourceArgsCommand<CommandT,CommandF>* create(DataSource<Arg1T>* a1) const
            {
                DataSourceArgsCommand<CommandT,CommandF>* r =  this->clone();
                r->mcom.setArguments(a1);
                r->mcon.setArguments(a1);
                return r;
            }

            template<class Arg1T, class Arg2T>
            DataSourceArgsCommand<CommandT,CommandF>* create(DataSource<Arg1T>* a1, DataSource<Arg2T>* a2) const
            {
                DataSourceArgsCommand<CommandT,CommandF>* r =  this->clone();
                r->mcom.setArguments(a1, a2);
                r->mcon.setArguments(a1, a2);
                return r;
            }

            template<class Arg1T, class Arg2T, class Arg3T>
            DataSourceArgsCommand<CommandT,CommandF>* create(DataSource<Arg1T>* a1, DataSource<Arg2T>* a2, DataSource<Arg3T>* a3) const
            {
                DataSourceArgsCommand<CommandT,CommandF>* r =  this->clone();
                r->mcom.setArguments(a1, a2, a3);
                r->mcon.setArguments(a1, a2, a3);
                return r;
            }

            template<class Arg1T, class Arg2T, class Arg3T, class Arg4T>
            DataSourceArgsCommand<CommandT,CommandF>* create(DataSource<Arg1T>* a1, DataSource<Arg2T>* a2, DataSource<Arg3T>* a3, DataSource<Arg4T>* a4) const
            {
                DataSourceArgsCommand<CommandT,CommandF>* r =  this->clone();
                r->mcom.setArguments(a1, a2, a3, a4);
                r->mcon.setArguments(a1, a2, a3, a4);
                return r;
            }

            bool ready() const {
                return !minvoked;
            }

            bool operator()() {
                if (minvoked)
                    return false;
                minvoked = true;
                return maccept = mcp->process( this );
            }

            virtual bool dispatch() {
                if (minvoked)
                    return false;
                minvoked = true;
                return maccept = mcp->process( this );
            }

            virtual void readArguments() { mcom.readArguments(); }

            virtual bool execute() {
                mvalid = mcom.execute();
                mexec = true;
                return mvalid;
            }
        
            virtual bool evaluate() const {
                if (mexec && mvalid )
                    return mcon.evaluate() != minvert;
                return false;
            }
     
            virtual void reset() {
                minvoked = (false);
                maccept = (false);
                mvalid = (false); 
                mexec = (false);
            }

            virtual bool sent() const {
                return minvoked;
            }

            virtual bool accepted() const {
                return maccept;
            }

            virtual bool executed() const {
                return mexec;
            }

            virtual bool valid() const {
                return mvalid;
            }

            virtual ConditionInterface* createCondition() const
            {
                return new detail::ConditionFunctor<CommandT,CommandF>(mcon, minvert);
            }

            virtual DataSourceArgsCommand<CommandT,CommandF>* clone() const {
                return new DataSourceArgsCommand<CommandT,CommandF>(*this);
            }

            virtual DataSourceArgsCommand<CommandT,CommandF>* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const {
                return new DataSourceArgsCommand<CommandT,CommandF>(CommandF(mcom.copy(alreadyCloned)),
                                                                    CommandF(mcon.copy(alreadyCloned)),mcp, minvert);
            }

        };
    }
}

#endif
