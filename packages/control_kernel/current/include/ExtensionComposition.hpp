

#ifndef EXTENSIONCOMPOSITION_HPP
#define EXTENSIONCOMPOSITION_HPP

#include "TypeManip.h"
#include "KernelInterfaces.hpp"

namespace ORO_ControlKernel
{

    namespace detail {
            // Helper class
            struct AspectRejector {
                template<class T>
                bool enableAspect( T ext ) {
                    return true; // Oh irony, we reject with 'true'
                }
                template<class T>
                void disableAspect( T ext ) {
                }
            };
    }
                

    /**
     * @brief A class for composing Aspects (if you want more than
     * one Aspect in your component).
     * 
     */
    template<class First, class Second>
    struct CompositeAspect
        : public First, public Second, private virtual detail::AspectRejector
    {
        CompositeAspect( const std::string& _name ) 
            :First(_name), Second(_name) {}

        virtual ~CompositeAspect() {}

        typedef typename First::Extension FirstExtension;
        typedef typename Second::Extension SecondExtension;

        // postcondition : Every kernel is of type KernelBaseFunction
        // neat trick to lure a 'composite composite' aspect
        // into thinking that our Extension is a subclass
        // of the 'enableAspect Kernel'. This makes sure
        // that we are called and that we can pass 
        // enableAspect on to First and Second.
        typedef KernelBaseFunction Extension; 

        // this removes compiler warnings, about
        // enableAspect being hidden, we want them
        // to be hidden, but hey, at least we lost the
        // warning...and this also works.
        using First::enableAspect;
        using Second::enableAspect;

        /**
         * @brief Enable the sub-aspects of this CompositeAspect.
         * @param _Extension The type of the ControlKernel extensions.
         */
        template< class _Extension >
        bool enableAspect(_Extension* ext) 
        {
            // Type Selection: If 'Extension' (a composite extension)
            // is derived from First, call
            // the enableAspect of First's Extension, else, reject it.
            // Ditto for Second
            typedef typename Loki::Select<SUPERSUBCLASS(FirstExtension, _Extension), First, AspectRejector>::Result  FirstResult;
            typedef typename Loki::Select<SUPERSUBCLASS(SecondExtension, _Extension), Second, AspectRejector>::Result SecondResult;
            // Enable first aspect, if successful, enable second,
            // if also successful, return true, otherwise, disable first
            // aspect (~undo) and return false.
            return (FirstResult::enableAspect(ext) &&
                    ( SecondResult::enableAspect(ext) || (FirstResult::disableAspect(), false) ) );
        }

        void disableAspect() 
        {
            // TODO: we could work analogous to enableAspect
            First::disableAspect();
            Second::disableAspect();
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
         * The Aspects of Both Extensions are composed 
         * automatically in the same First,Second way.
         */
        typedef CompositeAspect<typename First::CommonBase,typename Second::CommonBase> CommonBase;
            
        CompositeExtension() {}
        CompositeExtension( KernelBaseFunction* _base ) : First(_base), Second(_base) {}

        virtual ~CompositeExtension() {}

        virtual bool initialize() 
        {
            return (First::initialize() &&
                    (Second::initialize() || (First::finalize(),false) ) );
        }

        virtual void step() 
        {
            First::step();
            Second::step();
        }

        virtual void finalize() 
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
        CompositeExtension( KernelBaseFunction* _base ) : First(_base){}
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
     * @brief A helper class to construct a composite Aspect.
     *
     * Use MakeAspect<Extension1, Extension2, Extension3,...>::CommonBase to get a
     * CompositeAspect of these three _Extensions_.
     * It is thus a equivalent to MakeExtension<...>::CommonBase and is 
     * introduced for semantic convenience.
     */
    template
    < typename T1  = nil_ext, typename T2  = nil_ext, typename T3  = nil_ext,
      typename T4  = nil_ext, typename T5  = nil_ext, typename T6  = nil_ext >
    struct MakeAspect
    {
        typedef typename MakeExtension<T1,T2,T3,T4,T5,T6>::CommonBase Result;
        typedef Result CommonBase;
    };
}

#endif
