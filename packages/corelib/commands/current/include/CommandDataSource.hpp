#ifndef ORO_COMMAND_DATASOURCE_HPP
#define ORO_COMMAND_DATASOURCE_HPP

namespace ORO_CoreLib
{
    /**
     * A Command which evaluates a DataSourceBase and always returns true.
     */
    struct CommandDataSource :
        public CommandInterface
    {
        DataSourceBase::shared_ptr _dsb;
        CommandDataSource( DataSourceBase::shared_ptr dsb )
            : _dsb(dsb) {}
        bool execute() {
            _dsb->evaluate();
            return true;
        }
        void reset() {
            _dsb->reset();
        }
        CommandInterface* clone() const {
            return new CommandDataSource( _dsb );
        }

        CommandInterface* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const {
            return new CommandDataSource( _dsb->copy( alreadyCloned ) );
        }
            
    };

    /**
     * A Command which evaluates a DataSource<bool> and returns the result of get().
     */
    struct CommandDataSourceBool :
        public CommandInterface
    {
        DataSource<bool>::shared_ptr _dsb;
        CommandDataSourceBool( DataSource<bool>::shared_ptr dsb )
            : _dsb(dsb) {}
        bool execute() {
            return _dsb->get();
        }
        void reset() {
            _dsb->reset();
        }
        CommandInterface* clone() const {
            return new CommandDataSourceBool( _dsb );
        }

        CommandInterface* copy( std::map<const DataSourceBase*, DataSourceBase*>& alreadyCloned ) const {
            return new CommandDataSourceBool( _dsb->copy( alreadyCloned ) );
        }
            
    };

}

#endif
