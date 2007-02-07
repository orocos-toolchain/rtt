/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:20 CET 2004  TableHeaderMarshaller.hpp 

                        TableHeaderMarshaller.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU General Public                   *
 *   License as published by the Free Software Foundation;                 *
 *   version 2 of the License.                                             *
 *                                                                         *
 *   As a special exception, you may use this file as part of a free       *
 *   software library without restriction.  Specifically, if other files   *
 *   instantiate templates or use macros or inline functions from this     *
 *   file, or you compile this file and link it with other files to        *
 *   produce an executable, this file does not by itself cause the         *
 *   resulting executable to be covered by the GNU General Public          *
 *   License.  This exception does not however invalidate any other        *
 *   reasons why the executable file might be covered by the GNU General   *
 *   Public License.                                                       *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU General Public             *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/ 
 
#ifndef PI_PROPERTIES_TABLEHEADER_SERIALIZER
#define PI_PROPERTIES_TABLEHEADER_SERIALIZER

#include <Property.hpp>
#include <PropertyIntrospection.hpp>
#include "StreamProcessor.hpp"

namespace RTT
{
    /**
     * A Marshaller for generating headers of tables. It is used
     * in conjunction with TableMarshaller and generates a header
     * for the following numbers.
     *
     * @todo : build a tree for formatting and print all on flush().
     */
    template<typename o_stream>
    class TableHeaderMarshaller 
    : public Marshaller, public StreamProcessor<o_stream>
    {
        int level;
        int line;
        std::vector<std::string> header;
        public:
            typedef o_stream output_stream;
            typedef o_stream OutputStream;
            
            TableHeaderMarshaller(output_stream &os) :
                    StreamProcessor<o_stream>(os), level(0), line(1)
            {
                // sits ready for storing next column aligning.
                header.push_back(std::string(""));
            }

            virtual ~TableHeaderMarshaller() {}
            
			virtual void serialize(PropertyBase* v) 
			{ 
                Property<PropertyBag>* bag = dynamic_cast< Property<PropertyBag>* >( v );
                if ( bag )
                    this->serialize( *bag );
                else
                    store( v->getName() );
			}


            virtual void serialize(const PropertyBag &v) 
			{
                // A Bag has no name
                //

                //*s <<"| Data Set <"<<v.getType()<<"> containing :"<< std::endl <<"| ";
                //++line;
                /*
                for (
                    PropertyBag::const_iterator i = v.getProperties().begin();
                    i != v.getProperties().end();
                    i++ )
                {
                    // *s << (*i)->getName() <<" | ";
                    store( (*i)->getName() );
                }
                */
                ++level;
                //++line;
                //*s << " |"<<std::endl;
                for (
                    PropertyBag::const_iterator i = v.getProperties().begin();
                    i != v.getProperties().end();
                    i++ )
                {
                    
                    this->serialize(*i);
                }
                --level;
                //*s << " |"<<std::endl;
			}

            /**
             * @return the number of characters on this line.
             */
            int store(const std::string& s)
            {
                if ( line == int(header.size()) )
                {
                    // next line
                    header.push_back(std::string(""));
                }
                header[line-1] += std::string(" | ") + s;
               
                return header[line-1].length();
            }

            virtual void serialize(const Property<PropertyBag> &v) 
			{
                if ( line == int(header.size() ) )
                    header.push_back(std::string(""));
                /**
                 * Serializing a propery<bag> : pad the line below with spaces.
                 */
                if ( int(header[line-1].length()) - int(header[line].length()) > 0 )
                {
                    // add separator
                    header[line] += std::string(" | ");
                    // pad further if necessary.
                    if ( int(header[line-1].length()) - int(header[line].length()) > 0 )
                        header[line] += std::string( header[line-1].length() - header[line].length() ,' ');
                }

                /**
                 * Print our name
                 */
                
                std::string name = v.getName();
                if ( v.get().getType() != "type_less")
                    name+= std::string(" <") + v.get().getType() + std::string(">");
                store( name ) ;
                
                /**
                 * Serialize all properties on the line below.
                 */
                line++;
                if ( v.get().getProperties().empty() )
                    store( std::string("<empty>") );
                else
                    serialize(v.get());
                line--;
                
                /**
                 * Pad this line with spaces
                 */
                if ( int(header[line].length()) - int(header[line -1].length()) > 0)
                    header[line-1] += std::string( header[line].length() - header[line-1].length(), ' ');
            }

            virtual void flush() 
            {
                for (std::vector<std::string>::iterator it = header.begin(); it != header.end(); ++it)
                    if ( !it->empty())
                        *this->s << *it <<std::string(" |")<<std::endl;
                // TODO : buffer for formatting and flush here.
                level = 0;
                line  = 1;
                header.clear();
                header.push_back(std::string(""));
            }
	};
}
#endif
