
#include <fdi/SensorInterface.hpp>
#include <fdi/AnalogInInterface.hpp>

namespace CBDeviceDriver
{
    using namespace ORO_CoreLib;

    /**
     * A ForceSensor measures forces in Newton
     */
class ForceSensor1D
    :public SensorInterface<double>
{
public:
    ForceSensor1D(AnalogInInterface<unsigned int>* input, unsigned int chan, 
                  const Data scaleA = 1, const Data offsetA = 0)
        :dataCard(input), channel(chan), 
         scale(scaleA), offset(offsetA) 
    {}

    virtual int readSensor(Data& data)
    {
        unsigned int d=0;
        dataCard->read(channel, d);
        data = dataCard->lowest() + d/dataCard->resolution();
        data = data * scale + offset;
        std::cout <<"low :"<<dataCard->lowest()<<" res: "<< dataCard->resolution() <<" d:"<<d<<std::endl;
        return 0;
    }

    virtual Data maxMeasurement()
    {
        return dataCard->highest()*scale+offset;
    }

    virtual Data minMeasurement()
    {
        return dataCard->lowest()*scale+offset;
    }
    
    virtual Data zeroMeasurement()    
    {
        return 0;
    }

    virtual void offsetSet( const Data& o)
    {
        offset = o;
    }

    virtual void scaleSet( const Data& s)
    {
        scale = s;
    }
private:
    AnalogInInterface<unsigned int>* dataCard;
    
    unsigned int channel;

    Data scale;
    Data offset;
};

}
