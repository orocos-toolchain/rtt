
#ifndef OROCOS_SIGNAL_TEMPLATE_HEADER_INCLUDED
#define OROCOS_SIGNAL_TEMPLATE_HEADER_INCLUDED
#endif // !OROCOS_SIGNAL_TEMPLATE_HEADER_INCLUDED

// Include the appropriate functionN header
#define OROCOS_SIGNAL_FUNCTION_N_HEADER BOOST_JOIN(<boost/function/function,OROCOS_NUM_ARGS.hpp>)
#include OROCOS_SIGNAL_FUNCTION_N_HEADER

// Determine if a comma should follow a listing of the arguments/parameters
#if OROCOS_SIGNAL_NUM_ARGS == 0
#  define OROCOS_SIGNAL_COMMA_IF_NONZERO_ARGS
#else
#  define OROCOS_SIGNAL_COMMA_IF_NONZERO_ARGS ,
#endif // OROCOS_NUM_ARGS > 0

// Define class names used
#define OROCOS_SIGNAL_N BOOST_JOIN(signal,OROCOS_NUM_ARGS)
#define OROCOS_SIGNAL_FUNCTION_N BOOST_JOIN(boost::function,OROCOS_NUM_ARGS)
#define OROCOS_SIGNAL_CONNECTION_N BOOST_JOIN(connection,OROCOS_NUM_ARGS)


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

            void emit(OROCOS_SIGNAL_PARAMS)
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
		OROCOS_SIGNAL_N(const OROCOS_SIGNAL_N< R, OROCOS_SIGNAL_TEMPLATE_PARMS OROCOS_SIGNAL_COMMA_IF_NONZERO_ARGS SlotFunction>& s);
	public:
        typedef SlotFunction slot_function_type;
        typedef detail::OROCOS_SIGNAL_CONNECTION_N<SlotFunction> connection_impl;

        typedef R result_type;
        OROCOS_SIGNAL_ARG_TYPES

		OROCOS_SIGNAL_N()
		{
		}

        handle connect(const SlotFunction& f )
        {
            handle h = this->setup(f);
            h.connect();
            return h;
        }

        handle setup(const SlotFunction& f )
		{
			connection_t conn = 
				new connection_impl(this, f);
            //mconnections.allocator.grow( 1 );
            return handle(conn);
		}

		void emit(OROCOS_SIGNAL_PARAMS)
		{
			//lock_block<mt_policy> lock(this);

            iterator it = mconnections.begin();
            const_iterator end = mconnections.end();
            for (; it != end; ++it ) {
                connection_impl* ci = static_cast<connection_impl*>( it->get() );
                if ( ci )
                    ci->emit(OROCOS_SIGNAL_ARGS);
            }

            this->cleanup();
		}

		void operator()(OROCOS_SIGNAL_PARAMS)
		{
            this->emit(OROCOS_SIGNAL_ARGS);
		}
	};

    // TO BE REMOVED :
#if 0
	template<class arg1_type, class SlotFunction = boost::function1<void,arg1_type> >
	class signal1 : public detail::signal_base
	{
		signal1(const signal1<arg1_type, SlotFunction>& s);
	public:
        typedef SlotFunction            slot_function_type;
        typedef detail::connection1<SlotFunction> connection_impl;

		signal1()
		{
		}

        handle connect(const SlotFunction& f)
		{
            handle h = this->setup(f);
            h.connect();
            return h;
		}

        handle setup(const SlotFunction& f)
		{
			connection_t conn = 
				new connection_impl(this, f);
            //mconnections.allocator.grow( 1 );
            return handle( conn );
		}

		void emit(arg1_type a1)
		{
			//lock_block<mt_policy> lock(this);

            iterator it = mconnections.begin();
            const_iterator end = mconnections.end();
            for (; it != end; ++it ) {
                connection_impl* ci = static_cast<connection_impl*>( it->get() );
                if ( ci )
                    ci->emit(a1);
            }

            this->cleanup();
		}

		void operator()(arg1_type a1)
		{
            this->emit( a1 );
		}
	};

	template<class arg1_type, class arg2_type, class SlotFunction = boost::function1<void,arg1_type, arg2_type> >
	class signal2 : public detail::signal_base
	{
		signal2(const signal2<arg1_type, arg2_type, SlotFunction>& s);
	public:
        typedef SlotFunction            slot_function_type;
        typedef detail::connection2<SlotFunction> connection_impl;

		signal2()
		{
		}

        handle connect(const SlotFunction& f)
		{
            handle h = this->setup(f);
            h.connect();
            return h;
		}

        handle setup(const SlotFunction& f)
		{
			connection_t conn = 
				new connection_impl(this, f);
            //mconnections.allocator.grow( 1 );
            return handle( conn );
		}

		void emit(arg1_type a1, arg2_type a2)
		{
// 			lock_block<mt_policy> lock(this);

            iterator it = mconnections.begin();
            const_iterator end = mconnections.end();
            for (; it != end; ++it ) {
                connection_impl* ci = static_cast<connection_impl*>( it->get() );
                if ( ci )
                    ci->emit(a1,a2);
            }

            this->cleanup();
		}

		void operator()(arg1_type a1, arg2_type a2)
		{
            this->emit( a1, a2 );
		}
	};
#endif

} // namespace sigslot



