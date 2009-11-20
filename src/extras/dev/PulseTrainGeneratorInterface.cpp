/***************************************************************************
  tag: Klaas Gadeyne  Fri Sep 22 13:19:07 CEST 2006  PulseTrainGeneratorInterface.cpp

                        PulseTrainGeneratorInterface.cpp -  description
                           -------------------
    begin                : Fri Sep 22 13:19:07 CEST 2006
    copyright            : (C) 2006 FMTC
    email                : klaas gadeyne at fmtc be

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

#ifdef ORO_PRAGMA_INTERFACE
#pragma implementation
#endif
#include "PulseTrainGeneratorInterface.hpp"

namespace RTT {
    using namespace dev;
    NameServer<PulseTrainGeneratorInterface*> PulseTrainGeneratorInterface::nameserver;
}
