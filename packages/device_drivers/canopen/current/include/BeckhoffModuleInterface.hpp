#ifndef BECKHOFFTERMINALINTERFACE_HPP
#define BECKHOFFTERMINALINTERFACE_HPP


namespace Beckhoff
{
    
	/**
	 */
	struct BeckhoffTerminalInterface 
    {
        virtual ~BeckhoffTerminalInterface() {}

        virtual void initialize() = 0;

        virtual unsigned int terminalNr() = 0;
        
	};
			
}


#endif

