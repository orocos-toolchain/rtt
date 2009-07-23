#ifndef IRQ_ACTIVITY_HPP
#define IRQ_ACTIVITY_HPP

#include "NonPeriodicActivity.hpp"
#include "rtt-config.h"

#ifdef OROPKG_OS_XENOMAI
#include <native/intr.h>
#else
// Provide a dump RT_INTR type. start() always returns false on non-Xenomai
// OSes.
struct RT_INTR { };
#warning "IRQActivity is available only on Xenomai. It will fail on startup"
#endif

namespace RTT {
    /** An activity which is triggered by an IRQ edge. This activity is only
     * available on xenomai
     *
     * The IRQ line can be given by two means:
     * <ul>
     *   <li> setIRQ is called
     *   <li> one of the underlying task contexts (if any) provides the
     *   IRQActivity::Provider interface. In that case, the getIRQ() method of
     *   that task context is called to get the descriptor
     * </ul>
     */
    class IRQActivity : public NonPeriodicActivity
    {
        int      m_irq;
        RT_INTR  m_handle;
        RunnableInterface* m_runner;

    public:
        /** If this interface is derived from one of the TaskContext objects
         * which are executed by this activity, then the activity can
         * auto-configure itself (i.e. it can get the file descriptor from
         * that task context). Otherwise, setIRQ must be called
         * explicitely
         */
        struct Provider {
            virtual int getIRQ() const = 0;
        };

        /**
         * Create a IRQActivity with a given priority and RunnableInterface
         * instance. The default scheduler for IRQActivity
         * objects is ORO_SCHED_RT.
         *
         * @param priority The priority of the underlying thread.
         * @param _r The optional runner, if none, this->loop() is called.
         */
        IRQActivity(int priority, RunnableInterface* _r = 0 );

        /**
         * Create a IRQActivity with a given scheduler type, priority and
         * RunnableInterface instance.
         * @param scheduler
         *        The scheduler in which the activitie's thread must run. Use ORO_SCHED_OTHER or
         *        ORO_SCHED_RT.
         * @param priority The priority of the underlying thread.
         * @param _r The optional runner, if none, this->loop() is called.
         */
        IRQActivity(int scheduler, int priority, RunnableInterface* _r = 0 );

        /**
         * Create a IRQActivity with a given priority, name and
         * RunnableInterface instance.
         * @param priority The priority of the underlying thread.
         * @param name The name of the underlying thread.
         * @param _r The optional runner, if none, this->loop() is called.
         */
        IRQActivity(int priority, const std::string& name, RunnableInterface* _r = 0 );

        virtual ~IRQActivity();

        int  getIRQ() const;
        void setIRQ(int irq);

        virtual bool start();
#ifdef OROPKG_OS_XENOMAI
        virtual void loop();
        virtual bool breakLoop();
        virtual void step();
#endif
    };
}

#endif
