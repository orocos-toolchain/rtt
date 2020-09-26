/***************************************************************************
  tag: The SourceWorks  Tue Sep 7 00:55:18 CEST 2010  SendStatus.cpp

                        SendStatus.cpp -  description
                           -------------------
    begin                : Tue September 07 2010
    copyright            : (C) 2010 The SourceWorks
    email                : peter@thesourceworks.com

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


#include "rtt/SendStatus.hpp"

#include <string>

using namespace std;

namespace RTT {
    std::ostream& operator<<(std::ostream& os, SendStatus fs)
    {
        switch (fs) {
        case CollectFailure:
            os << "CollectFailure";
            break;
        case SendFailure:
            os << "SendFailure";
            break;
        case SendNotReady:
            os << "SendNotReady";
            break;
        case SendSuccess:
            os << "SendSuccess";
            break;
        default:
            os << "InvalidSendStatus";
        }
        return os;
    }

    std::istream& operator>>(std::istream& is, SendStatus& fs)
    {
        // default:
        fs = SendFailure;
        std::string s;
        is >> s;
        if (s == "SendNotReady")
            fs = SendNotReady;
        else if (s == "SendSuccess")
            fs = SendSuccess;
        else if (s == "CollectFailure")
            fs = CollectFailure;

        return is;
    }
}

