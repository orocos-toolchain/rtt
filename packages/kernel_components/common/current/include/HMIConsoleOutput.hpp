#ifndef HMI_CONSOLE_OUTPUT_HPP
#define HMI_CONSOLE_OUTPUT_HPP

#include <execution/TemplateCommandFactory.hpp>
#include <execution/TemplateDataSourceFactory.hpp>
#include <iostream>

namespace ORO_ControlKernel
{

    /**
     * This component can be used to display messages on the
     * standard output. It should not be used in hard realtime
     * programs, but is nice for testing program and state scripts.
     * HMI == Human-Machine Interface
     */
    template< class Base>
    class HMIConsoleOutput
        : public Base
    {
    public :
        HMIConsoleOutput()
            : Base("console")
              {}

        /**
         * Display a message on standard output.
         */
        void display(const std::string & what)
        {
            std::cout << "HMIConsoleOutput : "<< what <<std::endl;
        }

        template< class T >
        bool true_gen(const T& t) const
        {
            return true;
        }

        // Commands are display commands.
        CommandFactoryInterface* createCommandFactory()
        {
            TemplateCommandFactory< HMIConsoleOutput<Base> >* ret =
                newCommandFactory( this );
            ret->add( "display", 
                      command( &HMIConsoleOutput<Base>::display,
                               &HMIConsoleOutput<Base>::true_gen,
                               "Display a message on the console",
                               "message","The message to be displayed"
                               ) );
            return ret;
        }
    };

}

#endif
