
#pragma implementation
#include "kernel_components/HMIConsoleOutput.hpp"



namespace ORO_ControlKernel
{

    template class HMIConsoleOutput<>;

#if 0
    HMIConsoleOutputImpl::HMIConsoleOutputImpl()
        : HMIConsoleOutputImpl::Base("console_output"), coloron("\033[1;34m"), coloroff("\033[0m")
    {}

    /**
     * @brief Display a message on standard output.
     */
    void HMIConsoleOutputImpl::display(const std::string & what)
    {
        std::cout << coloron << "HMIConsoleOutput : "<< coloroff << what << std::endl;
    }

    /**
     * @brief Display a boolean on standard output.
     */
    void HMIConsoleOutputImpl::displayBool(bool what)
    {
        std::cout << coloron << "HMIConsoleOutput : "<< coloroff << what << std::endl;
    }

    /**
     * @brief Display an integer on standard output.
     */
    void HMIConsoleOutputImpl::displayInt( int what)
    {
        std::cout << coloron << "HMIConsoleOutput : "<< coloroff << what << std::endl;
    }

    /**
     * @brief Display a double on standard output.
     */
    void HMIConsoleOutputImpl::displayDouble( double what )
    {
        std::cout << coloron << "HMIConsoleOutput : "<< coloroff << what << std::endl;
    }

    // Commands are display commands.
    CommandFactoryInterface* HMIConsoleOutputImpl::createCommandFactory()
    {
        TemplateCommandFactory< HMIConsoleOutputImpl >* ret =
            newCommandFactory( this );
        ret->add( "display", 
                  command( &HMIConsoleOutputImpl::display,
                           &HMIConsoleOutputImpl::true_gen,
                           "Display a message on the console",
                           "message","The message to be displayed"
                           ) );
        ret->add( "displayBool", 
                  command( &HMIConsoleOutputImpl::displayBool,
                           &HMIConsoleOutputImpl::true_gen,
                           "Display a boolean on the console",
                           "boolean","The Boolean to be displayed"
                           ) );
        ret->add( "displayInt", 
                  command( &HMIConsoleOutputImpl::displayInt,
                           &HMIConsoleOutputImpl::true_gen,
                           "Display a integer on the console",
                           "integer","The Integer to be displayed"
                           ) );
        ret->add( "displayDouble", 
                  command( &HMIConsoleOutputImpl::displayDouble,
                           &HMIConsoleOutputImpl::true_gen,
                           "Display a double on the console",
                           "double","The Double to be displayed"
                           ) );
        return ret;
    }

#endif
}
