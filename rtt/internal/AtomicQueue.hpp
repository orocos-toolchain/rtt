/***************************************************************************
  tag: The SourceWorks  Tue Sep 7 00:55:18 CEST 2010  AtomicQueue.hpp

                        AtomicQueue.hpp -  description
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


#ifndef ORO_CORELIB_ATOMIC_QUEUE_HPP
#define ORO_CORELIB_ATOMIC_QUEUE_HPP

namespace RTT
{
    namespace internal {

    /**
     * An atomic, non-blocking single ended queue (FIFO) for storing
     * a \b pointer to \a T.
     *
     * @warning You can not store null pointers.
     * @param T The pointer type to be stored in the Queue.
     * Example : AtomicQueue< A* > is a queue of pointers to A.
     *
     * @ingroup CoreLibBuffers
     */
    template<class T>
    class AtomicQueue
    {
    public:
        typedef unsigned int size_type;

        virtual ~AtomicQueue() {}

        /**
         * Inspect if the Queue is full.
         * @return true if full, false otherwise.
         */
        virtual bool isFull() const = 0;

        /**
         * Inspect if the Queue is empty.
         * @return true if empty, false otherwise.
         */
        virtual bool isEmpty() const = 0;

        /**
         * Return the maximum number of items this queue can contain.
         */
        virtual size_type capacity() const = 0;

        /**
         * Return the exact number of elements in the queue.
         * This is slow because it scans the whole
         * queue.
         */
        virtual size_type size() const = 0;

        /**
         * Enqueue an item.
         * @param value The value to enqueue, not zero.
         * @return false if queue is full or value is zero, true if queued.
         */
        virtual bool enqueue(const T& value) = 0;

        /**
         * Dequeue an item.
         * @param value The value dequeued.
         * @return false if queue is empty, true if dequeued.
         */
        virtual bool dequeue( T& result ) = 0;

        /**
         * Return the next to be read value.
         */
        virtual const T front() const = 0;

        /**
         * Clear all contents of the Queue and thus make it empty.
         */
        virtual void clear() = 0;
    };

}}

#endif
