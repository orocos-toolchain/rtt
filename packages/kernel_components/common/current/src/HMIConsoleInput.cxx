
#pragma implementation
#include "kernel_components/HMIConsoleInput.hpp"

namespace ORO_ControlKernel
{
    //    template class HMIConsoleInput< SupportComponent< MakeExtension<KernelBaseFunction, ExecutionExtension>::Result::CommonBase > >;
    template class HMIConsoleInput<>;



#if 0
    HMIConsoleInput<SupportComponent< MakeExtension<KernelBaseFunction, ExecutionExtension>::Result::CommonBase > >::HMIConsoleInput( ExecutionExtension* _ee )
        : HMIConsoleInput<SupportComponent< MakeExtension<KernelBaseFunction, ExecutionExtension>::Result::CommonBase > >::Base("console_input"), start(false),
          startEvent(Event::SYNASYN,"HMIConsoleInput::StartEvent"),
          ee(_ee), condition(0), command(0), tester(0), dataobject(0)
    {}

    void HMIConsoleInput<SupportComponent< MakeExtension<KernelBaseFunction, ExecutionExtension>::Result::CommonBase > >::loop()
    {
        cout << endl<<
            "  This simple console reader can only accept start and stop \n\
  commands to start or stop the running program and  allows \n\
  to manually type in a Component command (type 'help' for info). \n\n\
  If the ReportingExtension is used, you can track the changes \n\
  in another terminal window."<<endl<<endl;
        while (1)
            {
                cout << "Type 'start' or 'stop' to start/stop the current program-script " << endl;
                if ( ee !=0 ) {
                    cout << " Or enter a command. (Status of previous command : ";
                    cout << (condition == 0 ? "none )" : condition->evaluate() == true ? "done )" : "busy )" );
                }
                cout << endl <<" (type 'quit' for exit) :" ;
                std::string command;
                getline(cin, command);
                if ( command == "quit" ) {
                    stopButton();
                    return;
                } else if ( command == "start") {
                    startButton();
                } else if ( command == "stop") {
                    stopButton();
                } else if ( command == "help") {
                    printHelp();
                } else if ( command == "" ) { // nop
                } else
                    evalCommand( command );
                cout <<endl;
            }
    }

    void HMIConsoleInput<SupportComponent< MakeExtension<KernelBaseFunction, ExecutionExtension>::Result::CommonBase > >::evalCommand(std::string& comm )
    {
        if ( ee == 0 )
            return;
        //             if ( condition != 0 && condition->evaluate() == false )
        //                 {
        //                     cout << "Previous command not done yet !"<<endl;
        //                     return;
        //                 }
        if ( ee->isProgramRunning("Default") )
            {
                cout << "A Program is running, not accepting commands  !"<<endl;
                return;
            }
        cout << "      Got :"<< comm <<endl;

        tester = ee->commandFactory().getObjectFactory( comm );
        dataobject = ee->dataFactory().factory( comm );
        if ( tester ) // only commandobject name was typed
            {
                std::vector<std::string> methods = tester->getMethodList();
                std::for_each( methods.begin(), methods.end(), boost::bind(&HMIConsoleInput<SupportComponent< MakeExtension<KernelBaseFunction, ExecutionExtension>::Result::CommonBase > >::printMethod, this, _1) );
            }
                    
        if ( dataobject ) // only dataobject name was typed
            {
                std::vector<std::string> methods = dataobject->dataNames();
                std::for_each( methods.begin(), methods.end(), boost::bind(&HMIConsoleInput<SupportComponent< MakeExtension<KernelBaseFunction, ExecutionExtension>::Result::CommonBase > >::printSource, this, _1) );
            }
        if ( tester || dataobject )
            return;
                    
        delete command;
        delete condition;
        ORO_Execution::Parser _parser;
        std::pair< CommandInterface*, ConditionInterface*> comcon;
        comcon = _parser.parseCommand(comm, ee);
        command = comcon.first;
        condition = comcon.second;
        if ( command == 0 ) {
            cout << "Parse Error : Illegal command."<<endl;
            return;
        }
        if ( ee->getProcessor()->process( command ) == false ) {
            cout << "Command not accepted by Processor ! " << endl;
            delete command;
            delete condition;
            command = 0;
            condition = 0;
            return;
        }
    }

    void HMIConsoleInput<SupportComponent< MakeExtension<KernelBaseFunction, ExecutionExtension>::Result::CommonBase > >::printHelp()
    {
        using boost::lambda::_1;
        if (ee == 0){
            cout << "  The HMIConsoleInput object hasn't got a reference to the Execution"<<endl;
            cout <<"   Engine on construction. No Commands are available."<<endl;
            return;
        }
        cout << endl;
        cout << "  A command consists of an object, followed by a dot ('.'), the method "<<endl;
        cout << "  name, followed by the parameters. An example could be :"<<endl;
        cout << "  CartesianGenerator.moveTo( frame( vector( .75, .5, .8), rotation( 90., 0., 90. ) ), 15.0 ) [then press enter] "<<endl;
        cout << "  console_output.displayDouble( CartesianGenerator.currentTime ) [enter]" <<endl;
        cout << endl<<"  The available objects are :"<<endl;
        std::vector<std::string> objlist = ee->commandFactory().getObjectList();
        std::for_each( objlist.begin(), objlist.end(), cout << _1 << "\n" );
        cout << "  The available DataSources are :"<<endl;
        objlist = ee->dataFactory().getObjectList();
        std::for_each( objlist.begin(), objlist.end(), cout << _1 << "\n" );
        cout << "  For the Argument list of an object, just type the object name (eg 'kernel')" <<endl;
        cout <<endl;
            
    }
        
    void HMIConsoleInput<SupportComponent< MakeExtension<KernelBaseFunction, ExecutionExtension>::Result::CommonBase > >::printMethod( const std::string m )
    {
        using boost::lambda::_1;
        std::vector<ArgumentDescription> args;
        args = tester->getArgumentList( m );
        cout << "  Method   : " << m <<endl;
        //int i = 0;
        if (args.begin() != args.end() ){
            cout << "  Argument :"<<endl;
            std::for_each( args.begin(), args.end(),
                           cout << &_1 ->* &ArgumentDescription::name << " - "
                           << &_1 ->* &ArgumentDescription::description << "\n" );
        }
        cout <<endl;
    }
                
    void HMIConsoleInput<SupportComponent< MakeExtension<KernelBaseFunction, ExecutionExtension>::Result::CommonBase > >::printSource( const std::string m )
    {
        using boost::lambda::_1;
        std::vector<ArgumentDescription> args;
        args = dataobject->getArgumentList( m );
        cout << "  Source   : " << m <<endl;
        if (args.begin() != args.end() ){
            //int i = 0;
            cout << "  Argument :"<<endl;
            std::for_each( args.begin(), args.end(),
                           cout << &_1 ->* &ArgumentDescription::name << " - "
                           << &_1 ->* &ArgumentDescription::description << "\n" );
        }
        cout <<endl;
    }
                
    void HMIConsoleInput<SupportComponent< MakeExtension<KernelBaseFunction, ExecutionExtension>::Result::CommonBase > >::startButton()
    {
        start = true;
        startEvent.fire();
    }

    void HMIConsoleInput<SupportComponent< MakeExtension<KernelBaseFunction, ExecutionExtension>::Result::CommonBase > >::stopButton()
    {
        start = false;
    }

    bool HMIConsoleInput<SupportComponent< MakeExtension<KernelBaseFunction, ExecutionExtension>::Result::CommonBase > >::startPushed() const
    {
        return start;
    }

    DataSourceFactoryInterface* HMIConsoleInput<SupportComponent< MakeExtension<KernelBaseFunction, ExecutionExtension>::Result::CommonBase > >::createDataSourceFactory()
    {
        TemplateDataSourceFactory< HMIConsoleInput<SupportComponent< MakeExtension<KernelBaseFunction, ExecutionExtension>::Result::CommonBase > > >* ret =
            newDataSourceFactory( this );
        ret->add( "startPushed", 
                  data( &HMIConsoleInput<SupportComponent< MakeExtension<KernelBaseFunction, ExecutionExtension>::Result::CommonBase > >::startPushed, "Is the start button pushed ? " ) );
        return ret;
    }
#endif
}
