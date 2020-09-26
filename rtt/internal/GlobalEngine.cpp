/***************************************************************************
  tag: The SourceWorks  Tue Sep 7 00:55:18 CEST 2010  GlobalEngine.cpp

                        GlobalEngine.cpp -  description
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


#include "rtt/internal/GlobalEngine.hpp"
#include "rtt/Activity.hpp"

namespace RTT
{

    namespace internal
    {
        static GlobalEngine* mengine = 0;

        GlobalEngine::GlobalEngine(int scheduler, int priority, unsigned cpu_affinity)
            : mact( new Activity( scheduler, priority, 0, cpu_affinity, this, "GlobalEngine") )
        {
            mact->start();
        }

        GlobalEngine::~GlobalEngine()
        {
            mact->stop();
            mact.reset();
        }

        ExecutionEngine* GlobalEngine::Instance(int scheduler, int priority, unsigned cpu_affinity) {
            if (mengine == 0) {
                mengine = new GlobalEngine(scheduler, priority, cpu_affinity);
            }
            return mengine;
        }
        void GlobalEngine::Release() {
            delete mengine;
            mengine = 0;
        }
    }

}
