/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:19 CET 2004  rtstreambufs.hpp 

                        rtstreambufs.hpp -  description
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
 
#ifndef RTSTREAMBUFS_HPP
#define RTSTREAMBUFS_HPP

#include <string>
#include "fosi.h"

namespace RTT
{namespace rt_std
{


    /**
     * A base class for all stream buffer operations
     */
    class streambufs
    {
        public:
            typedef unsigned int streamsize;
        
            virtual ~streambufs() {}
            virtual int sgetc() = 0;
            virtual streamsize sgetn(char* s, streamsize n ) = 0;
            virtual int sputc(char c) = 0;
            virtual streamsize sputn(const char* s, streamsize n ) = 0;
    };


    /**
     * A streambufs implementation based on strings.
     * It is initialized to a stringsize of 512 bytes.
     * You can increase it by calling buf.str().reserve( x_bytes );
     */
    class stringbufs
        : public streambufs
    {
        public:
            typedef streambufs::streamsize streamsize;
            
            static const streamsize buf_size = 512;
            
            stringbufs(const std::string& c="") : _s(c), ptr(0), end(0) { _s.reserve( buf_size ); }

            virtual ~stringbufs() {}
            
            virtual int sgetc()
            {
                if ( _s.empty() || ptr == _s.length() ) return EOF;
                return _s[ptr++];
            }
            
            virtual streamsize sgetn( char* s, streamsize n )
            {
                if ( _s.empty() || ptr ==  _s.length() ) return 0;
                streamsize len = (n <= _s.length() ? n : _s.length() );
                _s.copy(s, len, ptr);
                ptr += len;
                return len;
            }
            
            virtual int sputc(char c)
            {
                cleanup();
                
                if ( ptr == _s.capacity() )
                    return EOF;
                _s.push_back(c);
                return 1;
            }
            
            virtual streamsize sputn(const char* s, streamsize n )
            {
                cleanup(n);
                if ( ptr == _s.capacity() )
                    return 0;
                _s.append(s,n); // possibly expand _s !.
                return n;
            }
            
            std::string str() const
            { 
                return _s.substr(ptr, _s.length() - ptr);
            }

            void str( std::string& new_str )
            {
                ptr = 0;
                _s = new_str;
            }
        private:
            void clear()
            {
                ptr = 0;
                end = 0;
                _s.erase();
            }

            /**
             * @param free, the minimum space that must be free.
             */
            void cleanup(int free = 1)
            {
                if (ptr == _s.length() && !_s.empty() )
                    clear(); // when all is read, clear all
                if ( ptr != 0 && _s.length() + free >= _s.capacity() )
                {
                    _s.erase(0, ptr);// when some _must_ and _can_ be freed.
                    ptr = 0;
                }
            }

            std::string _s;
            streamsize ptr;
            streamsize end;
   };

    class printbufs
        : public streambufs
    {
        public:
            typedef streambufs::streamsize streamsize;
            
            virtual int sgetc()
            {
                return EOF;
            }
            
        virtual streamsize sgetn( char* /*s*/, streamsize /*n*/ )
            {
                return 0;
            }
            
            virtual int sputc(char c)
            {
                rtos_printf("%c",c);
                return 1;
            }
            
            virtual streamsize sputn(const char* s, streamsize n )
            {
                rtos_printf("%*s", n, s);
                return n;
            }
            
        private:
   };

}}


#endif
