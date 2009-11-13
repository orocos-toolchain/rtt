/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:17:00 CEST 2002  rtstreams.hpp 

                        rtstreams.hpp -  description
                           -------------------
    begin                : Thu October 10 2002
    copyright            : (C) 2002 Peter Soetens
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
 
#include "fosi.h"

#include "rtconversions.hpp"
#include "rtstreambufs.hpp"

#ifndef RTSTREAMS_HPP
#define RTSTREAMS_HPP

/**
 * @file (Almost) Real-Time output streams.
 * If you really have to print something out from a RealTime thread,
 * you can use the streams of the os namespace, which will call
 * the rtos_printf functions (which are supposed to be as realtime as possible)
 * of the OS you are using. Be warned, these classes have not been tested
 * extensively and might in certain cases still break hard-realtime behaviour.
 * avoid using it from your most critical threads and production code.
 */

namespace RTT
{
namespace os
{

    /**
     * This is a stream class for use in limited environments.
     * It is supposed to mimic the std::iostream classes to some extent.
     * Formatting and locale support are not provided for example.
     *
     * The default stream class does not store anything and can
     * thus be used as 'grabage bin'.
     *
     * @todo TODO there is no streambuf and string is used, maybe 
     * a streamsbuf might be better, for encapsulating buffering
     * policies (like BS meant it of course). Another todo is to
     * have at least the save get and put/write functions as in istream
     * and ostream. Thanks to the GCC 3.X cleanup, we might be able to mimic
     * quite a lot.
     */
    class RTT_API basic_streams
    {

        public:
            virtual ~basic_streams();

            /**
             * Write methods. These do nothing in the stream class,
             * but can be overloaded in subclasses to implement the proper
             * functionality.
             */
/*
            virtual void write( const char * c );
            virtual void write( const char * c, size_t n );
            virtual void write( const string s );
            virtual void put( char ch );
*/
            /**
             * Read methods. These do nothing in the stream class,
             * but can be overloaded in subclasses to implement the proper
             * functionality.
             */
/*            
            virtual string read( int n );
            virtual string read( char delimiter );
            virtual char read();
*/
    };

    /**
     * An basic_istream is a stream which can be read. Write
     * operations will have no effect.
     *
     * @detail Each part that is read from the string is discared,
     * so no seeking is possible. The problem is otherwise that the
     * string in iostreams would grow unlimited.
     */
    class RTT_API basic_istreams
        : virtual public basic_streams
    {

        public:
            typedef streambufs::streamsize streamsize;

            basic_istreams( streambufs& s ) : buf(s) {}
            virtual ~basic_istreams();

            int get();
            basic_istreams& get(char& c);
            basic_istreams& get(char* c, streamsize n, char delim);
            basic_istreams& get(char* c, char delim);

            basic_istreams& read( char* c, streamsize n );
            streamsize readsome( char* c, streamsize n);


            basic_istreams& operator>>( int &i );
            basic_istreams& operator>>( char &c );
            basic_istreams& operator>>( double &f );
            basic_istreams& operator>>( std::string &s );
            basic_istreams& operator>>( unsigned int &u );
        private:
            streambufs& buf;
    };

    /**
     * An basic_ostreams is a stream which can be written to.
     * Read operations will have no effect.
     */
    class RTT_API basic_ostreams : virtual public basic_streams
    {

        public:
            typedef streambufs::streamsize streamsize;

            basic_ostreams( streambufs& s ) : buf(s) {}
            virtual ~basic_ostreams();

            virtual basic_ostreams& put( char c );
            virtual basic_ostreams& write( const char * c, streamsize n );

            /**
             * Operators. These can not be virtual, so each one calls the
             * appropriate read or write method defined above.
             */
            basic_ostreams& operator<<( int i );
            basic_ostreams& operator<<( long i );
            basic_ostreams& operator<<( char c );
            basic_ostreams& operator<<( char * c );
            basic_ostreams& operator<<( double f );
            basic_ostreams& operator<<( std::string s );
            basic_ostreams& operator<<( unsigned int u );
            basic_ostreams& operator<<( basic_ostreams & ( *f ) ( basic_ostreams & ) );
        private:
            streambufs& buf;
    };

//#ifdef __KERNEL__
// defined in the namespace os
    /**
     * Flush and newline.
     */
    basic_ostreams& endl( basic_ostreams& );
//#endif

    /**
     * An IO stream based on strings.
     */
    class RTT_API basic_iostreams
        : public basic_istreams, public basic_ostreams
    {
        public:
        basic_iostreams( streambufs& s) 
            : basic_istreams(s), basic_ostreams(s) {}
    };
            

    struct print_helper { printbufs printer; };

    /**
     * The printstream is a stream for printing characters
     * to the terminal window. It mimics the behaviour of
     * iostreams cout.
     */
    class RTT_API printstream
        : private print_helper, public basic_ostreams
    {

        public:
            printstream() : basic_ostreams(printer) {}
            virtual ~printstream();
    };

    /**
     * Console Output.
     */
    extern printstream cout;
    
    struct string_helper { string_helper() : str() {}; string_helper(const std::string& _init) :  str(_init) {};  stringbufs str; };
    
    /**
     * Stringstreams, are not guaranteed to be hard realtime.
     */
    class RTT_API stringstreams
        : private string_helper, public basic_iostreams
        {
            public:
                stringstreams(const std::string& _init) : string_helper(_init), basic_iostreams(str) {}
                stringstreams() : string_helper(), basic_iostreams(str) {}
                virtual ~stringstreams();
        };
        
}
}
#endif
