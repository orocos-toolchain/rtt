
#include <execution/TemplateCommandFactory.hpp>
#include <execution/TemplateDataSourceFactory.hpp>
#include <iostream>

namespace ORO_ControlKernel
{
    using std::cout;
    using std::cin;
    using std::endl;

    /**
     * This component allows console input to be passed
     * to the program or state scripts. It can be referenced
     * as the 'console' component in these scripts.
     *
     * HMI == Human-Machine Interface
     */
    template< class Base>
    class HMIConsoleInput
        : public Base
    {
        bool start;
        Event startEvent;
    public :
        HMIConsoleInput()
            : Base("console_input"), start(false),
              startEvent(Event::SYNASYN,"HMIConsoleInput::StartEvent") {}


        /**
         * Call this method from ORO_main() to 
         * process keyboard input.
         */
        void loop()
        {
            char c;
            cout << endl<< "This simple console reader can only accept"<<endl;
            cout << "start and stop commands to start or stop the running program"<<endl;
            while (1)
                {
                    cout << "Press s to start or p to stop the current program-script (press q to quit) :" ;
                    cin >> c;
                    switch (c)
                        {
                        case 's':
                            startButton();
                            break;
                        case 'p':
                            stopButton();
                            break;
                        case 'q':
                            stopButton();
                            return;
                        }
                    cout <<endl;
                }
        }
                

        void startButton()
        {
            start = true;
            startEvent.fire();
        }

        void stopButton()
        {
            start = false;
        }

        bool startPushed() const
        {
            return start;
        }

        // The only data we export is the user's start input.
        DataSourceFactory* createDataSourceFactory()
        {
            TemplateDataSourceFactory< HMIConsoleInput<Base> >* ret =
                newDataSourceFactory( this );
            ret->add( "startPushed", 
                      data( &HMIConsoleInput<Base>::startPushed, "Is the start button pushed ? " ) );
            return ret;
        }

    };
}
