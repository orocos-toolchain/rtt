
#ifndef OROCOS_SIGNAL_TEMPLATE_HEADER_INCLUDED
#define OROCOS_SIGNAL_TEMPLATE_HEADER_INCLUDED
#include "signal_base.hpp"
#include <os/MutexLock.hpp>
#endif // !OROCOS_SIGNAL_TEMPLATE_HEADER_INCLUDED

// Include the appropriate functionN header
#define OROCOS_SIGNAL_FUNCTION_N_HEADER BOOST_JOIN(<boost/function/function,OROCOS_SIGNAL_NUM_ARGS.hpp>)
#include OROCOS_SIGNAL_FUNCTION_N_HEADER

// Determine if a comma should follow a listing of the arguments/parameters
#if OROCOS_SIGNAL_NUM_ARGS == 0
#  define OROCOS_SIGNAL_COMMA_IF_NONZERO_ARGS
#else
#  define OROCOS_SIGNAL_COMMA_IF_NONZERO_ARGS ,
#endif // OROCOS_NUM_ARGS > 0

// Define class names used
#define OROCOS_SIGNAL_N BOOST_JOIN(signal,OROCOS_SIGNAL_NUM_ARGS)
#define OROCOS_SIGNAL_FUNCTION_N BOOST_JOIN(boost::function,OROCOS_SIGNAL_NUM_ARGS)
#define OROCOS_SIGNAL_CONNECTION_N BOOST_JOIN(connection,OROCOS_SIGNAL_NUM_ARGS)


namespace sigslot {

    namespace detail {

        template<class SlotFunction>
        class OROCOS_SIGNAL_CONNECTION_N : public connection_base
        {
        public:
            typedef SlotFunction slot_function;
            OROCOS_SIGNAL_CONNECTION_TYPEDEFS

            OROCOS_SIGNAL_CONNECTION_N(signal_base* s, const slot_function& f)
                : connection_base(s), func(f)
            {
            }

            void emit(OROCOS_SIGNAL_PARMS)
            {
                func(OROCOS_SIGNAL_ARGS);
            }
        private:
            slot_function func;
        };

    }

	template<typename R, OROCOS_SIGNAL_TEMPLATE_PARMS OROCOS_SIGNAL_COMMA_IF_NONZERO_ARGS
             class SlotFunction = OROCOS_SIGNAL_FUNCTION_N< R OROCOS_SIGNAL_COMMA_IF_NONZERO_ARGS OROCOS_SIGNAL_TEMPLATE_ARGS> >
	class OROCOS_SIGNAL_N : public detail::signal_base
	{
		OROCOS_SIGNAL_N(const OROCOS_SIGNAL_N< R, OROCOS_SIGNAL_TEMPLATE_ARGS OROCOS_SIGNAL_COMMA_IF_NONZERO_ARGS SlotFunction>& s);
	public:
        typedef SlotFunction slot_function_type;
        typedef detail::OROCOS_SIGNAL_CONNECTION_N<SlotFunction> connection_impl;

        typedef R result_type;
        OROCOS_SIGNAL_ARG_TYPES

#if OROCOS_SIGNAL_NUM_ARGS == 1
        typedef arg1_type first_argument_type;
#endif
#if OROCOS_SIGNAL_NUM_ARGS == 2
        typedef arg1_type first_argument_type;
        typedef arg2_type second_argument_type;
#endif

		OROCOS_SIGNAL_N()
		{
		}

        handle connect(const slot_function_type& f )
        {
            handle h = this->setup(f);
            h.connect();
            return h;
        }

        handle setup(const SlotFunction& f )
		{
			connection_t conn = 
				new connection_impl(this, f);
            this->conn_setup( conn );
            return handle(conn);
		}

		void emit(OROCOS_SIGNAL_PARMS)
		{
			//lock_block<mt_policy> lock(this);
            ORO_OS::MutexLock lock(m);
            iterator it = mconnections.begin();
            const_iterator end = mconnections.end();
            for (; it != end; ++it ) {
                connection_impl* ci = static_cast<connection_impl*>( it->get() );
                if (ci)
                    ci->emit(OROCOS_SIGNAL_ARGS); // this if... race is guarded by the mutex.
            }

            this->cleanup();
		}

		void operator()(OROCOS_SIGNAL_PARMS)
		{
            this->emit(OROCOS_SIGNAL_ARGS);
		}

		void fire(OROCOS_SIGNAL_PARMS)
		{
            this->emit(OROCOS_SIGNAL_ARGS);
		}
	};

} // namespace sigslot


#undef OROCOS_SIGNAL_FUNCTION_N_HEADER
#undef OROCOS_SIGNAL_COMMA_IF_NONZERO_ARGS
#undef OROCOS_SIGNAL_N 
#undef OROCOS_SIGNAL_FUNCTION_N 
#undef OROCOS_SIGNAL_CONNECTION_N

