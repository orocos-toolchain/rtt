/***************************************************************************
  tag: Peter Soetens  Wed Jan 18 14:11:38 CET 2006  signal_template.hpp

                        signal_template.hpp -  description
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



#ifndef OROCOS_SIGNAL_TEMPLATE_HEADER_INCLUDED
#define OROCOS_SIGNAL_TEMPLATE_HEADER_INCLUDED
#include "rtt/internal/SignalBase.hpp"
#include "rtt/internal/NA.hpp"

#ifdef ORO_SIGNAL_USE_LIST_LOCK_FREE
#include <boost/bind.hpp>
#else
#include "rtt/os/MutexLock.hpp"
#endif
#endif // !OROCOS_SIGNAL_TEMPLATE_HEADER_INCLUDED

// Define class names used
#define OROCOS_SIGNAL_N BOOST_JOIN(signal,OROCOS_SIGNATURE_NUM_ARGS)
#define OROCOS_SIGNAL_CONNECTION_N BOOST_JOIN(connection,OROCOS_SIGNATURE_NUM_ARGS)


namespace RTT {

    namespace internal {

        template<class SlotFunction>
        class OROCOS_SIGNAL_CONNECTION_N : public ConnectionBase
        {
        public:
            typedef SlotFunction slot_function;
            typedef SlotFunction function_type;
            OROCOS_SIGNATURE_TYPEDEFS

            OROCOS_SIGNAL_CONNECTION_N(SignalBase* s, const slot_function& f)
                : ConnectionBase(s), func(f)
            {
            }

            void emit(OROCOS_SIGNATURE_PARMS)
            {
                if (this->mconnected)
                    func(OROCOS_SIGNATURE_ARGS);
            }
        private:
            slot_function func;
        };

	template<typename R, OROCOS_SIGNATURE_TEMPLATE_PARMS OROCOS_SIGNATURE_COMMA_IF_NONZERO_ARGS
             class SlotFunctionT = OROCOS_SIGNATURE_FUNCTION_N< R OROCOS_SIGNATURE_COMMA_IF_NONZERO_ARGS OROCOS_SIGNATURE_TEMPLATE_ARGS> >
	class OROCOS_SIGNAL_N
        : public SignalBase
	{
		OROCOS_SIGNAL_N(const OROCOS_SIGNAL_N< R, OROCOS_SIGNATURE_TEMPLATE_ARGS OROCOS_SIGNATURE_COMMA_IF_NONZERO_ARGS SlotFunctionT>& s);

	public:
        typedef SlotFunctionT slot_function_type;
        typedef OROCOS_SIGNAL_CONNECTION_N<SlotFunctionT> connection_impl;

        typedef R result_type;
        OROCOS_SIGNATURE_ARG_TYPES

#if OROCOS_SIGNATURE_NUM_ARGS == 1
        typedef arg1_type first_argument_type;
#endif
#if OROCOS_SIGNATURE_NUM_ARGS == 2
        typedef arg1_type first_argument_type;
        typedef arg2_type second_argument_type;
#endif

		OROCOS_SIGNAL_N()
		{
		}

        Handle connect(const slot_function_type& f )
        {
            Handle h = this->setup(f);
            h.connect();
            return h;
        }

        Handle setup(const slot_function_type& f )
		{
			connection_t conn(
				new connection_impl(this, f) );
            this->conn_setup( conn );
            return Handle(conn);
		}

    private:
        static void emitImpl(const connection_t& c
#if OROCOS_SIGNATURE_NUM_ARGS != 0
                                        , OROCOS_SIGNATURE_PARMS
#endif
                         )
        {
            static_cast<connection_impl*>(c.get())->emit(OROCOS_SIGNATURE_ARGS);
        }

    public:
		R emit(OROCOS_SIGNATURE_PARMS)
		{
#ifdef ORO_SIGNAL_USE_LIST_LOCK_FREE
            this->emitting = true;

            // this code did initially not work under gcc 4.0/ubuntu breezy.
            // connection_t::get() const becomes an undefined symbol.
            // works under gcc 3.4
            mconnections.apply( boost::bind(&OROCOS_SIGNAL_N::emitImpl,
                                                    _1
#if OROCOS_SIGNATURE_NUM_ARGS != 0
                                                    ,OROCOS_SIGNATURE_ARGS
#endif
                                                    ) );
            this->emitting = false;
#else
            os::MutexLock lock(m);
            if (this->emitting)
                return NA<R>::na(); // avoid uglyness : Handlers calling emit.
            this->emitting = true;
            iterator it = mconnections.begin();
            const_iterator end = mconnections.end();
            for (; it != end; ++it ) {
                connection_impl* ci = static_cast<connection_impl*>( it->get() );
                if (ci)
                    ci->emit(OROCOS_SIGNATURE_ARGS); // this if... race is guarded by the mutex.
            }
            this->emitting = false;
            this->cleanup();
#endif
            return NA<R>::na();
		}

		R operator()(OROCOS_SIGNATURE_PARMS)
		{
            return this->emit(OROCOS_SIGNATURE_ARGS);
		}

		R fire(OROCOS_SIGNATURE_PARMS)
		{
            return this->emit(OROCOS_SIGNATURE_ARGS);
		}

        virtual int arity() const { return OROCOS_SIGNATURE_NUM_ARGS; }
	};

}} // namespace sigslot


#undef OROCOS_SIGNAL_N
#undef OROCOS_SIGNAL_CONNECTION_N

