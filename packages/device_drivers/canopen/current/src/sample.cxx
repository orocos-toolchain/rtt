


int main()
{
    cbus = new CANBus();
    ccontrol = new CANPieController( cbus, 0 );

    cbus->write(msg);
    
    // CAN aware
    // opstarten bus coupler
    bhbus = new BeckHoffBus();
    
    bcoupler = new BeckhoffBusCoupler(cbus, node_id );

    // default LVD, later aparte f_ies per feature
    ssi_module = new BeckhoffSSIModule(bcoupler, 5 );
    ssi_module = new BeckhoffSSIModule(bcoupler, 6 );
    
    mySSI = SSIInterface::getObjectByName("LVDModule");
    
    ssi_module->getPosition();
    
    return 0;
}
