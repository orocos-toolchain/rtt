/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:21 CET 2004  Configuration.hpp 

                        Configuration.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 
 
#ifndef _CONFIGURATION_HPP_
#define _CONFIGURATION_HPP_

namespace ORO_KinDyn 
{

    class Configuration 
    {
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
