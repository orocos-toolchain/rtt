#ifndef PI_PROPERTIES_TABLEHEADER_SERIALIZER
#define PI_PROPERTIES_TABLEHEADER_SERIALIZER

#include <corelib/Property.hpp>
#include "StreamProcessor.hpp"

namespace ORO_CoreLib
{
    using namespace std;

    /**
     * A Marshaller for generating headers of tables. It is used
     * in conjunction with TableMarshaller and generates a header
     * for the following numbers.
     *
     * @todo : build a tree for formatting and print all on flush().
     */
    template<typename o_stream>
    class TableHeaderMarshaller 
    : public Marshaller, public PropertyIntrospection, public StreamProcessor<o_stream>
    {
        int level;
        int line;
        vector<string> header;
        public:
            typedef o_stream output_stream;
            typedef o_stream OutputStream;
            
            TableHeaderMarshaller(output_stream &os) :
                    StreamProcessor<o_stream>(os), level(0), line(1)
            {
                // sits ready for storing next column aligning.
                header.push_back(string(""));
            }

            virtual ~TableHeaderMarshaller() {}
            
			virtual void serialize(const Property<bool> &v) 
			{ 
                    store( v.getName() );
                //_os << " "<< v.getName();
			}

			virtual void serialize(const Property<char> &v) 
			{ 
                    store( v.getName() );
                //_os << " "<< v.getName();
			}

			virtual void serialize(const Property<int> &v) 
			{ 
                    store( v.getName() );
                //_os << " "<< v.getName();
			}
			
			virtual void serialize(const Property<double> &v) 
			{
                    store( v.getName() );
                //_os << " "<< v.getName();
			}
            
			virtual void serialize(const Property<std::string> &v) 
			{
                    store( v.getName() );
                //_os << " "<< v.getName();
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
                    (*i)->identify(this);
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
                    header.push_back(string(""));
                }
                header[line-1] += std::string(" | ") + s;
                //cout << line <<" - "<<level<<endl;
               
               return header[line-1].length();
            }

            virtual void serialize(const Property<PropertyBag> &v) 
			{
                if ( line == int(header.size() ) )
                    header.push_back(string(""));
                /**
                 * Serializing a propery<bag> : pad the line below with spaces.
                 */
                if ( int(header[line-1].length() - header[line].length()) > 0 )
                {
                    header[line] += string(" | ");
                    header[line] += string( header[line-1].length() - header[line].length() ,' ');
                }

                /**
                 * Print our name
                 */
                
                string name = v.getName();
                if ( v.get().getType() != "type_less")
                    name+= string(" <") + v.get().getType() + string(">");
                store( name ) ;
                
                /**
                 * Serialize all properties on the line below.
                 */
                line++;
                if ( v.get().getProperties().empty() )
                    store( string("<empty>") );
                else
                    serialize(v.get());
                line--;
                
                /**
                 * Pad this line with spaces
                 */
                if ( int(header[line].length()) - int(header[line -1].length()) > 0)
                    header[line-1] += string( header[line].length() - header[line-1].length(), ' ');
            }

            virtual void flush() 
            {
                for (vector<string>::iterator it = header.begin(); it != header.end(); ++it)
                    if ( !it->empty())
                        *s << *it <<std::string(" |")<<endl;
                // TODO : buffer for formatting and flush here.
                level = 0;
                line  = 1;
                header.clear();
                header.push_back(string(""));
            }
            
			virtual void introspect(const Property<bool> &v) 
			{ 
                serialize(v);
			}

			virtual void introspect(const Property<char> &v) 
			{ 
                serialize(v);
			}

			virtual void introspect(const Property<int> &v) 
			{ 
                serialize(v);
			}
			
			virtual void introspect(const Property<double> &v) 
			{
                serialize(v);
			}

			virtual void introspect(const Property<std::string> &v) 
			{
                serialize(v);
			}
			
            virtual void introspect(const Property<PropertyBag> &v) 
			{
				serialize(v);
            }
	};
}
#endif
