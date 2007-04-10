/***************************************************************************
  tag: Peter Soetens  Wed Jan 18 14:11:39 CET 2006  TypeStream.cxx 

                        TypeStream.cxx -  description
                           -------------------
    begin                : Wed January 18 2006
    copyright            : (C) 2006 Peter Soetens
    email                : peter.soetens@mech.kuleuven.be
 
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
 
 
#include <TypeStream.hpp>
#include <sstream>

namespace {
    // Code ripped from KDL 0.2


    // Eats space-like characters and comments
    // possibly returns the number of space-like characters eaten.
    int _EatSpace( std::istream& is,int* countp=NULL) {
        int ch;
        int count;
        count=-1;
        do { 
            if( !is )
                return '!';

            ch = is.get(); 
            count++;
        } while ((ch==' ')||(ch=='\n')||(ch=='\t'));
        if (countp!=NULL) *countp =  count;
        return ch;
    }

    // Eats whites, returns, tabs and the delim character
    //  Checks wether delim char. is encountered.
    bool Eat( std::istream& is, int delim )
    {   
        if( !is )
            return false;
        int ch;
        ch=_EatSpace(is);
        if (ch != delim) {
            return false;
        }
        ch=_EatSpace(is);   
        is.putback(ch);
        return true;
    }

    // Eats whites, returns, tabs and the delim character
    //  Checks wether delim char. is encountered.
    // EatEnd does not eat all space-like char's at the end.
    bool EatEnd( std::istream& is, int delim )
    {   
        if( !is )
            return false;
        int ch;
        ch=_EatSpace(is);
        if (ch != delim) {
            return false;
        }
        return true;
    }

}

namespace RTT
{


    using namespace std;
    std::ostream& operator<<(std::ostream& os, const std::vector<double>& v)
    {
        std::stringstream ss;
        ss << "{";
        for(unsigned int  i = 0; i < v.size(); ++i) {
            ss << v[i];
            if (i + 1 != v.size() ) {
                ss << ", ";
            }
        }
        ss << "}";
        os << ss.str();
        return os;
    }

    std::istream& operator>>(std::istream& os, std::vector<double>& v)
    {
        std::vector<double> t( v.size() );
        Eat( os, '{' );
        for(unsigned int  i = 0; i < v.size(); ++i) {
            os >> t[i];
            if (i + 1 != v.size() ) {
                Eat(os, ',');
            }
        }
        EatEnd( os, '}' );
        if (os )
            v = t;
        return os;
    }
}

