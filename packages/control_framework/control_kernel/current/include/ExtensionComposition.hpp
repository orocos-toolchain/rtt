/***************************************************************************
  tag: Peter Soetens  Thu Jul 15 11:21:24 CEST 2004  ExtensionComposition.hpp 

                        ExtensionComposition.hpp -  description
                           -------------------
    begin                : Thu July 15 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens at mech.kuleuven.ac.be
 
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


#ifndef EXTENSIONCOMPOSITION_HPP
#define EXTENSIONCOMPOSITION_HPP

#include "KernelInterfaces.hpp"
#include <boost/type_traits.hpp>
#include <boost/mpl/if.hpp>

namespace ORO_ControlKernel
{

    namespace detail {
            // Helper class
            struct FacetRejector {
                template<class T>
                bool enableFacet( T ext ) {
                    return true; // Oh irony, we reject with 'true'
                }
                void disableFacet( ) {
                }
                void component(ComponentBaseInterface*) {}
            };
    }
                

    /**
     * @brief A class for composing Facets (if you want more than
     * one Facet in your component).
     * 
     */
    template<class First, class Second>
    struct CompositeFacet
        : public First, public Second, private virtual detail::FacetRejector
    {
        CompositeFacet( const std::string& _name ) 
            :First(_name), Second(_name) {}

        virtual ~CompositeFacet() {}

        typedef typename First::Extension FirstExtension;
        typedef typename Second::Extension SecondExtension;

        // postcondition : Every kernel is of type ControlKernelInterface
        // neat trick to lure a 'composite composite' Facet
        // into thinking that our Extension is a subclass
        // of the 'enableFacet Kernel'. This makes sure
        // that we are called and that we can pass 
        // enableFacet on to First and Second.
        typedef ControlKernelInterface Extension; 

        // this removes compiler warnings, about
        // enableFacet being hidden, we want them
        // to be hidden, but hey, at least we lost the
        // warning...and this also works.
        using First::enableFacet;
        using Second::enableFacet;

        /**
         * @brief Enable the sub-Facets of this CompositeFacet.
         * @param _Extension The type of the ControlKernel extensions.
         */
        template< class _Extension >
        bool enableFacet(_Extension* ext) 
        {
            // Type Selection: If 'Extension' (a composite extension)
            // is derived from First, call
            // the enableFacet of First's Extension, else, reject it.
            // Ditto for Second
            typedef typename boost::mpl::if_<boost::is_base_and_derived<FirstExtension, _Extension>, First, FacetRejector>::type  FirstResult;
            typedef typename boost::mpl::if_<boost::is_base_and_derived<SecondExtension, _Extension>, Second, FacetRejector>::type SecondResult;
            // Enable first Facet, if successful, enable second,
            // if also successful, return true, otherwise, disable first
            // Facet (~undo) and return false.
            return (FirstResult::enableFacet(ext) &&
                    ( SecondResult::enableFacet(ext) || (FirstResult::disableFacet(), false) ) );
        }

        void disableFacet() 
        {
            // TODO: we could work analogous to enableFacet
            First::disableFacet();
            Second::disableFacet();
        }

        /**
         * This method is called during component construction
         * to inform all facets of the ComponentBaseInterface.
         */
        void component(ComponentBaseInterface* cbi) {
            First::component(cbi);
            Second::component(cbi);
        }
    };
    
    /**
     * @brief A class for composing extensions (if you want more than
     * one extension in your kernel). 
     *
     * Composing allows the user to specify order of initialisation,
     * execution and cleanup of multiple Extensions in one kernel.
     * 
     * @param First The first Extension to be started and executed
     * relatively to the Second.
     * @param Second The second Extension to be started and executed
     * relatively to the First.
     */
    template<class First, class Second>
    struct CompositeExtension
        : public First, public Second
    {
        /**
         * The Facets of Both Extensions are composed 
         * automatically in the same First,Second way.
         */
        typedef CompositeFacet<typename First::CommonBase,typename Second::CommonBase> CommonBase;
            
        CompositeExtension() {}
        CompositeExtension( ControlKernelInterface* _base ) : First(_base), Second(_base) {}

        virtual ~CompositeExtension() {}

        bool initialize() 
        {
            return (First::initialize() &&
                    (Second::initialize() || (First::finalize(),false) ) );
        }

        void step() 
        {
            First::step();
            Second::step();
        }

        void finalize() 
        {
            First::finalize();
            Second::finalize();
        }
    };

    struct nil_ext {};

    template<class First>
    struct CompositeExtension< First, nil_ext>
        : public First
    {
        typedef typename First::CommonBase CommonBase;
        CompositeExtension() {}
        CompositeExtension( ControlKernelInterface* _base ) : First(_base){}
        virtual ~CompositeExtension() {}
    };

    /**
     * @brief A helper class to construct a composite Extension.
     *
     * Use MakeExtension<E1, E2, E3,...>::Extension to get a
     * CompositeExtension of these three Extensions.
     */
    template
    < typename T1  = nil_ext, typename T2  = nil_ext, typename T3  = nil_ext,
      typename T4  = nil_ext, typename T5  = nil_ext, typename T6  = nil_ext >
    struct MakeExtension
    {
    private:
        typedef typename MakeExtension
        <
            T2 , T3 , T4 , 
            T5 , T6
            >
        ::Result TailResult;
            
    public:
        typedef CompositeExtension<T1, TailResult> Result;
        typedef Result Extension;
        typedef typename Result::CommonBase CommonBase;
    };

    template<>
    struct MakeExtension<>
    {
        typedef nil_ext Result;
    };

    /**
     * @brief A helper class to construct a composite Facet.
     *
     * Use MakeFacet<Extension1, Extension2, Extension3,...>::CommonBase to get a
     * CompositeFacet of these three _Extensions_.
     * It is thus a equivalent to MakeExtension<...>::CommonBase and is 
     * introduced for semantic convenience.
     */
    template
    < typename T1  = nil_ext, typename T2  = nil_ext, typename T3  = nil_ext,
      typename T4  = nil_ext, typename T5  = nil_ext, typename T6  = nil_ext >
    struct MakeFacet
    {
        typedef typename MakeExtension<T1,T2,T3,T4,T5,T6>::CommonBase Result;
        typedef Result CommonBase;
    };
}

#endif
