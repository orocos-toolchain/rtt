/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:19 CET 2004  StandardEventListener.hpp 

                        StandardEventListener.hpp -  description
                           -------------------
    begin                : Mon January 19 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 
 
#ifndef STANDARDEVENTLISTENER_HPP
#define STANDARDEVENTLISTENER_HPP

#include "EventListenerInterface.hpp"

#include <boost/function.hpp>
#include <boost/bind.hpp>

namespace ORO_CoreLib
{
	/**
	 * This is a wrapper class for holding a
	 * pointer to a function and calling that function
	 * on handleEvent().
	 * Do not use this class directly, but use the listener()
	 * function.
	 *
	 * @see listener
	 */
	class StandardEventListener
		: public EventListenerInterface
		{
			typedef boost::function<void (CallbackInterface*)> fun_t;
			fun_t fun;
			public:
			StandardEventListener( fun_t f )
				: fun( f )
				{
				}

			void handleEvent( CallbackInterface* cb )
			{
				fun( cb );
			}
		};
	
	/**
	 * This is a wrapper class for holding a
	 * pointer to a function and calling that function
	 * on handleEvent().
	 * Do not use this class directly, but use the listener()
	 * function.
	 *
	 * @see listener
	 */
	class SimpleEventListener
		: public EventListenerInterface
		{
			typedef boost::function<void (void)> fun_t;
			fun_t fun;
			public:
			SimpleEventListener( fun_t f )
				: fun( f )
				{
				}

			void handleEvent( CallbackInterface* )
			{
				fun();
			}
		};


  /**
   * Factory method. 
   * Creates a new StandardEventListener object.
   *
   * @return A new StandardEventListener instance, which is
   *         owned by the caller.
   *
   * @note The reason for returning a new instance on the
   * heap (instead of stack) is because the lifetime of the
   * object must be longer than the current scope.
   */
  template<typename t>
  StandardEventListener* standlistener( t fun )
  {
    return new StandardEventListener( fun );
  }

  /**
   * Factory method.
   * Creates a new EventListenerInterface object which
   * will execute a given member function.
   *
   * @param f The member function to execute, with CallbackInterface
   * @param t The object holding the member function.
   * @return A new EventListenerInterface instance, which is
   *         owned by the caller.
   *
   */
  template<typename T>
  EventListenerInterface* listener( void (T::*f)(CallbackInterface* ), T* t )
  {
    return standlistener( boost::bind( f, t ) );
  }

  /**
   * Factory method. 
   * Creates a new SimpleEventListener object.
   *
   * @return A new SimpleEventListener instance, which is
   *         owned by the caller.
   *
   * @note The reason for returning a new instance on the
   * heap (instead of stack) is because the lifetime of the
   * object must be longer than the current scope.
   */
  template<typename t>
  SimpleEventListener* simpllistener( t fun )
  {
    return new SimpleEventListener( fun );
  }

  /**
   * Factory method.
   * Creates a new EventListenerInterface object which
   * will execute a given member function.
   *
   * @param f The member function to execute, without CallbackInterface
   * @param t The object holding the member function.
   * @return A new EventListenerInterface instance, which is
   *         owned by the caller.
   *
   */
  template<typename T>
  EventListenerInterface* listener( void (T::*f)( void ), T* t )
  {
    return simpllistener( boost::bind( f, t ) );
  }

  /**
   * @example An example how to use this class.
   *
  class PseudoComponent
  {
    void handleFirstEvent( CallbackInterface* cb )
      {
        // do stuff..
      }
    void handleSecondEvent( CallbackInterface* cb )
      {
        // do different stuff..
      }

    PseudoComponent::PseudoComponent()
      {
        EventListenerInterface* firstlistener =
          listener( &PseudoComponent::handleFirstEvent, this );
        EventListenerInterface* secondlistener =
          listener( &PseudoComponent::handleSecondEvent, this );
      }
  };
   */
}

#endif
