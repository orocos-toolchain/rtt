/***************************************************************************
  tag: Peter Soetens  Mon Jan 19 14:11:26 CET 2004  CompositeComponent.hpp 

                        CompositeComponent.hpp -  description
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
 
 
#ifndef COMPOSITE_COMPONENT_HPP
#define COMPOSITE_COMPONENT_HPP

namespace ORO_ControlKernel
{
            
    template<class Base>
    class CompositeGenerator
    :public Base
    {
        typedef typename Base::ModelType ModelType;
        typedef typename Base::CommandType CommandType;
        typedef typename Base::InputType InputType;
        typedef typename Base::SetPointType SetPointType;
                                            
        public:
            virtual void update()
            {
                for (typename Subs::iterator it= subs.begin(); it != subs.end(); ++it)
                   (*it)->update();
            }

            virtual void pull()
            {
                for (typename Subs::iterator it= subs.begin(); it != subs.end(); ++it)
                   (*it)->pull();
            }

            virtual void calculate()
            {
                for (typename Subs::iterator it= subs.begin(); it != subs.end(); ++it)
                   (*it)->calculate();
            }

            virtual void push()
            {
                for (typename Subs::iterator it= subs.begin(); it != subs.end(); ++it)
                   (*it)->push();
            }

            virtual void readFrom(const InputType* data)
            {
                for (typename Subs::iterator it= subs.begin(); it != subs.end(); ++it)
                   (*it)->readFrom(data);
                Base::readFrom(data);
            }
                
            virtual void readFrom(const ModelType* data)
            {
                for (typename Subs::iterator it= subs.begin(); it != subs.end(); ++it)
                   (*it)->readFrom(data);
                Base::readFrom(data);
            }
                
            virtual void readFrom(const CommandType* data)
            {
                for (typename Subs::iterator it= subs.begin(); it != subs.end(); ++it)
                   (*it)->readFrom(data);
                Base::readFrom(data);
            }

            virtual void writeTo(SetPointType* data)
            {
                for (typename Subs::iterator it= subs.begin(); it != subs.end(); ++it)
                   (*it)->writeTo(data);
                Base::writeTo(data);
            }
                
                
            /**
             * Add a sub component to this component.
             */
            bool addSubComponent(Base* s)
            {
                if ( inKernel() ) return false;
                /*
                if ( inKernel() )
                    if ( kernel()->enable(s) )
                    {
                        subs.push_back(s);
                        return true;
                    } else
                        return false;
                        */
                subs.push_back(s);
                return true;
            }

            /**
             * Remove a sub component from this component.
             */
            bool removeSubComponent(Base* s)
            {
                typename Subs::iterator it = find(subs.begin(), subs.end(), s);
                if (it != subs.end())
                {
                    if ( inKernel() ) return false;
                    /*
                    if ( inKernel() )
                        if ( !kernel()->disable(s) )
                        {
                            subs.erase(it);
                            return true;
                        } else
                            return false;
                        subs.erase(it);
                        */
                        return true;
                }
            }

            template<class Extension>
            void enableAspect( Extension* e)
            {
                for (typename Subs::iterator it= subs.begin(); it != subs.end(); ++it)
                   (*it)->enableAspect(e);
            }

            void disableAspect()
            {
                for (typename Subs::iterator it= subs.begin(); it != subs.end(); ++it)
                   (*it)->disableAspect();
            }

        private:
            typedef vector<Base*> Subs;
            /**
             * A vector of all sub components.
             */
            Subs subs;
    };

    /**
     * A class for composing multiple components of the
     * same type to be used simultaneously in a kernel.
     */
    template<class Base>
    class CompositeComponent
    :public Base
    {
        public:
            virtual void update()
            {
                for (typename Subs::iterator it= subs.begin(); it != subs.end(); ++it)
                   (*it)->update();
            }

            virtual void pull()
            {
                for (typename Subs::iterator it= subs.begin(); it != subs.end(); ++it)
                   (*it)->pull();
            }

            virtual void calculate()
            {
                for (typename Subs::iterator it= subs.begin(); it != subs.end(); ++it)
                   (*it)->calculate();
            }

            virtual void push()
            {
                for (typename Subs::iterator it= subs.begin(); it != subs.end(); ++it)
                   (*it)->push();
            }

            /**
             * Add a sub component to this component.
             */
            bool addSubComponent(Base* s)
            {
                /*
                if ( inKernel() )
                    if ( kernel()->enable(s) )
                    {
                        subs.push_back(s);
                        return true;
                    } else
                        return false;
                        */
                subs.push_back(s);
                return true;
            }

            /**
             * Remove a sub component from this component.
             */
            bool removeSubComponent(Base* s)
            {
                typename Subs::iterator it = find(subs.begin(), subs.end(), s);
                if (it != subs.end())
                {
                    /*
                    if ( inKernel() )
                        if ( !kernel()->disable(s) )
                        {
                            subs.erase(it);
                            return true;
                        } else
                            return false;
                            */
                        subs.erase(it);
                        return true;
                }
            }

            template<class Extension>
            void enableAspect( Extension* e)
            {
                for (typename Subs::iterator it= subs.begin(); it != subs.end(); ++it)
                   (*it)->enableAspect(e);
            }

            void disableAspect()
            {
                for (typename Subs::iterator it= subs.begin(); it != subs.end(); ++it)
                   (*it)->disableAspect();
            }

        private:
            typedef vector<Base*> Subs;
            /**
             * A vector of all sub components.
             */
            Subs subs;
    };

}

#endif
