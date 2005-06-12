// sigslot.h: Signal/Slot classes
// 
// Written by Sarah Thompson (sarah@telergy.com) 2002.
//
// License: Public domain. You are free to use this code however you like, with the proviso that
//          the author takes on no responsibility or liability for any use.
//
// QUICK DOCUMENTATION 
//		
//				(see also the full documentation at http://sigslot.sourceforge.net/)
//
//		#define switches
//			SIGSLOT_PURE_ISO			- Define this to force ISO C++ compliance. This also disables
//										  all of the thread safety support on platforms where it is 
//										  available.
//
//			SIGSLOT_USE_POSIX_THREADS	- Force use of Posix threads when using a C++ compiler other than
//										  gcc on a platform that supports Posix threads. (When using gcc,
//										  this is the default - use SIGSLOT_PURE_ISO to disable this if 
//										  necessary)
//
//			SIGSLOT_DEFAULT_MT_POLICY	- Where thread support is enabled, this defaults to multi_threaded_global.
//										  Otherwise, the default is single_threaded. #define this yourself to
//										  override the default. In pure ISO mode, anything other than
//										  single_threaded will cause a compiler error.
//
//		PLATFORM NOTES
//
//			Win32						- On Win32, the WIN32 symbol must be #defined. Most mainstream
//										  compilers do this by default, but you may need to define it
//										  yourself if your build environment is less standard. This causes
//										  the Win32 thread support to be compiled in and used automatically.
//
//			Unix/Linux/BSD, etc.		- If you're using gcc, it is assumed that you have Posix threads
//										  available, so they are used automatically. You can override this
//										  (as under Windows) with the SIGSLOT_PURE_ISO switch. If you're using
//										  something other than gcc but still want to use Posix threads, you
//										  need to #define SIGSLOT_USE_POSIX_THREADS.
//
//			ISO C++						- If none of the supported platforms are detected, or if
//										  SIGSLOT_PURE_ISO is defined, all multithreading support is turned off,
//										  along with any code that might cause a pure ISO C++ environment to
//										  complain. Before you ask, gcc -ansi -pedantic won't compile this 
//										  library, but gcc -ansi is fine. Pedantic mode seems to throw a lot of
//										  errors that aren't really there. If you feel like investigating this,
//										  please contact the author.
//
//		
//		THREADING MODES
//
//			single_threaded				- Your program is assumed to be single threaded from the point of view
//										  of signal/slot usage (i.e. all objects using signals and slots are
//										  created and destroyed from a single thread). Behaviour if objects are
//										  destroyed concurrently is undefined (i.e. you'll get the occasional
//										  segmentation fault/memory exception).
//
//			multi_threaded_global		- Your program is assumed to be multi threaded. Objects using signals and
//										  slots can be safely created and destroyed from any thread, even when
//										  connections exist. In multi_threaded_global mode, this is achieved by a
//										  single global mutex (actually a critical section on Windows because they
//										  are faster). This option uses less OS resources, but results in more
//										  opportunities for contention, possibly resulting in more context switches
//										  than are strictly necessary.
//
//			multi_threaded_local		- Behaviour in this mode is essentially the same as multi_threaded_global,
//										  except that each signal, and each object that inherits has_slots, all 
//										  have their own mutex/critical section. In practice, this means that
//										  mutex collisions (and hence context switches) only happen if they are
//										  absolutely essential. However, on some platforms, creating a lot of 
//										  mutexes can slow down the whole OS, so use this option with care.
//
//		USING THE LIBRARY
//
//			See the full documentation at http://sigslot.sourceforge.net/
//
//

#ifndef SIGSLOT_H__
#define SIGSLOT_H__

#include <set>
#include <list>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/lambda/casts.hpp>
#include <boost/intrusive_ptr.hpp>
#include <boost/function.hpp>

#if defined(SIGSLOT_PURE_ISO) || (!defined(WIN32) && !defined(__GNUG__) && !defined(SIGSLOT_USE_POSIX_THREADS))
#	define _SIGSLOT_SINGLE_THREADED
#elif defined(WIN32)
#	define _SIGSLOT_HAS_WIN32_THREADS
#	include <windows.h>
#elif defined(__GNUG__) || defined(SIGSLOT_USE_POSIX_THREADS)
#	define _SIGSLOT_HAS_POSIX_THREADS
#	include <pthread.h>
#else
#	define _SIGSLOT_SINGLE_THREADED
#endif

#ifndef SIGSLOT_DEFAULT_MT_POLICY
#	ifdef _SIGSLOT_SINGLE_THREADED
#		define SIGSLOT_DEFAULT_MT_POLICY single_threaded
#	else
#		define SIGSLOT_DEFAULT_MT_POLICY multi_threaded_local
#	endif
#endif


namespace sigslot {

    using namespace boost;

    namespace detail {

        class single_threaded
        {
        public:
            single_threaded()
            {
                ;
            }

            virtual ~single_threaded()
            {
                ;
            }

            virtual void lock()
            {
                ;
            }

            virtual void unlock()
            {
                ;
            }
        };

#ifdef _SIGSLOT_HAS_WIN32_THREADS
        // The multi threading policies only get compiled in if they are enabled.
        class multi_threaded_global
        {
        public:
            multi_threaded_global()
            {
                static bool isinitialised = false;

                if(!isinitialised)
                    {
                        InitializeCriticalSection(get_critsec());
                        isinitialised = true;
                    }
            }

            multi_threaded_global(const multi_threaded_global&)
            {
                ;
            }

            virtual ~multi_threaded_global()
            {
                ;
            }

            virtual void lock()
            {
                EnterCriticalSection(get_critsec());
            }

            virtual void unlock()
            {
                LeaveCriticalSection(get_critsec());
            }

        private:
            CRITICAL_SECTION* get_critsec()
            {
                static CRITICAL_SECTION g_critsec;
                return &g_critsec;
            }
        };

        class multi_threaded_local
        {
        public:
            multi_threaded_local()
            {
                InitializeCriticalSection(&m_critsec);
            }

            multi_threaded_local(const multi_threaded_local&)
            {
                InitializeCriticalSection(&m_critsec);
            }

            virtual ~multi_threaded_local()
            {
                DeleteCriticalSection(&m_critsec);
            }

            virtual void lock()
            {
                EnterCriticalSection(&m_critsec);
            }

            virtual void unlock()
            {
                LeaveCriticalSection(&m_critsec);
            }

        private:
            CRITICAL_SECTION m_critsec;
        };
#endif // _SIGSLOT_HAS_WIN32_THREADS

#ifdef _SIGSLOT_HAS_POSIX_THREADS
        // The multi threading policies only get compiled in if they are enabled.
        class multi_threaded_global
        {
        public:
            multi_threaded_global()
            {
                pthread_mutex_init(get_mutex(), NULL);
            }

            multi_threaded_global(const multi_threaded_global&)
            {
                ;
            }

            virtual ~multi_threaded_global()
            {
                ;
            }

            virtual void lock()
            {
                pthread_mutex_lock(get_mutex());
            }

            virtual void unlock()
            {
                pthread_mutex_unlock(get_mutex());
            }

        private:
            pthread_mutex_t* get_mutex()
            {
                static pthread_mutex_t g_mutex;
                return &g_mutex;
            }
        };

        class multi_threaded_local
        {
        public:
            multi_threaded_local()
            {
                pthread_mutex_init(&m_mutex, NULL);
            }

            multi_threaded_local(const multi_threaded_local&)
            {
                pthread_mutex_init(&m_mutex, NULL);
            }

            virtual ~multi_threaded_local()
            {
                pthread_mutex_destroy(&m_mutex);
            }

            virtual void lock()
            {
                pthread_mutex_lock(&m_mutex);
            }

            virtual void unlock()
            {
                pthread_mutex_unlock(&m_mutex);
            }

        private:
            pthread_mutex_t m_mutex;
        };
#endif // _SIGSLOT_HAS_POSIX_THREADS

        template<class mt_policy>
        class lock_block
        {
        public:
            mt_policy *m_mutex;

            lock_block(mt_policy *mtx)
                : m_mutex(mtx)
            {
                m_mutex->lock();
            }

            ~lock_block()
            {
                m_mutex->unlock();
            }
        };

        class signal_base;

        /**
         * A connection may be connected or destroyed.
         * It is the central object which knows about one signal
         * and its slot (the latter in its derived classes).
         */
        class connection_base
        {
            friend void intrusive_ptr_add_ref(connection_base*);
            friend void intrusive_ptr_release(connection_base*);
            bool mconnected;
            signal_base* m_sig;
            int refcount;

            /**
             * Increase the reference count by one.
             */
            void ref() { ++refcount; };
            /**
             * Decrease the reference count by one and delete this on zero.
             */
            void deref() { if ( --refcount <= 0 ) delete this; };

            connection_base(const connection_base&);
        public:
            connection_base(signal_base* sig)
                : mconnected(false), m_sig(sig), refcount(0)
            {}

            typedef boost::intrusive_ptr<connection_base> shared_ptr;

            bool connected() { return mconnected && m_sig; }
            bool connect();
            bool disconnect();
            void destroy();
        };

        void intrusive_ptr_add_ref( connection_base* p ) { p->ref(); }
        void intrusive_ptr_release( connection_base* p ) { p->deref(); }

        /**
         * The base signal class which stores connection objects.
         */
        class signal_base
        {
        public:
            typedef connection_base::shared_ptr        connection_t;
            typedef std::list< connection_t >    connections_list;
            typedef connections_list::iterator iterator;
            typedef connections_list::iterator const_iterator;
        private:
            friend class connection_base;

            void conn_connect( connection_t conn ) {
                if ( !conn )
                    return; // virtually impossible ! only connection base should call this function !
                mconnections.push_back( conn );
            }

            void conn_disconnect( connection_t conn ) {
                if ( !conn )
                    return; // virtually impossible ! only connection base should call this function !
                iterator tgt;

                if ( (tgt = std::find( mconnections.begin(),
                                       mconnections.end(),
                                       conn)) != mconnections.end() ) {
                    //mconnections.erase( tgt );
                    connection_t d(0);
                    *tgt = d; // clear out, no erase, keep all iterators valid !
                }
            }

        protected:
            /**
             * Erase all empty list items.
             */
            void cleanup() {
                iterator it = mconnections.begin();
                const_iterator itend = mconnections.end();
                while( it != itend ) {
                    iterator nxt_it = it;
                    ++nxt_it;
                    if ( !(*it) )
                        mconnections.erase( it );
                    it = nxt_it;
                }
            }

            connections_list mconnections;
        public:
            virtual ~signal_base(){
                // call destroy on all connections.
                while ( !mconnections.empty() ) {
                    if ( mconnections.front() )
                        mconnections.front()->destroy(); // this calls-back conn_disconnect.
                    mconnections.erase( mconnections.begin() );
                }
            }

        };

        bool connection_base::connect() { if( !m_sig ) return false; mconnected = true; m_sig->conn_connect(this); return true; }
        bool connection_base::disconnect() { if (!m_sig) return false; mconnected = false; m_sig->conn_disconnect(this); return true; }
        void connection_base::destroy() {
            if( !m_sig ) return;
            mconnected = false;
            signal_base* copy = m_sig;
            m_sig = 0; 
            copy->conn_disconnect(this);
            // after this point this object may be destructed !
        }

        template<class SlotFunction>
        class connection0 : public connection_base
        {
        public:
            typedef SlotFunction slot_function;

            connection0(signal_base* s, const slot_function& f)
                : connection_base(s), func(f)
            {
            }

            void emit()
            {
                func();
            }
        private:
            slot_function func;
        };

        template<class SlotFunction>
        class connection1 : public connection_base
        {
        public:
            typedef SlotFunction slot_function;
            typedef typename SlotFunction::arg1_type arg1_type;

            connection1(signal_base* s, const slot_function& f)
                : connection_base(s), func(f)
            {
            }

            void emit(arg1_type a)
            {
                func(a);
            }
        private:
            slot_function func;
        };

        template<class SlotFunction>
        class connection2 : public connection_base
        {
        public:
            typedef SlotFunction slot_function;
            typedef typename SlotFunction::arg1_type arg1_type;
            typedef typename SlotFunction::arg2_type arg2_type;

            connection2(signal_base* s, const slot_function& f)
                : connection_base(s), func(f)
            {
            }

            void emit(arg1_type a1, arg2_type a2)
            {
                func(a1,a2);
            }
        private:
            slot_function func;
        };

    }
    /**
     * A copy-able connection handle of a (connected) slot and a signal.
     */
	class handle
	{
	private:
		typedef detail::signal_base           sender_t; //! the signal type
        typedef sender_t::connection_t connection_t; //! the connection type for the slot of this handle
	public:
        /**
         * Create an empty handle.
         */
		handle()
            : m_conn(0)
		{
			;
		}

        handle(connection_t conn)
            : m_conn(conn)
        {
        }

        /**
         * Create a copy-equivalent handle.
         */
        handle(const handle& hs)
            : m_conn( hs.m_conn )
		{
		} 

        /**
         * No-op destructor, does not change signal/slot state.
         */
		~handle()
		{
		}

        /**
         * (Re-)Connect the slot with the signal.
         * @retval true the slot is connected.
         * @retval false no valid signal or slot in this handle
         */
		bool connect()
		{
            if ( connected() )
                return true;
            return m_conn->connect();
		}

        /**
         * Disconnect the slot from the signal.
         * @retval true the slot is disconnected.
         * @retval false no valid signal or slot in this handle
         */
		bool disconnect()
		{
            if ( !connected() )
                return true;
            return m_conn->disconnect();
		}

        /** 
         * Inspect if this handle represents a connected signal and slot.
         * 
         * @return true if a connection is present.
         */
        bool connected() const {
            return m_conn->connected();
        }
	protected:
        /**
         * This is actually a smart pointer which always
         * points to an existing connection object.
         */
        connection_t  m_conn;
	};

    /**
     * A scoped connection handle of a (connected) slot which
     * disconnects a slot from a signal in its destructor.
     */
	class scoped_handle
        :public handle
	{
		scoped_handle();
    public:

        scoped_handle(const handle& hs)
            : handle( hs )
		{
		} 


        /**
         * If connected, disconnect the slot from the signal.
         */
		~scoped_handle()
		{
            this->disconnect();
		}
    };

    /**
     * A connection handle of a (connected) slot which
     * disconnects and cleans up (free all resources)
     * a slot from a signal in its destructor.
     */
	class cleanup_handle
        :public handle
	{
		cleanup_handle();
    public:

        cleanup_handle(const handle& hs)
            : handle( hs )
		{
		} 

        /**
         * Cleanup all signal and slot connection resources.
         * If connected, disconnect the slot from the signal.
         */
		~cleanup_handle()
		{
            m_conn->destroy();
		}
    };


	template<class SlotFunction = boost::function0<void> >
	class signal0 : public detail::signal_base
	{
		signal0(const signal0<SlotFunction>& s);
	public:
        typedef SlotFunction slot_function_type;
        typedef detail::connection0<SlotFunction> connection_impl;

		signal0()
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
            return handle(conn);
		}

		void emit()
		{
			//lock_block<mt_policy> lock(this);

            iterator it = mconnections.begin();
            const_iterator end = mconnections.end();
            for (; it != end; ++it ) {
                connection_impl* ci = static_cast<connection_impl*>( it->get() );
                if ( ci )
                    ci->emit();
            }

            this->cleanup();
		}

		void operator()()
		{
            this->emit();
		}
	};

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
			//lock_block<mt_policy> lock(this);
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
// 			lock_block<mt_policy> lock(this);
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

    namespace detail {
      template<int Arity,
               typename Signature,
               typename SlotFunction>
      class real_get_signal_impl;

      template<typename Signature,
               typename SlotFunction>
      class real_get_signal_impl<0, Signature,
                                 SlotFunction>
      {
        typedef function_traits<Signature> traits;

      public:
        typedef signal0<SlotFunction> type;
      };

      template<typename Signature,
               typename SlotFunction>
      class real_get_signal_impl<1, Signature,
                                 SlotFunction>
      {
        typedef function_traits<Signature> traits;

      public:
        typedef signal1<typename traits::arg1_type,
                        SlotFunction> type;
      };

      template<typename Signature,
               typename SlotFunction>
      class real_get_signal_impl<2, Signature,
                                 SlotFunction>
      {
        typedef function_traits<Signature> traits;

      public:
        typedef signal2<typename traits::arg1_type,
                        typename traits::arg2_type,
                        SlotFunction> type;
      };

#if 0
      template<typename Signature,
               typename SlotFunction>
      class real_get_signal_impl<3, Signature,
                                 SlotFunction>
      {
        typedef function_traits<Signature> traits;

      public:
        typedef signal3<typename traits::arg1_type,
                        typename traits::arg2_type,
                        typename traits::arg3_type,
                        SlotFunction> type;
      };

      template<typename Signature,
               typename SlotFunction>
      class real_get_signal_impl<4, Signature,
                                 SlotFunction>
      {
        typedef function_traits<Signature> traits;

      public:
        typedef signal4<typename traits::arg1_type,
                        typename traits::arg2_type,
                        typename traits::arg3_type,
                        typename traits::arg4_type,
                        SlotFunction> type;
      };

      template<typename Signature,
               typename SlotFunction>
      class real_get_signal_impl<5, Signature,
                                 SlotFunction>
      {
        typedef function_traits<Signature> traits;

      public:
        typedef signal5<typename traits::arg1_type,
                        typename traits::arg2_type,
                        typename traits::arg3_type,
                        typename traits::arg4_type,
                        typename traits::arg5_type,
                        SlotFunction> type;
      };

      template<typename Signature,
               typename SlotFunction>
      class real_get_signal_impl<6, Signature,
                                 SlotFunction>
      {
        typedef function_traits<Signature> traits;

      public:
        typedef signal6<typename traits::arg1_type,
                        typename traits::arg2_type,
                        typename traits::arg3_type,
                        typename traits::arg4_type,
                        typename traits::arg5_type,
                        typename traits::arg6_type,
                        SlotFunction> type;
      };

      template<typename Signature,
               typename SlotFunction>
      class real_get_signal_impl<7, Signature,
                                 SlotFunction>
      {
        typedef function_traits<Signature> traits;

      public:
        typedef signal7<typename traits::arg1_type,
                        typename traits::arg2_type,
                        typename traits::arg3_type,
                        typename traits::arg4_type,
                        typename traits::arg5_type,
                        typename traits::arg6_type,
                        typename traits::arg7_type,
                        SlotFunction> type;
      };

      template<typename Signature,
               typename SlotFunction>
      class real_get_signal_impl<8, Signature,
                                 SlotFunction>
      {
        typedef function_traits<Signature> traits;

      public:
        typedef signal8<typename traits::arg1_type,
                        typename traits::arg2_type,
                        typename traits::arg3_type,
                        typename traits::arg4_type,
                        typename traits::arg5_type,
                        typename traits::arg6_type,
                        typename traits::arg7_type,
                        typename traits::arg8_type,
                        SlotFunction> type;
      };

      template<typename Signature,
               typename SlotFunction>
      class real_get_signal_impl<9, Signature,
                                 SlotFunction>
      {
        typedef function_traits<Signature> traits;

      public:
        typedef signal9<typename traits::arg1_type,
                        typename traits::arg2_type,
                        typename traits::arg3_type,
                        typename traits::arg4_type,
                        typename traits::arg5_type,
                        typename traits::arg6_type,
                        typename traits::arg7_type,
                        typename traits::arg8_type,
                        typename traits::arg9_type,
                        SlotFunction> type;
      };

      template<typename Signature,
               typename SlotFunction>
      class real_get_signal_impl<10, Signature,
                                 SlotFunction>
      {
        typedef function_traits<Signature> traits;

      public:
        typedef signal10<typename traits::arg1_type,
                         typename traits::arg2_type,
                         typename traits::arg3_type,
                         typename traits::arg4_type,
                         typename traits::arg5_type,
                         typename traits::arg6_type,
                         typename traits::arg7_type,
                         typename traits::arg8_type,
                         typename traits::arg9_type,
                         typename traits::arg10_type,
                         SlotFunction> type;
      };
#endif

      template<typename Signature,
               typename SlotFunction>
      struct get_signal_impl :
        public real_get_signal_impl<(function_traits<Signature>::arity),
                                    Signature,
                                    SlotFunction>
      {
      };

    } // end namespace detail

    /**
     * Very lightweight wrapper around the signalN classes that allows signals to
     * be created where the number of arguments does not need to be part of the
     * class name.
     */
    template<
        typename Signature, // function type R (T1, T2, ..., TN)
        typename SlotFunction = boost::function<Signature>
    >
    class signal :
        public detail::get_signal_impl<Signature,
                                       SlotFunction>::type
    {
        typedef typename detail::get_signal_impl< Signature,
                                                  SlotFunction>::type base_type;
        
    public:
        signal() {}
    };
    
} // namespace sigslot


#endif // SIGSLOT_H__

