/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:21 CET 2004  Configuration.hpp 

                        Configuration.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
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
