^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package rtt
^^^^^^^^^^^^^^^^^^^^^^^^^

Toolchain 2.10
==============

Important Caveats
-----------------

* For the `gnulinux` target, periodic components, timers or waiting on
  a condition variable are not affected by system clock adjustments anymore
  (e.g. due to NTP). Therefore the timestamp returned by
  ``RTT::os::TimeService::getNSecs()`` is also retrieved
  from a monotonic clock. Before, this method returned the real/wall time
  (as nanoseconds since the Unix epoch, 1 January 1970, 00:00:00 UTC).
  Only use the returned time for relative comparisons or for
  `RTT::os::Condition::wait_until(m, abs_time)`. See PR `#258 <https://github.com/orocos-toolchain/rtt/pull/258>`_.

2.10.0 (Forthcoming) - WORK IN PROGRESS
---------------------------------------

New
~~~

* Allow loading of global service plugins via the CORBA interface of each TaskContextProxy (`#228 <https://github.com/orocos-toolchain/rtt/pull/228>`_)

  Follow-up of `#134 <https://github.com/orocos-toolchain/rtt/pull/134>`_, which reverted a patch introduced in
  `#90 <https://github.com/orocos-toolchain/rtt/pull/90>`_ for loading global service plugins into an Orocos process from remote peers,
  which is required for the RTT introspection feature.

  Each `TaskContextProxy` now has a new virtual operation ``bool loadPlugin(const std::string& pluginPath)`` that loads a global
  plugin in the process that owns the respective server (by forwarding the call to the ``RTT::plugin::PluginLoader::Instance()``).

Changes
~~~~~~~

* Install headers to include/orocos/rtt instead of include/rtt  (`#243 <https://github.com/orocos-toolchain/rtt/issues/243>`_)
* Always try to read from the same channel in MultipleInputsChannelElement (follow-up of `#114 <https://github.com/orocos-toolchain/rtt/issues/114>`_)

  Except for buffer policy ``PerInputPort`` every input port has to select a channel from the
  list of connections for each read operation. Before this commit, the channel selection
  strategy depended on the type of buffer policy and each ``MultipleInputsChannelElement``
  or ``MultipleOutputsChannelElement`` only allowed to set the buffer policy once. Or, in
  other words, a single port could not have multiple connections with different buffer
  policies.

  This commit drops the ``buffer_policy`` member variable and changes the
  `MultipleInputsChannelElement` reader selection to apply the "start with the
  same channel that had new data the last time" unconditionally, which before was only
  selected for buffer policies ```PerConnection`` and ``PerOutputPort``. So the only case where
  this makes a difference is for input ports with multiple connections of type ``Shared``.
  The ``SharedConnection<T>`` instance itself, which is also a ``MultipleInputsChannelElement``,
  overrides the read method and is not influenced by the channel selection either.

  The channel selection strategy that always reads from the last signaled/written channel
  introduced in `afe68a8 <https://github.com/orocos-toolchain/rtt/commit/afe68a89ea8fb5d4c631411cf2af56573610573a>`_ was only effective in the case of
  an input port with multiple ``Shared`` connections.

  On the positive side some restrictions on mixed buffer policies for connections to the
  same port have been relaxed and a significant amount of code could be removed from the
  channel element implementation.

* corba: manage links to Service instances in weak pointers (`#238 <https://github.com/orocos-toolchain/rtt/issues/238>`_)

  The CORBA layer should not delay the destruction of Service instances once the owning TaskContext gets destroyed.
  Accessing a destroyed service instance through CORBA would result in a bad_weak_ptr exception now which is handled
  by the CORBA implementation, but does not cause a segfault.

* Use a lock-free buffer for remote logging instead of an ``std::stringstream`` instance (`#221 <https://github.com/orocos-toolchain/rtt/issues/221>`_)
* os: reimplemented SharedMutex supporting platform-independent lock-free read locks (follow-up on `#119 <https://github.com/orocos-toolchain/rtt/issues/119>`_)
* internal: required a typed ChannelElement<T> to construct a SharedConnection (`#114 <https://github.com/orocos-toolchain/rtt/issues/114>`_)

Fixes
~~~~~

Toolchain 2.9
=============

The Orocos Toolchain v2.9 release series mainly improved on the
correct execution of the Component updateHook() and allowing
extended configuration options when connecting Data Flow ports.

Important Caveats
-----------------

* The Orocos CMake macro orocos_use_package() does not longer
  automatically add the package to the CMake include
  directories. It is still picked up by the orocos_component()
  and related macros, just no longer by other targets built with
  the standard cmake commands. Most users should not notice a
  difference. This minor change was required to fix include
  directory ordering issues when rebuilding a package without
  a proper cleanup of the installation folder. For details, see
  `#85 <https://github.com/orocos-toolchain/rtt/pull/85>`_.

* updateHook() will now only be executed when an 'user' triggered
  event has happened, and no longer when internal bookkeeping
  of the ExeuctionEngine happens. For full details, see PR
  `#91 <https://github.com/orocos-toolchain/rtt/pull/91>`_.
  The motivation of this change was an older issue which reported
  that updateHook() was called too many times, and in unpredictable
  ways for the average user. The calling of updateHook() is now
  fully determined and under control of the user.

* RTT::base::ActivityInterface got a new pure virtual member
  function bool timeout() which you need to implement in case
  you created your own Activity implementation. See
  `#53 <https://github.com/orocos/rtt_ros_integration/pull/53>`_ for
  an example of a solution.

* OCL XML deployments treats a ConnPolicy XML Property with
  the name "Default" as a special case. The values of the
  "Default" ConnPolicy will be used for each unspecified field
  in each subsequently created ConnPolicy in the current process.
  This also influences ConnPolicy defaults in the C++ code paths
  that have nothing to do with the XML deployment. It was introduced
  to change at run-time the default data flow configuration,
  which was introduced in 2.9, and still defaults to 2.8 semantics.

Improvements
------------

* updateHook() will now only be executed when an 'user' triggered
  event has happened, and no longer when internal bookkeeping
  of the ExeuctionEngine happens. For full detail, see PR
  `#91 <https://github.com/orocos-toolchain/rtt/pull/91>`_.
  Yes, it's also a major improvement.

* The RTT scripting re-added the Orocos v1 'Command', by emulating
  it when an Operation is called with the '.cmd()' suffix. See PR
  `#84 <https://github.com/orocos-toolchain/rtt/pull/84>`_.

* The RTT Data Flow implementation has been rewritten, in a fully
  backwards compatible way. It however adds powerful alternative
  connection semantics which are often required in control
  applications. For all details, see PR `#114 <https://github.com/orocos-toolchain/rtt/pull/114>`_.
  The robustness and flexibility of the Orocos Data Flow
  has improved tremendously in this release and should hold for the
  next years.
  It addresses all known data flow architecture issues for
  intra- and inter-process communication. User can choose to
  opt-in on the newly available connection policies, piecewise
  or change the process-wide default (see OCL XML deployments
  below as well). There is a broad motivation text linked by
  the above PR, but one of the major motivators was to have
  much better control and predictability over the sample-by-
  sample dataflow going on between RTT components.

* The initial connection between RTT components over CORBA has been speed-up
  significantly due to the improvement of the introspection+discovery. In some
  cases non-blocking oneway calls are used to signal or write to remote ports.
  The main motivation of this patch is to enable using the CORBA transport on
  slow or long latency links.

  See `#123 <https://github.com/orocos-toolchain/rtt/pull/123>`_ for all details.

* ``RTT::extras::FileDescriptorSimulationActivity`` allows to simulate
  file descriptor activities in unit tests. This is however incomplete
  and will be completed in RTT 2.9 when the updateHook() updates have been
  merged.

Other API changes
-----------------

* The method `RTT::Property<T>::copy()` introduced in version 2.7
  to fix a memory leak in class `PropertyBag` has been removed in
  favor of an overload of `RTT::Property<T>::create()` that accepts
  a data source. See `#159 <https://github.com/orocos-toolchain/rtt/pull/159>`_.


2.9.1 (2019-05-15)
------------------

New
~~~

* add inException operation (`#265 <https://github.com/orocos-toolchain/rtt/issues/265>`_)

Changes
~~~~~~~

* Change component loader behaviour to consider a component found on first valid path  (`#288 <https://github.com/orocos-toolchain/rtt/pull/288>`_)
* Fix Activity default wait period policy and behavior of ORO_WAIT_REL (`Intermodalics/rtt#2 <https://github.com/Intermodalics/rtt/pull/2>`_)

  Fixes a regression bug in `#91 <https://github.com/orocos-toolchain/rtt/issues/91>`_ as merged into 2.9.0 that effectively changed the default
  wait period policy from ``ORO_WAIT_ABS`` to ``ORO_WAIT_REL`` and also changed the meaning of ``ORO_WAIT_REL``.
  This patch restores the previous behavior of ``ORO_WAIT_REL``: The next wakeup time is equal to the actual wakeup time, incremented by one period.
  In version 2.9.0 the next wakeup time was always set to the last nominal wakeup time plus a multiple of a period. Unless in the case of overruns
  this was equivalent to ``ORO_WAIT_ABS``.

* Follow-up on CORBA efficiency patches (`#123 <https://github.com/orocos-toolchain/rtt/pull/123>`_):

  * Avoid some CORBA calls improving getArgumentList() (`meyerj/rtt#8 <https://github.com/meyerj/rtt/pull/8>`_)
  * Strip qualifiers from the name returned by CorbaOperationCallerFactory::getArgumentType()
  * Fixed segfault in CDataFlowInterface_i::buildChannelInput() if the remote side has no CORBA transport for that type
  * Improve log output in case of unsufficient type info during the synchronization of component interfaces
  * Fix crash retrieving property name through corba

* Corba aliases: revert API changes and fixed ComponentLoader::unloadComponent() for aliased components (`#66 <https://github.com/orocos-toolchain/rtt/pull/66>`_, `#230 <https://github.com/orocos-toolchain/rtt/pull/230>`_, `#277 <https://github.com/orocos-toolchain/rtt/pull/277>`_)

  `#66 <https://github.com/orocos-toolchain/rtt/pull/66>`_ introduced aliases for CORBA component servers and added an optional
  name argument to ``ComponentLoader::unloadComponent(tc)`` in `bf92dcb <https://github.com/orocos-toolchain/rtt/commit/bf92dcb3483f89ccb436502e1f74a04361e53c74>`_. This change has been
  reverted, because it could lead to inconsistencies in the internal component map, if a TaskContext is destroyed based on the
  given name but with an unrelated instance pointer.

  `#277 <https://github.com/orocos-toolchain/rtt/pull/277>`_ reverts part of these changes to keep version 2.9.1 ABI-compatible
  with 2.9.0. But the new argument has no effect anymore.

* cmake: unset found and missing components list after each invocation of the RTT cmake config (`#220 <https://github.com/orocos-toolchain/rtt/pull/220>`_)
* scripting: accept semicolon after an if statement without curly brackets before the else clause (part of `#156 <https://github.com/orocos-toolchain/rtt/pull/156>`_)
* types: minor optimization of TypeInfo::convert(arg) for the common case that nothing needs to be done (`730a5eb <https://github.com/orocos-toolchain/rtt/commit/730a5eba800e05127a3e0d1dbed68a6b4dcb35c7>`_)
* Add TypeInfo retrieval by alias name (`#239 <https://github.com/orocos-toolchain/rtt/pull/239>`_)

Fixes
~~~~~

* Fix copyright notices in header files (`#264 <https://github.com/orocos-toolchain/rtt/pull/264>`_)
* Fixed InputPort<T>::getDataSample() by introducing a copy_sample flag in DataObjectInterface<T>::Get(...) (fix `#231 <https://github.com/orocos-toolchain/rtt/issues/231>`_, part of `#117 <https://github.com/orocos-toolchain/rtt/issues/117>`_)

  Follow-up:
  * buffers: add DataObjectInterface::data_sample() method and use call_traits (`588987c <https://github.com/orocos-toolchain/rtt/commit/588987c9f0b1d9ee92679b3049d43fda152e2970>`_)

  `#277 <https://github.com/orocos-toolchain/rtt/pull/277>`_ reverts part of these changes to keep version 2.9.1 ABI-compatible
  with 2.9.0.

* Prevent changing the activity from a component's own thread (`#271 <https://github.com/orocos-toolchain/rtt/issues/271>`_)
* Fix null ptr exception in RTT::TaskContext::setActivity() if neither ORO_ACT_DEFAULT_SEQUENTIAL nor ORO_ACT_DEFAULT_ACTIVITY compile-time option is defined (`#217 <https://github.com/orocos-toolchain/rtt/issues/217>`_)
* tests: fixed number of workers in buffers_test/testListLockFree (`#278 <https://github.com/orocos-toolchain/rtt/issues/278>`_)
* Fix namespace of some definitions and exclude unrelated namespaces from API documentation (`#241 <https://github.com/orocos-toolchain/rtt/issues/241>`_)
* tests: fixed some newer unit tests for the xenomai target (Xenomai 2) (`#267 <https://github.com/orocos-toolchain/rtt/issues/267>`_)
* Another attempt to fix flaky unit tests at Travis (`#266 <https://github.com/orocos-toolchain/rtt/issues/266>`_)
* Fix compilation error with boost 1.64 (`#274 <https://github.com/orocos-toolchain/rtt/issues/274>`_)
* Update README.md, made a link to the official Installation Guide for 2.9 (`#271 <https://github.com/orocos-toolchain/rtt/issues/271>`_)
* Fix collectIfDone() for more than three arguments (`#261 <https://github.com/orocos-toolchain/rtt/issues/261>`_)
* tests: removed special case for high-priority real-time threads in testAffinity (`#260 <https://github.com/orocos-toolchain/rtt/issues/260>`_)
* fix compilation on win32 (`#240 <https://github.com/orocos-toolchain/rtt/issues/240>`_)
* os/tlsf: removed empty and unused CMakelists.txt (`815533a <https://github.com/orocos-toolchain/rtt/commit/815533a7972223259cbb51f1f4a4e7cd083c9942>`_)
* Removed even more traces of ROS support directly in RTT (`dd19338 <https://github.com/orocos-toolchain/rtt/commit/dd193389e2e733126b5b55762a2d77c8d56df9db>`_)
* base: fixed copy&paste error in documentation of ``RTT::base::ChannelElementBase::getOutputEndPoint()`` (`eaefbcb <https://github.com/orocos-toolchain/rtt/commit/eaefbcb2a81e2d88751a0e3175a8436e8d378b87>`_)
* engine: do not evaluate predicate twice in each cycle of waitAndProcessMessages() (part of `#91 <https://github.com/orocos-toolchain/rtt/issues/91>`_)
* Fix timeout handling and disconnection of mqueue channels: (part of `#114 <https://github.com/orocos-toolchain/rtt/pull/114>`_)

  * Revert "mqueue: block until the initial sample has been received in mqReady()"
  * mqueue: read with timeout from message queues
  * Revert "mqueue: make reading from message queues non-blocking to prevent lockups during connection cleanup"

* corba: forward result of remote RemoteChannelElement::channelReady() invocation to the caller (part of `#114 <https://github.com/orocos-toolchain/rtt/pull/114>`_)
* tests: do not expect zero dropped samples in multi-threaded lock-free buffers test with more than one writer (part of `#114 <https://github.com/orocos-toolchain/rtt/pull/114>`_)
* tests: make taskstates_test more robust against slow execution (`#242 <https://github.com/orocos-toolchain/rtt/issues/242>`_)
* tests: printout average (per thread) performance for each test case in dataflow_performance_test (part of `#250 <https://github.com/orocos-toolchain/rtt/issues/250>`_)
* tests: added guard to detect concurrent assignments to the dataflow_performance_test (part of `#250 <https://github.com/orocos-toolchain/rtt/issues/250>`_)
* tests: fixed dataflow_performance_test when compiled for RTT versions <2.9 (part of `#250 <https://github.com/orocos-toolchain/rtt/issues/250>`_)
* tests: fixed dropped count in buffers_test and added a test case for writing to a DataObjectLockFree<T> with multiple threads concurrently (part of `#250 <https://github.com/orocos-toolchain/rtt/issues/250>`_)
* Avoid unused parameter compiler warning in base/ChannelElementBase.hpp (part of `#250 <https://github.com/orocos-toolchain/rtt/issues/250>`_)
* base: avoid costly call to size() when pushing to circular buffers (part of `#250 <https://github.com/orocos-toolchain/rtt/issues/250>`_)
* tests: replace BOOST_ASSERT by BOOST_REQUIRE (`afe081a <https://github.com/orocos-toolchain/rtt/commit/afe081acf4b9532d76820806de8dc75c3186b8a7>`_)
* Removed faulty assertion in MultipleOutputsChannelElement<T> and forward buffer_policy to base class in SharedConnection<T> constructor (part of `#114 <https://github.com/orocos-toolchain/rtt/issues/114>`_)
* rm some unnecessary Logger::In (`#221 <https://github.com/orocos-toolchain/rtt/issues/221>`_ up to `a8148a9 <https://github.com/orocos-toolchain/rtt/commit/a8148a9f0ddbab2bc9188ef9405de74f03f66b53>`_)
* add aarch64 support (`#219 <https://github.com/orocos-toolchain/rtt/pull/219>`_)
* doc: Update Doxyfile.in to version 1.8.11 and enable EXTRACT_ALL and HTML_TIMESTAMP options (`b634441 <https://github.com/orocos-toolchain/rtt/commit/b6344416a45887f61af37061b29045e5a1d3cc3d>`_)
* tests: avoid compiler warnings (`d681b68 <https://github.com/orocos-toolchain/rtt/commit/d681b681ee308a9582f135e3dfd0cc1d9f4d071e>`_)
* marsh: fixed missing lines in merge resolution resulting from `#208 <https://github.com/orocos-toolchain/rtt/pull/208>`_ in combination with `#281 <https://github.com/orocos-toolchain/rtt/pull/281>`_
* tests: removed invalid TaskContext inheritance in corba_ipc_test.cpp (`#236 <https://github.com/orocos-toolchain/rtt/pull/236>`_)
* use another variable name for CMAKE_PREFIX_PATH entries (`#225 <https://github.com/orocos-toolchain/rtt/pull/225>`_, `#237 <https://github.com/orocos-toolchain/rtt/pull/237>`_)
* add null pointer check in ServiceRequester::connectTo() (`c700886 <https://github.com/orocos-toolchain/rtt/commit/c700886061c6c86fd99cc212e9b035ab405db5bd>`_)
* tests: fixed execution thread spec in fixture for the state_test (`#84 <https://github.com/orocos-toolchain/rtt/issues/84>`_)
* tests: add a test case to program_test for send and collect of scripting functions (`#84 <https://github.com/orocos-toolchain/rtt/issues/84>`_)
* tlsf: fixed compiler warnings related to C99 compliance (`35f1437 <https://github.com/orocos-toolchain/rtt/commit/35f14379112d74bdbe2bc1aa98c990ab2edb074e>`_)
* Fixed bug in component unloading (`#232 <https://github.com/orocos-toolchain/rtt/issues/232>`_)
* Fix some compiler warnings in the CORBA plugin (`#229 <https://github.com/orocos-toolchain/rtt/issues/229>`_)
* Fixed missing removal of USE_CPP11 macro check from merge resolution (`9da2c35 <https://github.com/orocos-toolchain/rtt/commit/9da2c351c9dd995add12e0ee62250147e2446d4c>`_, `#201 <https://github.com/orocos-toolchain/rtt/pull/201>`_)
* tests: unified ASSERT_PORT_SIGNALLING and wait_for_* macros (`#123 <https://github.com/orocos-toolchain/rtt/pull/123>`_)
* tests: disable some checks depending on plugin options (`#123 <https://github.com/orocos-toolchain/rtt/pull/123>`_)
* activity: fixed missing initializers for member variables introduced in `#91 <https://github.com/orocos-toolchain/rtt/pull/91>`_ for the constructor introduced in `#74 <https://github.com/orocos-toolchain/rtt/pull/74>`_ (`#91 <https://github.com/orocos-toolchain/rtt/pull/91>`_)
* TimerThread: fix comparison of CPU affinity when deciding whether a new thread needs to be created (`#215 <https://github.com/orocos-toolchain/rtt/pull/215>`_)
* os/gnulinux: relax log level of failures to set the POSIX thread name to Warning (`e297928 <https://github.com/orocos-toolchain/rtt/commit/e297928ea174b3a5bb11fed177902c0a9aa5ced5>`_, follow-up of `#176 <https://github.com/orocos-toolchain/rtt/pull/176>`_)
* Avoid unneeded locking in ```RTT::Logger`` (`#203 <https://github.com/orocos-toolchain/rtt/pull/203>`_)
* Fixed memory leak in ``RTT::updateProperties()`` and ``RTT::marsh::CPFMarshaller::deserialize()`` (`#208 <https://github.com/orocos-toolchain/rtt/pull/208>`_)

2.9.0 (2017-04-28)
------------------

New
~~~

* New dataflow semantics with shared input and output buffers (`#117 <https://github.com/orocos-toolchain/rtt/issues/117>`_, `#114 <https://github.com/orocos-toolchain/rtt/issues/114>`_ up to `eea55c7 <https://github.com/orocos-toolchain/rtt/commit/eea55c71986dd0e85156315b5c91a4e384599dac>`_)

  Adds new types of port connections with different semantics.
  The main addition is the support of shared input and/ or output buffers, where either multiple output ports write to the same input
  buffer, multiple input ports read from the same output buffer, or both. The latter case is basically reinstalling the connection
  semantics of RTT v1, where a connection was not much more than a shared data object or buffer.

  For details, see https://docs.google.com/document/d/1zDnPPz4SiCVvfEFxYFUZBcVbXKj33o4KaotpCtXM4E0/pub.

* Added new mutex class SharedMutex which implements a shared lock ownership mode (`#119 <https://github.com/orocos-toolchain/rtt/issues/119>`_ up to `62428f0 <https://github.com/orocos-toolchain/rtt/commit/62428f0fd7383ae1668aae273c6b5687a0a09d92>`_)

  The new mutex type implements a shared ownership lock that allows concurrent access to shared data by multiple readers and exclusive
  access for writers. This lock type is used to protect the input and output pointers lists in the new ChannelElementBase
  implementations.

  This first implementation is using mutexes internally and thus breaks the lock-freedom of LOCK_FREE data or buffer connections
  *with multiple readers or writers*. The implementation was improved afterwards and will be released in version 2.10 (breaks ABI).

* Add the RTT v1 cmd syntax (`#84 <https://github.com/orocos-toolchain/rtt/issues/84>`_ up to `361fe29 <https://github.com/orocos-toolchain/rtt/commit/361fe2909aaeb81b7cee83600b44c62e5b22aa7a>`_)

  This adds the RTT v1 command semantics to operations used in RTT scripts, by introducing a ``.cmd()`` method, analog to .send()
  and ``.call()``. An operation invoked with ``.cmd()`` will wait in a non-blocking way for the operation to complete. So this can
  mainly be used in periodically executed components, which call operations which take a long time to execute in non-periodic
  components.

* Corba aliases (`#66 <https://github.com/orocos-toolchain/rtt/issues/66>`_ up to `bf92dcb <https://github.com/orocos-toolchain/rtt/commit/bf92dcb3483f89ccb436502e1f74a04361e53c74>`_)

  This PR allows to create CORBA component servers that instead of using their own name, use an alias.
  This is handy if you have X similar deployments in which you do not want to do component renaming for various reasons but you
  still want to remotely connect to multiple of them. In this case registering them under an alias allows you to create remote
  proxies that remotely will be known under their alias name.

* TLSF memory corruption detection (`#63 <https://github.com/orocos-toolchain/rtt/issues/63>`_, `#77 <https://github.com/orocos-toolchain/rtt/issues/77>`_)

  This PR adds memory corruption detection to the tlsf code, I needed this to detect a double free of tlsf memory,
  which cannot be detected by valgrind for instance. It currently aborts if it detects a tlsf memory corruption since
  once the tlsf memory is corrupted your code that uses tlsf can segfault at any time.

* Added method setDataSource() and assignment operator to the PropertyBase interface (`#158 <https://github.com/orocos-toolchain/rtt/pull/158>`_)
* Implement CORBA disconnect(port) (`#142 <https://github.com/orocos-toolchain/rtt/pull/142>`_, `#194 <https://github.com/orocos-toolchain/rtt/pull/194>`_)
* Added option, to load typekits on demand (`#162 <https://github.com/orocos-toolchain/rtt/pull/162>`_)
* Basic connection tracing support and counting of dropped samples for buffer connections (`#90 <https://github.com/orocos-toolchain/rtt/pull/90>`_, `#130 <https://github.com/orocos-toolchain/rtt/pull/130>`_, `#131 <https://github.com/orocos-toolchain/rtt/pull/131>`_, `#134 <https://github.com/orocos-toolchain/rtt/pull/134>`_)
* Added typekit, property, scripting and transport support for signed and unsigned ``long long`` types (`#281 <https://github.com/orocos-toolchain/rtt/pull/281>`_)
* Added function to check if a port is already connected to another given port (`9092849 <https://github.com/orocos-toolchain/rtt/commit/9092849ecf96cd3029a4f51ddd5dd33a11ccd317>`_)
* Added macro RTT_VERSION_GTE(major,minor,patch) to rtt-config.h (`#115 <https://github.com/orocos-toolchain/rtt/issues/115>`_)
* New sister class ``FileDescriptorSimulationActivity`` and common base ``FileDescriptorActivityInterface``
  of ``FileDescriptorActivity`` to enable stepping of I/O-driven activities in unit tests (`#109 <https://github.com/orocos-toolchain/rtt/pull/109>`_)
* Introduce ``RTT_UNUSED`` macro and use it consistently to suppress compiler warnings on unused code elements (`6a4a469 <https://github.com/orocos-toolchain/rtt/commit/6a4a469100d42526cdfc9c96a924aedeb6f2824e>`_)

Changes
~~~~~~~

* Refactored execution engine and activity implementations such that the updateHook() is not triggered too often (`#91 <https://github.com/orocos-toolchain/rtt/issues/91>`_ up to `521f076 <https://github.com/orocos-toolchain/rtt/commit/521f0763f2fbf8a5dc171f9657c181ab75413927>`_)

  In previous RTT releases ``updateHook()`` is called too many times (even according to specifications), especially when ``OwnThread`` operations are implemented OR called(!).
  This was due to internal bookkeeping of the ``ExecutionEngine`` to dispatch the asynchronous requests. Users found it confusing that ``updateHook()`` was called during the
  use of operations. This also kills performance of Lua state machines sitting in ``updateHook()``, since they are evaluated too many times.

  Installing a callback for an event port caused calling that callback AND ``updateHook()``, instead of calling only the callback.
  The callback does not replace the call to ``updateHook()``. This is also related to users being surprised that an event port callback is only called
  periodically in periodic threads, while they thought it would be called immediately upon each event reception, and eventually ``updateHook()``
  periodically (but serialized with the callbacks).

  With this patch, TaskContext execution has been split into a callback step (OwnThread operations and event port callbacks) and an update step
  (additionally runs pending scripting functions or state machines and the ```updateHook()``).

  Furthermore it is possible to disable the triggering of non-periodic components on start by setting the ``TriggerOnStart`` attribute to ``false``.
  The old behavior (and new default behavior) was to run a full update step once after the component started.

  For details, see the description of `#91 <https://github.com/orocos-toolchain/rtt/issues/91>`_.

* CORBA efficiency patches (`#123 <https://github.com/orocos-toolchain/rtt/pull/123>`_, `#151 <https://github.com/orocos-toolchain/rtt/pull/151>`_, `#169 <https://github.com/orocos-toolchain/rtt/issues/169>`_)

  Most changes in the CORBA transport affect the way a TaskContextProxy synchronizes its objects
  (ports, operations, properties, services etc.) with the remote server. The number of CORBA calls
  going over the network has been greatly reduced. In fact, the proxy requests one big
  TaskContextDescription object in a single call now instead of enumerating all task objects subsequently.

  Furthermore remoteSignal() calls for port connections via a CORBA transport have
  been converted to oneway calls, which means that the writer (the CORBA dispatcher of the writing
  TaskContext) is not blocking anymore (see also `#122 <https://github.com/orocos-toolchain/rtt/issues/122>`_).
  The signalling can be disabled completely if appropriate.

* operations: improve OperationCallerInterface::isSend() for master-slave calls (`#126 <https://github.com/orocos-toolchain/rtt/issues/126>`_)

  When calling an OwnThread operation, RTT needs to check whether the engine that is supposed to execute it and indirectly the thread that
  operates on that engine is the same as the caller's thread. With this patch we check directly whether the calling thread is the same as
  the executor, relaxing the requirements on setting the caller engine correctly. Especially for slave components the behavior of operation
  calls depends on which thread actually updates the slave component. The old behavior caused dead-locks almost certainly when calling
  operations across different slaves of the same master or from master to slaves or vice-versa.

* UseOROCOS-RTT.cmake: several improvements and fixes to mitigate issues when rebuilding packages, for overlays or for handling RPATH settings

  * Configuration of Orocos include directories and definitions per target instead of per directory (`#85 <https://github.com/orocos-toolchain/rtt/pull/85>`_)
  * Check the real path of the current and project source dir for automatic Orocos package name derivation (`#145 <https://github.com/orocos-toolchain/rtt/pull/145>`_)
  * Added optional macro argument ``EXPORT`` and export cmake target names for all target types (`#144 <https://github.com/orocos-toolchain/rtt/pull/144>`_)
  * Fix RPATH issues when linking to ocl or 3rd-party libraries (`#157 <https://github.com/orocos-toolchain/rtt/pull/157>`_)
  * Fixed documentation (`078caf8 <https://github.com/orocos-toolchain/rtt/commit/078caf8a29aa46b38a4cbbd670feced1a9b2c30e>`_)

* gnulinux: use CLOCK_MONOTONIC for periodic tasks (`#105 <https://github.com/orocos-toolchain/rtt/pull/105>`_, `#138 <https://github.com/orocos-toolchain/rtt/pull/138>`_)

  **Important:** The patch is not effective anymore for periodic components in combination with `#91 <https://github.com/orocos-toolchain/rtt/issues/91>`_ in 2.9.0.
  This has been fixed in RTT version 2.10.0, which uses a monotonic clock source consistently for timers, periodic activities and waiting for condition variables
  (`#258 <https://github.com/orocos-toolchain/rtt/pull/258>`_).

  However, the patch does have an effect when using a periodic ``Activity`` directly, for other use cases than running a ``TaskContext``.

* env-hooks: install env-hook to etc/orocos/profile.d (`#160 <https://github.com/orocos-toolchain/rtt/pull/160>`_, `e75a22c <https://github.com/orocos-toolchain/rtt/commit/e75a22c5abe6c318a3c364bfb474ee3cd7ce73d9>`_)

  This shell script for RTT sets the default ``OROCOS_TARGET`` and the ``RTT_COMPONENT_PATH`` environment variables.

  The concept of env-hooks is borrowed from `catkin <http://docs.ros.org/api/catkin/html/user_guide/environment.html>`_.
  Each package in the workspace can install its own env-hooks and a central per-workspace script (either catkin-generated `setup.sh`
  or the `setup.sh <https://github.com/meyerj/orocos_toolchain/blob/installation-script/setup.sh>`_ script in the
  `orocos_toolchain <https://github.com/orocos-toolchain/orocos_toolchain.git>`_ meta repository) sources all env-hooks in the install-space.

  The installed location of the env-hook has been updated in a later commit (`e75a22c <https://github.com/orocos-toolchain/rtt/commit/e75a22c5abe6c318a3c364bfb474ee3cd7ce73d9>`_).

* Upgraded TLSF to version 2.46 and added debugging helpers (`#214 <https://github.com/orocos-toolchain/rtt/pull/214>`_)
* Remove ``PropertyBase::copy()`` method and allow to construct or create a property with a custom data source (`#159 <https://github.com/orocos-toolchain/rtt/pull/159>`_)
* Improved naming of timer threads (`#213 <https://github.com/orocos-toolchain/rtt/pull/213>`_)
* mqueue: ignore EINTR in select() (`#184 <https://github.com/orocos-toolchain/rtt/pull/184>`_)
* mqueue: enforce MQ name convention (`#181 <https://github.com/orocos-toolchain/rtt/pull/181>`_)
* corba: removed special cases that depend on the data source type in CorbaTemplateProtocol<T> (`#174 <https://github.com/orocos-toolchain/rtt/pull/174>`_)
* corba: added template arguments Alloc and Compare for RTT::corba::AnyConversion specializations for vectors and maps (`#173 <https://github.com/orocos-toolchain/rtt/pull/173>`_)

Fixes
~~~~~

* Support for older or newer Boost versions:

  * Eliminate boost lambda outside scripting (`#201 <https://github.com/orocos-toolchain/rtt/issues/201>`_, reverts `#103 <https://github.com/orocos-toolchain/rtt/issues/103>`_)
  * Fix backwards compatibility with older CMake and Boost versions (tested with Boost 1.40) (`#199 <https://github.com/orocos-toolchain/rtt/pull/199>`_)
  * Boost 1.61 support (`#192 <https://github.com/orocos-toolchain/rtt/pull/192>`_)
  * Fixed OperationInterfacePartFused for -std=c++11 builds with Boost 1.58 (`#196 <https://github.com/orocos-toolchain/rtt/pull/196>`_, `#197 <https://github.com/orocos-toolchain/rtt/pull/197>`_)
  * tests: replaced deprecated BOOST_MESSAGE() macro call with BOOST_TEST_MESSAGE() in property_test.cpp (`#189 <https://github.com/orocos-toolchain/rtt/pull/189>`_)
  * Fixed BOOST_GLOBAL_FIXTURE() macro call for Boost >=1.60 and removed extra semicolons (`#189 <https://github.com/orocos-toolchain/rtt/pull/189>`_)
  * Renamed boost::serialization::array<T> class name for Boost version 1.62 and higher (`8765918 <https://github.com/orocos-toolchain/rtt/commit/87659188c71bf3013247590a386321648dd8fb67>`_)

* rtt Logger: clear any fail and eof flags (`#209 <https://github.com/orocos-toolchain/rtt/pull/209>`_)
* Fix dangling children after service destruction (`#204 <https://github.com/orocos-toolchain/rtt/pull/204>`_)
* Fix use after free during disconnection in ``RTT::internal::ConnectionManager`` (`#211 <https://github.com/orocos-toolchain/rtt/pull/211>`_)
* tests: fixed dev_test for gcc 5 (`#202 <https://github.com/orocos-toolchain/rtt/pull/202>`_)
* Replaced all occurences of deprecated auto_ptr by unique_ptr (`#198 <https://github.com/orocos-toolchain/rtt/pull/198>`_)
* cmake: fixed version printout and added variable RTT_SOVERSION (`#172 <https://github.com/orocos-toolchain/rtt/pull/172>`_, `163d3e5 <https://github.com/orocos-toolchain/rtt/commit/163d3e5e5d64d8d822a740ead88634631ca83ffb>`_)
* plugin: do not log during destruction of the PluginLoader singleton as this might trigger the creation of a new Logger instance during static destruction (`#185 <https://github.com/orocos-toolchain/rtt/pull/185>`_)
* logger: Ensure that the notification of real-time logging being enabled is actually logged (`#190 <https://github.com/orocos-toolchain/rtt/pull/190>`_)
* tests: fixed timeout counter expectations in FileDescriptorActivity tests (`#186 <https://github.com/orocos-toolchain/rtt/pull/186>`_)
* fix race conditions between stop() and the runtime error state (`#182 <https://github.com/orocos-toolchain/rtt/pull/182>`_)
* internal: avoid copying ChannelDescriptor tuples while reading ports in RTT::internal::ConnectionManager (`#180 <https://github.com/orocos-toolchain/rtt/pull/180>`_)
* rtt/os/tlsf: replaced variadic macros with C99-compliant __VA_ARGS__ variant (`1419056 <https://github.com/orocos-toolchain/rtt/commit/141905655e5cf61585d3729dea55441da9fb34de>`_)
* macosx: Add static casts to slience C++11 narrowing errors (`#188 <https://github.com/orocos-toolchain/rtt/pull/188>`_)
* Fix compatibility with MacOSX 10.12 which provides clock_gettime() now (`cea4935 <https://github.com/orocos-toolchain/rtt/commit/cea4935db93def53dfc29c06f72278209f444027>`_)
* corba: expose the CircularBuffer connection type on CORBA (`#183 <https://github.com/orocos-toolchain/rtt/pull/183>`_)
* Compatibility with GCC 5 and 6 (`#154 <https://github.com/orocos-toolchain/rtt/pull/154>`_, `#191 <https://github.com/orocos-toolchain/rtt/pull/191>`_)
* Added missing virtual destructors (`aecaca9 <https://github.com/orocos-toolchain/rtt/commit/aecaca921946b78cb1af84e15d122d8f044b6014>`_, `0a7890e <https://github.com/orocos-toolchain/rtt/commit/0a7890ede913f9f01d8de263f66b886a182c111d>`_)
* scripting: fixed memory leak during destruction of ``CallFunction`` and ``CmdFunction`` instances (`9f79e55 <https://github.com/orocos-toolchain/rtt/commit/9f79e5585e479203f3bdc5a7005b21759edd91ff>`_)
* Removed traces of ROS support directly in RTT (`#152 <https://github.com/orocos-toolchain/rtt/pull/152>`_)
* pluginloader: fixed segfault if a service plugin refuses to create a service instance to be loaded into the global service (`#139 <https://github.com/orocos-toolchain/rtt/pull/139>`_)
* tests: fixed race conditions in CORBA ipc tests and increase port signalling timeout in corba_mqueue_ipc_test (`#251 <https://github.com/orocos-toolchain/rtt/pull/251>`_)
* tests: fixed corba_ipc_test and corba_mqueue_ipc_test on slow machines if the server thread is not executed fast enough (`#251 <https://github.com/orocos-toolchain/rtt/pull/251>`_)
* tests: fixed max_threads parameterization of buffer instances in the BuffersDataFlowTestSuite test suite (`ae48d02 <https://github.com/orocos-toolchain/rtt/commit/ae48d02861ad51a488c0d46b9abb330a8565eabe>`_)
* tests: refactored atomic queue tests in buffers_test using the ThreadPool class (`75745d6 <https://github.com/orocos-toolchain/rtt/commit/75745d6aec03a4fe6cc1a11bfa4362001296fc19>`_)
* scripting: do not lock the execution mutex in StateMachine::execute() if the state machine is not active (`7106983 <https://github.com/orocos-toolchain/rtt/commit/7106983f79d12304d588e8e8a3fbb0b40d3e6bdc>`_)
* tests: fixed OperationCaller signature in state_test (`#137 <https://github.com/orocos-toolchain/rtt/pull/137>`_)
* scripting: removed additional enableEvents() call in StateMachine::activate() (`#137 <https://github.com/orocos-toolchain/rtt/pull/137>`_)
* tests: moved operation caller checks from OperationCallerComponent constructor to ready() member function (`#137 <https://github.com/orocos-toolchain/rtt/pull/137>`_)
* scripting: Fix the case where entry is preempted by an event (`#137 <https://github.com/orocos-toolchain/rtt/pull/137>`_)
* os: fixed invalid read of freed thread name in os::Thread destructor (`#132 <https://github.com/orocos-toolchain/rtt/pull/132>`_)
* Removed outdated ``debian`` folder (`#121 <https://github.com/orocos-toolchain/rtt/issues/121>`_, `#133 <https://github.com/orocos-toolchain/rtt/issues/133>`_)
* config: renamed log4cpp library to orocos-log4cpp (see `orocos-toolchain/log4cpp#4 <https://github.com/orocos-toolchain/log4cpp/issues/4>`_)
* typekit: make rt_string and string constructors thread-safe by returning a temporary object (`#126 <https://github.com/orocos-toolchain/rtt/issues/126>`_)
* signals: fix multi-signal emission in case of fused functor data source callbacks (`#126 <https://github.com/orocos-toolchain/rtt/issues/126>`_)
* extras: Disable logging on timeout of FileDescriptorActivity (`#126 <https://github.com/orocos-toolchain/rtt/issues/126>`_)
* Fix mqueue transport and corba ipc tests (`#147 <https://github.com/orocos-toolchain/rtt/issues/147>`_)
* Added test suite to test connecting and disconnecting ports concurrently (`#120 <https://github.com/orocos-toolchain/rtt/issues/120>`_)
* Fixed compilation errors and warnings if compiling with clang (part of `#114 <https://github.com/orocos-toolchain/rtt/issues/114>`_)
* Fixed FlowStatus race condition in ChannelDataElement and fixed BufferLockFree implementation for the circular buffer case (`#117 <https://github.com/orocos-toolchain/rtt/issues/117>`_ up to 62d5462)
* logger: use const reference for templated streaming operator (`#116 <https://github.com/orocos-toolchain/rtt/issues/116>`_)

Toolchain 2.8
=============

The Orocos Toolchain v2.8 release series mainly improved on the
execution of various activities and control of the threads in RTT.


Important Caveats
-----------------

* ``RTT::SendStatus`` now also has a ``CollectFailure`` enum value
  (without changing the existing enum integer values).

* There were changes to the RTT StateMachine execution flow
  that may influence existing state machine scripts in case
  they are using the event operations introduced in v2.7.0.
  These changes were required because the event operation
  transition programs could execute asynchronously with respect
  to the State Machine.

Improvements
------------

* Better support for executing ``RTT::extras::SlaveActivity``, especially
  for calling Operations, where the Operation is executed by the master
  component and not by the slave component in order to avoid deadlocks.

* RTT allows to replace ``boost::bind`` with C++11 ``std::bind``, but only
  when compiling RTT. This needs more work in next releases.

* Orocos-RTT CMake macros added DESTDIR support.

* ``RTT::Activity`` got an extra constructor for running non periodic
  RunnableInterfaces in a given scheduler+priority setting.

* There was another round of improvements to ``RTT::extras::FileDescriptorActivity``
  in order to work correctly in combination with ``RTT::extras::SlaveActivity``.

* ``RTT::Timer`` class has been cleaned up for correctness in corner cases
  and the ``waitFor()`` methods have been implemented.

* An RTT cmake flag has been added to not emit the CORBA IOR to cerr and file
  when the CORBA transport does not find the naming service.

2.8.3 (2016-09-08)
------------------

Changes
~~~~~~~

* cmake: updated exact version check semantics for find_package(Orocos-RTT) (`56b671c <https://github.com/orocos-toolchain/rtt/commit/56b671c0804aa7e855bc16646ac7a25b68d59e04>`_)

  Every version that has the same major version number and at least the
  requested minor version number is considered as compatible. Nothing changed here.

  If an exact version of RTT is requested, the script also accepts it if the patch version
  or the minor version was not specified. If they are, they have to match.

  This patch allows to release RTT and OCL independently, with different patch versions.

* Skip task/timer updates if late for more than a few periods (`#129 <https://github.com/orocos-toolchain/rtt/issues/129>`_)

  This patch mitigates problems due to the fact that RTT uses the system clock for timers and periodic tasks. In case the system time
  jumps due to manual or automatic time adjustments timers and threads might starve for a long time or execute in a busy-loop until
  time has caught up. RTT skips pending cycles now if the loop missed its deadline by more than four periods.

  Note that the patch is not effective anymore for periodic components in combination with
  `#91 <https://github.com/orocos-toolchain/rtt/issues/91>`_ in 2.9.0. It has been reverted for RTT version 2.10.0, which uses a
  monotonic clock source consistently for timers, periodic activities and waiting for condition variables (`#258 <https://github.com/orocos-toolchain/rtt/pull/258>`_).

* Set thread name to match activity name on linux (`#128 <https://github.com/orocos-toolchain/rtt/issues/128>`_)

Fixes
~~~~~

* Do not abort if pthread_setname_np fails (`#176 <https://github.com/orocos-toolchain/rtt/issues/176>`_)
* tests: make slave_test more robust (fix occassional errors on Travis) (`f31d83d <https://github.com/orocos-toolchain/rtt/commit/f31d83d37379dcbad10867825d72b0d1b8903d4b>`_)
* Fixed typekit for bool sequences (`#146 <https://github.com/orocos-toolchain/rtt/issues/146>`_)
* Fixed build when sequential activity is used by default (`#143 <https://github.com/orocos-toolchain/rtt/pull/143>`_)
* tests: fixed missing character in testThreadName (`ed57b20 <https://github.com/orocos-toolchain/rtt/commit/ed57b20cfd2c9584c4b217ca5cf83b13fc642df8>`_)
* Timer: fixed wake up time on 32 bit systems (`#127 <https://github.com/orocos-toolchain/rtt/issues/127>`_)
* .travis.yml: removed ROS bootstrapping from Travis config and switch to trusty (`cd3c19e <https://github.com/orocos-toolchain/rtt/commit/cd3c19e41a2b9fdff130ec869e57a29b95b07061>`_)
* Fixed potential double deallocation and pool corruption for circular BufferLockFree (`#148 <https://github.com/orocos-toolchain/rtt/pull/148>`_)
* tests: fixed CORBA ipc tests and made corba tests resilient against future changes in CConnPolicy struct (`27a5d62 <https://github.com/orocos-toolchain/rtt/commit/27a5d62da0f64119a1415c0421cc7983b16bf893>`_)
* scripting: added missing zero-pointer checks in StateMachine::executePending() (`#113 <https://github.com/orocos-toolchain/rtt/issues/113>`_)
* tests: fixed typos in corba and mqueue ipc tests that caused segfaults (`#112 <https://github.com/orocos-toolchain/rtt/issues/112>`_)
* Fixed decomposition of PropertyBags which contain types that implement the TemplateCompositionFactory interface (`#163 <https://github.com/orocos-toolchain/rtt/issues/163>`_)

2.8.2 (2015-12-21)
------------------

no changes, only in OCL

2.8.1 (2015-07-01)
------------------

New
~~~

* corba: Optionally support not emitting IORs if name service not used (`#93 <https://github.com/orocos-toolchain/rtt/issues/93>`_)

Changes
~~~~~~~

* cmake: add SOVERSION for typekits and plugins (`#104 <https://github.com/orocos-toolchain/rtt/issues/104>`_)
* cmake: also export the package install prefix (`#99 <https://github.com/orocos-toolchain/rtt/issues/99>`_)

Fixes
~~~~~

* Added ORO_WAIT_ABS and ORO_WAIT_ABS constants for all targets (`#106 <https://github.com/orocos-toolchain/rtt/issues/106>`_, `#107 <https://github.com/orocos-toolchain/rtt/issues/107>`_)
* RTT fails when using gcc5 (`#100 <https://github.com/orocos-toolchain/rtt/issues/100>`_)
* Removed setting env flags to cmake variables XXXX_FLAGS_ADD (`#88 <https://github.com/orocos-toolchain/rtt/issues/88>`_)
* Fixed "boost lambda ist not working for boost 1.57 and clang 3.4, 3.5, 3.6, ..." (`#103 <https://github.com/orocos-toolchain/rtt/issues/103>`_)
* prevent ROS generated messages linking (`#92 <https://github.com/orocos-toolchain/rtt/issues/92>`_, `#102 <https://github.com/orocos-toolchain/rtt/issues/102>`_)
* Fixed some C++11 Incompatibilities (`#94 <https://github.com/orocos-toolchain/rtt/issues/94>`_)
* typekit: fixed composition of type bool (`#95 <https://github.com/orocos-toolchain/rtt/issues/95>`_)

2.8.0 (2015-01-21)
------------------

New
~~~

* Allow the construction of an uninitialized TaskContextProxy (`#80 <https://github.com/orocos-toolchain/rtt/issues/80>`_)

  This is useful if a derived class wants to initialize the
  typekits, before initializing the Proxy itself.

* Added os::Timer::waitFor(...) and os::Timer::waitForUntil(...) member functions (`#76 <https://github.com/orocos-toolchain/rtt/issues/76>`_)

  This is needed for the implementation of `OCL::TimerComponent::wait()` and `OCL::TimerComponent::waitFor()`.
  See `ocl#23 <https://github.com/orocos-toolchain/ocl/issues/23>`_.

* Added ORO_GLOBAL_SERVICE_PLUGIN macro to ServicePlugin.hpp (`#62 <https://github.com/orocos-toolchain/rtt/issues/62>`_)

  * tests: extended plugins_test to include global service plugins
  * plugin: added ORO_GLOBAL_SERVICE_NAMED_PLUGIN for service plugins that want to install a global service

* cmake: Support configuring executables for Orocos (`#55 <https://github.com/orocos-toolchain/rtt/issues/55>`_)

Changes
~~~~~~~

* types: added assignment operators to RTT::types::carray to assign from boost::serialization::array and boost::array (`#75 <https://github.com/orocos-toolchain/rtt/issues/75>`_)

  The `boost::array` operator is required to assign an array data source from a `boost::array`, e.g. a fixed-sized
  field of a ROS message. The `boost::serialization::array` assignment operator was added for completeness as carray also
  has a `boost::serialization::array` constructor.

  The value type of the RHS of the assignment may differ from the value type of the carray as long as the elements are assignable.

* Added an `Activity(int scheduler, int priority, ...)` constructor (`#74 <https://github.com/orocos-toolchain/rtt/issues/74>`_)

  Otherwise, the contructor call `Activity(ORO_SCHED_RT, os::HighestPriority)` would interpret the second
  argument as a double period time and ORO_SCHED_RT as a priority, which can lead to very strange and unforeseen
  results.

* Message processing for SlaveActivity components (`#71 <https://github.com/orocos-toolchain/rtt/issues/71>`_)

  Messages/operations sent to components running with a SlaveActivity are forwarded to the master's execution engine
  and processed without the need to call `update()` explicitly.

  Replaces/amends earlier patches proposed in `#60 <https://github.com/orocos-toolchain/rtt/issues/60>`_ and `#35 <https://github.com/orocos-toolchain/rtt/issues/35>`_.

* Fix state_test failure by fixing StateMachine responding to signals (`#68 <https://github.com/orocos-toolchain/rtt/issues/68>`_)

  * ports: add clear() to inputport operation object.
  * scripting: correctly implement asynchronous events in eventTransition

    Before this patch, only the last eventTransition of two consecutive ones took effect,
    since the event data was shared among all subscribers. This patch makes eventTransition
    synchronous again such that each eventTransition evaluates its own data. This requires
    some proper locking in the SM.

  * scripting: more robust state change scheduling in order to avoid races

    When evenTransition comes into a state machine, this may cause re-writing
    of the entry/running/exit programs currently being executed, becaues the
    event sets these hooks immediately, instead of letting the FSM finish
    the current (entry or exit) program.
    This patch lets changeState schedule a state change, and makes enterstate/
    leavestate effectively do the state change and switch the new state programs in.
    Since changeState now only schedules, in a number of places, the executePending()
    function needs to be called to effectively execute SM statements.

  * docs: fix bug in documenting port reads + add section on level events with ports.

  Follow-up in `cd905c7 <https://github.com/orocos-toolchain/rtt/commit/cd905c72c574e27464289edefc05e0c77d047336>`_:

  * scripting: disable events in StateMachine::deactivate() and add mutex protection to StateMachine::activate()

* operations: force user to set the caller when collecting is done. (`dc81be1 <https://github.com/orocos-toolchain/rtt/commit/dc81be1ed4ec8ea7e46caf4909a505efaddec668>`_)

  In order to save users from debugging deadlocks, force them to declare the
  caller when using collect(), directly or indirectly. Does not influence send()

  Some regressions in the CORBA transport plugin have been fixed later in

  * corba: update to new SendStatus enum which contains CollectFailure (`d5bf2ae <https://github.com/orocos-toolchain/rtt/commit/d5bf2ae6d94c473cf8df7c059fbfce43a9aafc57>`_)
  * corba: explicitly provide GlobalEngine::Instance when calling operations (`#65 <https://github.com/orocos-toolchain/rtt/issues/65>`_)

Fixes
~~~~~

* Fixed naming of constants in documentation of RTT::InputPort (`f6f44fa <https://github.com/orocos-toolchain/rtt/commit/f6f44faba64ca2be0782ca7c5595b6f10b8dac86>`_)
* rtt: Add virtual destructor to OperationCallerBaseInvoker (`abb8ece <https://github.com/orocos-toolchain/rtt/commit/abb8ecee57a5e81e6c99fdd91e9f22ef32ab7051>`_)
* timer: make internal struct RTT::os::Timer::TimerInfo CopyConstructible and Assignable (`c810beb <https://github.com/orocos-toolchain/rtt/commit/c810beb7bf0dd6c614d93c65e05e296e242f9523>`_)
* cmake: suppress cmake policy CMP0042 warning in CMake >= 3.0.2 (`7021075 <https://github.com/orocos-toolchain/rtt/commit/7021075306b3dd843e41fbd09d1b62defec9a8fb>`_)
* useorocos: fixed INSTALL_NAME_DIR and MAXOSX_RPATH target properties for Mac OS (`1277069 <https://github.com/orocos-toolchain/rtt/commit/1277069b605fde749080dd1aa311eca0bb024d1d>`_)
* globals: added ORO_WAIT_ABS and ORO_WAIT_REL to the GlobalsRepository (`a480b03 <https://github.com/orocos-toolchain/rtt/commit/a480b039d1440861485ebbd5edacae6a4c736d2f>`_)
* corba: fixed reading from remote channels with old data and copy_old_data flag set to false (fix `#83 <https://github.com/orocos-toolchain/rtt/issues/83>`_)
* types: fixup BoolTypeInfo, cleanup PrimitiveTypeInfo (`8cc95bc <https://github.com/orocos-toolchain/rtt/commit/8cc95bc391fe9a48ed0cbfa30670f58f556ffe64>`_)
* types: only add a streamFactory if use_ostream is true (`a4659bb <https://github.com/orocos-toolchain/rtt/commit/a4659bb8b06a3e5e200e6501612dbb024a95b716>`_)
* os/thread: fix error log to printout the correct timeout (`ce86e85 <https://github.com/orocos-toolchain/rtt/commit/ce86e851c8dcb8ab9da5b052378dad11f5597b1c>`_)
* extras: fixed blocking trigger() calls on FileDescriptorActivities if the step() function takes too long (`#78 <https://github.com/orocos-toolchain/rtt/issues/78>`_, follow-up in `8604de5 <https://github.com/orocos-toolchain/rtt/commit/8604de59c9961c131a26096f41de16729e56d008>`_)
* re-add return statement removed during refactoring in 0e1ead2db346 (`eb9952e <https://github.com/orocos-toolchain/rtt/commit/eb9952eee97ad392b4c2e6deae6895e55a1c73dc>`_)
* corba: Make RemoteChannelElement re-entrant (`0e1ead2 <https://github.com/orocos-toolchain/rtt/commit/0e1ead2db34601508a7fcdb5ff7c57d90eb53beb>`_)
* scripting: first check if we are still loaded before we try to remove ourselves (`d8a1e9b <https://github.com/orocos-toolchain/rtt/commit/d8a1e9b73df96e1e5cf211869bbf402f68d9325f>`_)
* Clear service requester in TaskContext::clear() (`#73 <https://github.com/orocos-toolchain/rtt/issues/73>`_)
* Do not invoke user callbacks or trigger the component if it is not running (`#69 <https://github.com/orocos-toolchain/rtt/issues/69>`_, fixes `#69 <https://github.com/orocos-toolchain/rtt/issues/61>`_)
* scripting: protect FSM execution and tracing against self-deactivation (`45d832d <https://github.com/orocos-toolchain/rtt/commit/45d832dc56a26bb7d5ae392168bb7a892bc9d1d3>`_)
* cmake/useorocos: Added DESTDIR support for staged installs (`#51 <https://github.com/orocos-toolchain/rtt/issues/51>`_, `#79 <https://github.com/orocos-toolchain/rtt/issues/79>`_)
* tests: fixed "will be initialized after" compiler warning in state_test (`05ba033 <https://github.com/orocos-toolchain/rtt/commit/05ba0332aa16195bc48e27fd3046abdae09faff5>`_)
* scripting: allow parsing of keywords at the end of a parse string. (`07a4609 <https://github.com/orocos-toolchain/rtt/commit/07a4609fd64d3097484c79253b15d253e68c8470>`_)
* cmake: add all standard rpaths for all kind of targets to all kind of targes (`dafaeab <https://github.com/orocos-toolchain/rtt/commit/dafaeab316e298224c34ae67321fccef5e79a816>`_)
* timers: use rtos_get_time_ns() directly bypassing the TimeService (`8c1988a <https://github.com/orocos-toolchain/rtt/commit/8c1988acdcf3bbc2df324b344670d24ffdd793a6>`_)
* os: also cleanup the TypeInfoRepository in __os_exit (`#49 <https://github.com/orocos-toolchain/rtt/issues/49>`_)
* scripting: fix bug where SM was installing synchronous operation callbacks (`00e748c <https://github.com/orocos-toolchain/rtt/commit/00e748c5498865d210074d6fd618fca49941d355>`_)

  * revert the removal of the GlobalEngine bookkeeping. This belongs in another patch (`60fabe4 <https://github.com/orocos-toolchain/rtt/commit/60fabe4678320c7470f893bcb90da03496b8a012>`_)

* config: allow user to also define BOOST_DATE_TIME_POSIX_TIME_STD_CONFIG on a global level (`9b0c9bc <https://github.com/orocos-toolchain/rtt/commit/9b0c9bcfd8d683dc6e66399a3b85b04706676a9b>`_)
* tests: relax timer tests in order to avoid false failures. (`2b9fdb5 <https://github.com/orocos-toolchain/rtt/commit/2b9fdb533ebeeae5fa3a2e48764e4e447668d525>`_)
* os: fixed getNSecs which did not return anything meaningful. (`2c7c2c8 <https://github.com/orocos-toolchain/rtt/commit/2c7c2c8e4a74ebb442e91da3a9e6f7e9cbdf01cb>`_)

  * revert change caused by commit 2c7c2c8e until we have a solution. (`085aad1 <https://github.com/orocos-toolchain/rtt/commit/085aad16bc68c108f9e394bb08c317d73a37b7c2>`_)

* Do not reset the caller engine in OperationCaller assignment (`8128106 <https://github.com/orocos-toolchain/rtt/commit/81281067c891b28929e9e8a0520ec04f54247216>`_)
* cmake: Fix OS X RPATH issue with CMake 3.0 (`#47 <https://github.com/orocos-toolchain/rtt/issues/47>`_)
* cmake: remove duplicate rpath settings for Apple (`#42 <https://github.com/orocos-toolchain/rtt/issues/42>`_)
* os: update oro_allocator to be copy-able. Required for newer compilers. (`#45 <https://github.com/orocos-toolchain/rtt/issues/45>`_)
* osx: switch off boost chrono to allow typegen-generated typekits using rt_string (`#41 <https://github.com/orocos-toolchain/rtt/issues/41>`_)
* Fix visibility flag for clang and intel compilers (`#38 <https://github.com/orocos-toolchain/rtt/issues/38>`_)

  * cmake: enable the visibility flag also for clang and intel compilers, needed if you use the rtt/typekit/Types.hpp explicit instantiation header

* typekit: include explicit instantiation for rt_string based RTT classes ... (`#39 <https://github.com/orocos-toolchain/rtt/issues/39>`_)
* Explicit instantiation (`#37 <https://github.com/orocos-toolchain/rtt/issues/37>`_)

  * internal: fix for explicit instantiantion of Operations and friends
  * fix constness of setOperationCallerImpl functions

* tests: don't require that a fd activity is running right after component... (`#40 <https://github.com/orocos-toolchain/rtt/issues/40>`_)

  * Since components trigger when started, the fd activity is shortly running.
    Travis found this from time to time.

* set a port's interface to 0 when it gets removed (`#36 <https://github.com/orocos-toolchain/rtt/issues/36>`_)
* TaskContext: fixed zero pointer fault (part of `#35 <https://github.com/orocos-toolchain/rtt/issues/35>`_)


Orocos Toolchain 2.7
====================

The Orocos Toolchain v2.7 release series mainly improved on the cmake building
side and removing all the ROS interactions. It also added features and improvements
proposed by the community.

Important Caveats
-----------------

* There were changes in the RTT::TaskContext API, where RTT::ServiceRequester
  became a shared_ptr and getName() became const. ServiceRequester
  is still considered an experimental feature.

* The RTT::ComponentLoader has been changed to be again independent
  to ROS and the rtt_ros_integration package manages importing ROS
  packages.

* RTT::FileDescriptorActivity was extended with timeouts at micro
  second resolution.

* The RTT DataFlow.idl takes an extra argument in channelReady() in order
  to pass on the connection policy, which is required for correct
  channel construction.

Improvements
------------

* The main change in this release is the cleanup that happened
  in the Orocos RTT CMake macros, which no longer behave differently
  when the ROS_PACKAGE_PATH or ROS_ROOT has been set. Version 2.6
  and earlier switched to a rosbuild layout, which proved to be
  undesirable. We still detect a CATKIN or rosmake build
  in case these tools are used and marked as such in the CMakeLists.txt
  files.

* Signalling operations have been introduced to allow adding multiple callbacks
  to operations, in addition to calling the operation's user function.
  The RTT scripting state machines use this mechanism to respond to
  calls on the Service interface.

* Logging the RTT logger to log4cpp was added and can be enabled
  at using a cmake flag in RTT.

* The thread of the RTT::GlobalEngine can be configured during instantiation.

* Loading and Storing RTT::Service properties has been added to the
  RTT::MarshallingService.

* RTT::os::Thread now provides a member function to set the stop() timeout.

* There were several fixes to RTT::scripting for correct execution of
  OwnThread / ClientThread operations, as well as parser improvements.

* RTT::rt_string was added to the RTT CORBA transport.

* The RTT mqueue transport is more relaxed to accepting types
  with virtual tables, in case no memcpy is used to marshall.


Orocos Toolchain 2.6 and before
===============================

See http://www.orocos.org/stable/documentation/rtt/v2.6.x/doc-xml/orocos-rtt-changes.html.
