#ifndef _CONFIGURATION_HPP_
#define _CONFIGURATION_HPP_

namespace ORO_KinDyn {


class Configuration {
public:

    Configuration()
    {
	    configuration = 0;
    }

    virtual ~Configuration()
    {
    }

    Configuration& operator=(const Configuration &conf)
    {
	    configuration = conf.configuration;
	    return *this;
    }
    
    virtual void set ( const int configurationPart )
    {
	    configuration |= configurationPart;
    }

    bool get ( const int configurationPart ) const
    {
        return configuration & configurationPart;
    }

    bool isConfig( const int configurationPart ) const
    {
	    return get(configurationPart);
    }
    
    void clear()
    {
        configuration = 0;
    }

protected:
    int configuration;
};

}
#endif
