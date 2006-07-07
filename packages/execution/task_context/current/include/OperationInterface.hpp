#ifndef ORO_OPERATION_INTERFACE_HPP
#define ORO_OPERATION_INTERFACE_HPP

#include "CommandFunctors.hpp"
#include "CommandProcessor.hpp"
#include "CommandDSFunctors.hpp"
#include "DispatchInterface.hpp"
#include "OperationFactory.hpp"
#include "TemplateMemberFactory.hpp"
#include "CommandC.hpp"
#include "MethodC.hpp"

namespace ORO_Execution
{

    /**
     * A Command which dispatches locally.
     * The arguments are fetched from DataSources.
     */
    template<class CommandT,
             class ComF=detail::CommandFunctor<CommandT>,
             class ConF=detail::ConditionFunctor<CommandT> >
    class CommandFromDS
        : public DispatchInterface
    {
        ComF mcom;
        mutable ConF mcon;
        CommandProcessor* mcp;
        bool minvoked, maccept, mvalid, mexec;
    public:
        typedef boost::function_traits<CommandT> traits;
        typedef CommandT Signature;
        typedef bool result_type;

        CommandFromDS(boost::function<CommandT> com, boost::function<CommandT> con, CommandProcessor* cp, bool inverted)
            : mcom( com ), mcon( con, inverted ),
              mcp( cp ),
              minvoked(false), maccept(false),
              mvalid(false), mexec(false)
        {
        }

        CommandFromDS(ComF com, ConF con, CommandProcessor* cp)
            : mcom( com ), mcon( con ),
              mcp( cp ),
              minvoked(false), maccept(false),
              mvalid(false), mexec(false)
        {
        }

        CommandFromDS<CommandT,ComF,ConF>* create() const
        {
            return clone();
        }

        template<class Arg1T>
        CommandFromDS<CommandT,ComF,ConF>* create(DataSource<Arg1T>* a1) const
        {
            CommandFromDS<CommandT,ComF,ConF>* r =  this->clone();
            r->mcom.com.setArguments(a1);
            r->mcon.con.setArguments(a1);
            return r;
        }

        template<class Arg1T, class Arg2T>
        CommandFromDS<CommandT,ComF,ConF>* create(DataSource<Arg1T>* a1, DataSource<Arg2T>* a2) const
        {
            CommandFromDS<CommandT,ComF,ConF>* r =  this->clone();
            r->mcom.com.setArguments(a1, a2);
            r->mcon.con.setArguments(a1, a2);
            return r;
        }

        template<class Arg1T, class Arg2T, class Arg3T>
        CommandFromDS<CommandT,ComF,ConF>* create(DataSource<Arg1T>* a1, DataSource<Arg2T>* a2, DataSource<Arg3T>* a3) const
        {
            CommandFromDS<CommandT,ComF,ConF>* r =  this->clone();
            r->mcom.com.setArguments(a1, a2, a3);
            r->mcon.con.setArguments(a1, a2, a3);
            return r;
        }

        template<class Arg1T, class Arg2T, class Arg3T, class Arg4T>
        CommandFromDS<CommandT,ComF,ConF>* create(DataSource<Arg1T>* a1, DataSource<Arg2T>* a2, DataSource<Arg3T>* a3, DataSource<Arg4T>* a4) const
        {
            CommandFromDS<CommandT,ComF,ConF>* r =  this->clone();
            r->mcom.com.setArguments(a1, a2, a3, a4);
            r->mcon.con.setArguments(a1, a2, a3, a4);
            return r;
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

        virtual void readArguments() {}

        virtual bool execute() {
            mexec = true;
            return mvalid = mcom.execute();
        }
        
        virtual bool evaluate() const {
            if (mexec && mvalid )
                return mcon.evaluate();
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

        virtual ORO_CoreLib::ConditionInterface* createCondition() const
        {
            return mcon.clone();
        }

        virtual CommandFromDS<CommandT,ComF,ConF>* clone() const {
            return new CommandFromDS(*this);
        }

    };

    /**
     * A command repository stores a number of commands from a Task which
     * can be used by other tasks or from scripts.
     */
    class CommandRepository
        : public OperationFactory<DispatchInterface*>
    {
        template<class T>
        inline T* getpointer(T t) {
            return &t;
        }
        template<class T>
        inline T* getpointer(T* t) {
            return t;
        }
    public:
        /** 
         * Retrieve a previously added Command.
         * 
         * @param name The name of the Command
         * @param args A vector of command arguments.
         * 
         * @return A dispatchable object which is a new Command object.
         */
        DispatchInterface* getCommand( std::string name,
                                          const std::vector<ORO_CoreLib::DataSourceBase::shared_ptr>& args) const
        {
            return this->produce(name, args);
        }

        /** 
         * Retrieve the completion condition of a previously added Command.
         * 
         * @param name The name of the Command
         * @param args A vector of command arguments
         * 
         * @return A condition which evaluates the command's completion.
         */
        ConditionInterface* getCondition( std::string name,
                                         const std::vector<ORO_CoreLib::DataSourceBase::shared_ptr>& args) const
        {
            DispatchInterface* di = this->produce(name, args);
            ConditionInterface* ret = di->createCondition();
            delete di;
            return ret;
        }

        /** 
         * Create a CommandC container object, which can be used
         * to access an added Command.
         * 
         * @param name The name of the Command
         * 
         * @return A new CommandC object.
         */
        CommandC create(std::string name) {
            return CommandC( this, name );
        }

        /** 
         * Add an existing Command, which takes no arguments, to the Operations interface.
         * 
         * @param c A pointer to the existing command.
         * @param description A user readable description of what this command does.
         * 
         * @return true if the command could be added.
         */
        template<class CommandT>
        bool addCommand( CommandT com, const char* description) 
        {
            typedef typename boost::remove_pointer<CommandT>::type CommandVT;
            typedef typename boost::add_pointer<CommandVT>::type CommandPT;
            typedef typename CommandVT::Signature ComSig;
            CommandPT c = this->getpointer(com);
            if ( this->hasMember(c->getName() ) )
                return false;
            this->add( c->getName(), new detail::OperationFactoryPart0<DispatchInterface*, CommandFromDS<ComSig> >( 
                  CommandFromDS<ComSig>(c->getCommandFunction(),
                                        c->getConditionFunction(), 
                                        c->getCommandProcessor(), c->isInverted() ), description) );
            return true;
        }

        /** 
         * Add an existing Command, which takes one argument, to the
         * Operations Interface.
         * 
         * @param c A pointer to the existing command.
         * @param description A user readable description of what this command does.
         * @param arg1 A single word name of the argument.
         * @param arg1_description A user readable description of the argument.
         * 
         * @return true if the command could be added.
         */
        template<class CommandT>
        bool addCommand( CommandT com, const char* description,
                         const char* arg1, const char* arg1_description)
        {
            typedef typename boost::remove_pointer<CommandT>::type CommandVT;
            typedef typename boost::add_pointer<CommandVT>::type CommandPT;
            typedef typename CommandVT::Signature ComSig;
            CommandPT c = this->getpointer(com);
            if ( this->hasMember(c->getName() ) )
                return false;
            this->add( c->getName(), new detail::OperationFactoryPart1<DispatchInterface*, CommandFromDS<ComSig> >( 
                  CommandFromDS<ComSig>(c->getCommandFunction(),
                                        c->getConditionFunction(), 
                                        c->getCommandProcessor(), c->isInverted() ), 
                  description, arg1, arg1_description) );
            return true;
        }

        /** 
         * Add an existing Command, which takes two arguments, to the
         * Operations Interface.
         * 
         * @param c A pointer to the existing command.
         * @param description A user readable description of what this command does.
         * @param arg1 A single word name of the first argument.
         * @param arg1_description A user readable description of the first argument.
         * @param arg2 A single word name of the second argument.
         * @param arg2_description A user readable description of the second argument.
         * 
         * @return true if the command could be added.
         */
        template<class CommandT>
        bool addCommand( CommandT com, const char* description,
                         const char* arg1, const char* arg1_description,
                         const char* arg2, const char* arg2_description)
        {
            typedef typename boost::remove_pointer<CommandT>::type CommandVT;
            typedef typename boost::add_pointer<CommandVT>::type CommandPT;
            typedef typename CommandVT::Signature ComSig;
            CommandPT c = this->getpointer(com);
            if ( this->hasMember(c->getName() ) )
                return false;
            this->add( c->getName(), new detail::OperationFactoryPart2<DispatchInterface*, CommandFromDS<ComSig> >( 
                  CommandFromDS<ComSig>(c->getCommandFunction(),
                                        c->getConditionFunction(), 
                                        c->getCommandProcessor(), c->isInverted() ), 
                  description, arg1, arg1_description,
                  arg2, arg2_description) );
            return true;
        }


        template<class CommandT>
        bool addCommand( CommandT com, const char* description,
                         const char* arg1, const char* arg1_description,
                         const char* arg2, const char* arg2_description,
                         const char* arg3, const char* arg3_description)
        {
            typedef typename boost::remove_pointer<CommandT>::type CommandVT;
            typedef typename boost::add_pointer<CommandVT>::type CommandPT;
            typedef typename CommandVT::Signature ComSig;
            CommandPT c = this->getpointer(com);
            if ( this->hasMember(c->getName() ) )
                return false;
            this->add( c->getName(), new detail::OperationFactoryPart3<DispatchInterface*, CommandFromDS<ComSig> >( 
                  CommandFromDS<ComSig>(c->getCommandFunction(),
                                               c->getConditionFunction(), 
                                               c->getCommandProcessor(), c->isInverted() ), 
                  description, arg1, arg1_description,
                  arg2, arg2_description,
                  arg3, arg3_description) );
            return true;
        }

        template<class CommandT>
        bool addCommand( CommandT com, const char* description,
                         const char* arg1, const char* arg1_description,
                         const char* arg2, const char* arg2_description,
                         const char* arg3, const char* arg3_description,
                         const char* arg4, const char* arg4_description)
        {
            typedef typename boost::remove_pointer<CommandT>::type CommandVT;
            typedef typename boost::add_pointer<CommandVT>::type CommandPT;
            typedef typename CommandVT::Signature ComSig;
            CommandPT c = this->getpointer(com);
            if ( this->hasMember(c->getName() ) )
                return false;
            this->add( c->getName(), new detail::OperationFactoryPart4<DispatchInterface*, CommandFromDS<ComSig> >( 
                  CommandFromDS<ComSig>(c->getCommandFunction(),
                                               c->getConditionFunction(), 
                                               c->getCommandProcessor(), c->isInverted() ), 
                  description, arg1, arg1_description,
                  arg2, arg2_description,
                  arg3, arg3_description,
                  arg4, arg4_description) );
            return true;
        }

        template<class CommandT,class CompT>
        bool addCommandDS( DataSource< boost::weak_ptr<CompT> >* wp, CommandT* c, const char* description) 
        {
            using namespace detail;
            typedef typename CommandT::Signature ComSig;
            if ( this->hasMember(c->getName() ) )
                return false;
            typedef CommandFunctor<ComSig,FunctorDS0<boost::function<ComSig> > > ComF;
            typedef ConditionFunctor<ComSig,FunctorDS0<boost::function<ComSig> > > ConF;
            typedef CommandFromDS<ComSig,
                                  ComF, ConF> DSComm;
            this->add( c->getName(), new detail::OperationFactoryPart0<DispatchInterface*, DSComm>( 
                        DSComm( ComF( FunctorDS0<boost::function<ComSig> >(wp, c->getCommandFunction())),
                                ConF( FunctorDS0<boost::function<ComSig> >(wp, c->getConditionFunction()), c->isInverted()), 
                                c->getCommandProcessor() ),
                        description) );
            return true;
        }

        template<class CommandT, class CompT>
        bool addCommandDS( DataSource<boost::weak_ptr<CompT> >* wp, CommandT* c, const char* description,
                         const char* arg1, const char* arg1_description)
        {
            using namespace detail;
            typedef typename CommandT::Signature ComSig;
            typedef CommandFunctor<ComSig,FunctorDS1<boost::function<ComSig> > > ComF;
            typedef ConditionFunctor<ComSig,FunctorDS1<boost::function<ComSig> > > ConF;
            typedef CommandFromDS<ComSig,
                                  ComF, ConF> DSComm;
            if ( this->hasMember(c->getName() ) )
                return false;
            this->add( c->getName(), new detail::OperationFactoryPart1<DispatchInterface*, DSComm, typename DSComm::traits::arg2_type>( 
                        DSComm( ComF( FunctorDS1<boost::function<ComSig> >(wp, c->getCommandFunction())),
                                ConF( FunctorDS1<boost::function<ComSig> >(wp, c->getConditionFunction()), c->isInverted()), 
                                c->getCommandProcessor() ),
                        description, arg1, arg1_description) );
            return true;
        }


    };

    /**
     * A method which gets its arguments from a data source and
     * is a datasource itself.
     */
    template<class SignatureT, class FunctorT = detail::FunctorDataSource<boost::function<SignatureT> > >
    class MethodFromDS
        : public DataSource< typename boost::function_traits<SignatureT>::result_type >
    {
        typename FunctorT::shared_ptr mmeth;
    public:
        typedef boost::function_traits<SignatureT> traits;
        typedef SignatureT Signature;

        typedef typename boost::function_traits<Signature>::result_type result_type;
        typedef DataSource<result_type> Base;

        MethodFromDS(boost::function<Signature> meth)
            : mmeth( new FunctorT(meth) )
        {
        }

        MethodFromDS( typename FunctorT::shared_ptr ds)
            : mmeth( ds )
        {
        }

        MethodFromDS<Signature,FunctorT>* create() const
        {
            return clone();
        }

        template<class Arg1T>
        MethodFromDS<Signature,FunctorT>* create(DataSource<Arg1T>* a1) const
        {
            MethodFromDS<Signature,FunctorT>* r =  this->clone();
            r->mmeth->setArguments(a1);
            return r;
        }

        template<class Arg1T, class Arg2T>
        MethodFromDS<Signature,FunctorT>* create(DataSource<Arg1T>* a1, DataSource<Arg2T>* a2) const
        {
            MethodFromDS<Signature,FunctorT>* r =  this->clone();
            r->mmeth->setArguments(a1, a2);
            return r;
        }

        template<class Arg1T, class Arg2T, class Arg3T>
        MethodFromDS<Signature,FunctorT>* create(DataSource<Arg1T>* a1, DataSource<Arg2T>* a2, DataSource<Arg3T>* a3) const
        {
            MethodFromDS<Signature,FunctorT>* r =  this->clone();
            r->mmeth->setArguments(a1, a2, a3);
            return r;
        }

        template<class Arg1T, class Arg2T, class Arg3T, class Arg4T>
        MethodFromDS<Signature,FunctorT>* create(DataSource<Arg1T>* a1, DataSource<Arg2T>* a2, DataSource<Arg3T>* a3, DataSource<Arg4T>* a4) const
        {
            MethodFromDS<Signature,FunctorT>* r =  this->clone();
            r->mmeth->setArguments(a1, a2, a3, a4);
            return r;
        }

        result_type operator()() {
            return mmeth->get();
        }

        virtual result_type get() const {
            return mmeth->get();
        }

        virtual result_type value() const {
            return mmeth->value();
        }

        virtual MethodFromDS<Signature,FunctorT>* clone() const {
            return new MethodFromDS( typename FunctorT::shared_ptr(mmeth->clone()) );
        }

        virtual DataSource<result_type>* copy( std::map<const ORO_CoreLib::DataSourceBase*, ORO_CoreLib::DataSourceBase*>& alreadyCloned ) const
        {
            return new MethodFromDS<Signature,FunctorT>( typename FunctorT::shared_ptr(mmeth->copy(alreadyCloned)) );
        }

        boost::function<Signature> getMethodFunction() const {
            return mmeth.ff.gen;
        }
    };


    /**
     * This class allows storage and retrieval of Method objects.
     * 
     */
    class MethodRepository
        : public OperationFactory<DataSourceBase*>
    {
    public:
        DataSourceBase* getMethod( std::string name,
                                   const std::vector<ORO_CoreLib::DataSourceBase::shared_ptr>& args) const
        {
            return this->produce(name, args);
        }

        MethodC create(std::string name) {
            return MethodC( this, name );
        }

        template<class MethodT>
        bool addMethod( MethodT* c, const char* description) 
        {
            typedef typename MethodT::Signature Sig;
            if ( this->hasMember(c->getName() ) )
                return false;
            this->add( c->getName(), new detail::OperationFactoryPart0<DataSourceBase*, MethodFromDS<Sig> >( 
                  MethodFromDS<Sig>(c->getMethodFunction()), description) );
            return true;
        }

        template<class MethodT>
        bool addMethod( MethodT* c, const char* description,
                         const char* arg1, const char* arg1_description)
        {
            typedef typename MethodT::Signature Sig;
            if ( this->hasMember(c->getName() ) )
                return false;
            this->add( c->getName(), new detail::OperationFactoryPart1<DataSourceBase*, MethodFromDS<Sig> >( 
                  MethodFromDS<Sig>(c->getMethodFunction()), 
                  description, arg1, arg1_description) );
            return true;
        }

        template<class MethodT>
        bool addMethod( MethodT* c, const char* description,
                        const char* arg1, const char* arg1_description,
                        const char* arg2, const char* arg2_description)
        {
            typedef typename MethodT::Signature Sig;
            if ( this->hasMember(c->getName() ) )
                return false;
            this->add( c->getName(), new detail::OperationFactoryPart2<DataSourceBase*, MethodFromDS<Sig> >( 
                  MethodFromDS<Sig>(c->getMethodFunction()), 
                  description, 
                  arg1, arg1_description,
                  arg2, arg2_description) );
            return true;
        }
        template<class MethodT>
        bool addMethod( MethodT* c, const char* description,
                        const char* arg1, const char* arg1_description,
                        const char* arg2, const char* arg2_description,
                        const char* arg3, const char* arg3_description)
        {
            typedef typename MethodT::Signature Sig;
            if ( this->hasMember(c->getName() ) )
                return false;
            this->add( c->getName(), new detail::OperationFactoryPart3<DataSourceBase*, MethodFromDS<Sig> >( 
                  MethodFromDS<Sig>(c->getMethodFunction()), 
                  description, 
                  arg1, arg1_description,
                  arg2, arg2_description,
                  arg3, arg3_description) );
            return true;
        }

        template<class MethodT>
        bool addMethod( MethodT* c, const char* description,
                        const char* arg1, const char* arg1_description,
                        const char* arg2, const char* arg2_description,
                        const char* arg3, const char* arg3_description,
                        const char* arg4, const char* arg4_description)
        {
            typedef typename MethodT::Signature Sig;
            if ( this->hasMember(c->getName() ) )
                return false;
            this->add( c->getName(), new detail::OperationFactoryPart4<DataSourceBase*, MethodFromDS<Sig> >( 
                  MethodFromDS<Sig>(c->getMethodFunction()), 
                  description, 
                  arg1, arg1_description,
                  arg2, arg2_description,
                  arg3, arg3_description,
                  arg4, arg4_description) );
            return true;
        }

        template<class MethodT,class CompT>
        bool addMethodDS( DataSource< boost::weak_ptr<CompT> >* wp, MethodT* c, const char* description) 
        {
            using namespace detail;
            typedef typename MethodT::Signature Sig;
            if ( this->hasMember(c->getName() ) )
                return false;
            typedef FunctorDataSourceDS0<CompT, boost::function<Sig> > FunctorT;
            typedef MethodFromDS<Sig, FunctorT> DSMeth;
            
            this->add( c->getName(), new detail::OperationFactoryPart0<DataSourceBase*, DSMeth>( 
                        DSMeth( typename FunctorT::shared_ptr(new FunctorT(wp, c->getMethodFunction()))),
                        description));
            return true;
        }

        template<class MethodT,class CompT>
        bool addMethodDS( DataSource< boost::weak_ptr<CompT> >* wp, MethodT* c, const char* description, 
                          const char* a1, const char* d1) 
        {
            using namespace detail;
            typedef typename MethodT::Signature Sig;
            if ( this->hasMember(c->getName() ) )
                return false;
            typedef typename MethodT::traits::arg2_type arg1_type; // second arg is 1st data arg.
            typedef FunctorDataSourceDS1<CompT, boost::function<Sig>, arg1_type > FunctorT;
            typedef MethodFromDS<Sig, FunctorT> DSMeth;
            this->add( c->getName(), new detail::OperationFactoryPart1<DataSourceBase*, DSMeth, arg1_type >( 
                        DSMeth( typename FunctorT::shared_ptr(new FunctorT(wp, c->getMethodFunction()))),
                        description, a1, d1));
            return true;
        }

    };

    /**
     * The interface for accessing and executing 'operations', being commands
     * and methods.
     */
    class OperationInterface
    {
    public:
        virtual ~OperationInterface() {}
        virtual CommandRepository* commands() = 0;

        virtual MethodRepository* methods() = 0;
    };


    /**
     * A task object groups a set of commands and methods (operations)
     * which may be invoked.
     */
    class TaskObject
        : public OperationInterface
    {
        const std::string mname;

        CommandRepository mcommands;

        MethodRepository mmethods;
    public:
        TaskObject(std::string name)
            : mname(name)
        {}

        const std::string& getName() { return mname; }

        virtual CommandRepository* commands() { return &mcommands; }

        virtual MethodRepository* methods() { return &mmethods;}
    };
}


#endif
