^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package rtt
^^^^^^^^^^^^^^^^^^^^^^^^^

2.6.9 (2013-10-10)
-------------------
* catkin: added run_depend from catkin to package.xml for released packages
  Signed-off-by: Johannes Meyer <johannes@intermodalics.eu>
* Merge branch 'catkin' into prepare-catkin-merge
  Conflicts:
  UseOROCOS-RTT.cmake
* os: fix for bug 1046: OSX 10.8, boost 1.54, apple-gcc-4.2 RTT main-test FAILS
  Apparently copying the boost functions went wrong and we ended up with a NULL pointer.
  Fixed by directly using the function pointer instead of the boost function.
  Signed-off-by: Ruben Smits <ruben@intermodalics.eu>
* fix for bug 1047: Added missing boost version check
  Signed-off-by: Ruben Smits <ruben@intermodalics.eu>
* tests: disable testThreadPID for macosx, this is not supported yet
  Signed-off-by: Ruben Smits <ruben@intermodalics.eu>
* cmake: fix the SelectOneLibrary macro, it added one } too many
  Signed-off-by: Ruben Smits <ruben.smits@intermodalics.eu>
* Merge pull request #17 from jhu-lcsr-forks/remove_duplicates_from_use_orocos_variables-updated
  Removing duplicates, with merged conflicts
* resolving conflicts in duplicate removal in orocos_use_package
* Merge pull request #16 from jhu-lcsr-forks/find-vs-use-macro
  Fixing the distinction between "find" and "use"
* Merge pull request #18 from jhu-lcsr-forks/rosbuild_orocos_libdir_with_package_name-updated
  Add package name to the orocos libdir also for rosbuild packages Edit
* Resolving conflict
* Merge branch 'catkin' into rosbuild_orocos_libdir_with_package_name
* Fixing the distinction between "find" and "use"
  orocos_find_package() now only finds and sets individual ${${PACKAGE}_*}
  variables, and orocos_use_package() now adds those variables to the
  aggregated ${USE_OROCOS_*} variables. As such, the catkin package.xml
  parsing now uses orocos_use_package() but with OROCOS_NO_AUTO_LINKING
  enabled so that nothing gets auto-linked. This enables people to _find_
  orocos packages withoug having other side-effects not related to that
  package.
* Adding xpath to manifest rosdep
* increasing number of travis cores
* Merge pull request #10 from jhu-lcsr-forks/repair
  Additional features / robustness for orocos_use_package and orocos_find_package
* Merge branch 'catkin' into repair
* disabling more tests which don't work on CI
* Merge branch 'catkin' of github.com:jhu-lcsr-forks/rtt into catkin
* adding xpath rosdep
* missing xpath dep
* compatibility with rosdep just isn't worth this
* only install package.xml.in files automatically
* fixing package.xml rename in install target
* Merge branch 'repair' of github.com:jhu-lcsr-forks/rtt into repair
* Adding install targets for orocos sub-packages
* Merge pull request #12 from jhu-lcsr-forks/disable-affinity-test-on-ci
  disabling cpu affinity tests with the CI environment variable is defined
* disabling cpu affinity tests with the CI environment variable is defined, workaround for #11
* adding travis build config
* Adding more status output
* adding STATUS, un-caching orocos_find and orocos_use macro variables
* removing auto-link from targets
* cleaning up
* Adding options to orocos_find_package and orocos_use_package, adding more debug output, adding link commands to orocos_* targets
* wip: repair
* Merge remote-tracking branch 'autobuild/master' into next
* macosx: fix clashing Posix features defines
  Signed-off-by: Ruben Smits <ruben@intermodalics.eu>
* cmake: fix APPLE LINKFLAGS wrt rpath settings
  Signed-off-by: Ruben Smits <ruben@intermodalics.eu>
* cmake: add project/package name suffix to the orocos libdir
* Fix building with boost 1.53: this header has been deprecated since boost 1.53
  Signed-off-by: Ruben Smits <ruben.smits@mech.kuleuven.be>
* Merge remote-tracking branch 'orocos/master'
* Default to dry setup for Groovy and wet for Hydro or later. Set NO_ROS_BUILD to force wet/catkin.
  Signed-off-by: Peter Soetens <peter@thesourceworks.com>
* Merge remote-tracking branch 'orocos/master'
* extras: fix ROPtr::reset(ptr) when 'ptr' is the actual pointer that 'this' already manages
* useorocos: generate typekits in the project source dir
  Required in case we use catkin.
  Signed-off-by: Peter Soetens <peter@thesourceworks.com>
* Merge remote-tracking branch 'orocos/master'
* add missing setter for RTT::os::Thread::stopTimeout
* os: add a way to set the stop timeout on a per-activity basis
* cmake: UseOrocos macros should use the correct way of rpath setting on OSX
  Signed-off-by: Ruben Smits <ruben@intermodalics.eu>
* Also append target suffix for macosx to not confuse the OCL utility scripts
  Signed-off-by: Ruben Smits <ruben@intermodalics.eu>
* deregister the dataflow servant in cleanupServer
* corba: CORBA was doing an unnecessary copy in CorbaTemplateProtocol
  The issue was a mismatch between RemoteChannelElement using
  Late(Const)ReferenceDataSource and CorbaTemplateProtocol looking
  for (Const)ReferenceDataSource. Both have been moved to the Late*
  versions.
  This most likely will improve performance of the CORBA transport
* mqueue: factor the createStream in a base class
  The issue this commit tries to fix is that all subclasses of
  MQTemplateProtocol have the restriction that T needs to have
  no virtual tables, as a static assert in MQTemplateProtocol
  checks this.
  However, there is no need for such a check in the general case,
  only the "use memcpy to marshal" case has this issue. So,
  create MQTemplateProtocolBase to get the completely generic
  createStream method, and make the other marshaller implementations
  depend on it
* Merge remote-tracking branch 'autobuild/master' into next
* Merge remote-tracking branch 'autobuild/stable' into next
* Allow customization of lock timeout for periodic and non periodic threads
* Fixed cmake 2.8.9 bugg during include, Cmake only acceps true if find_package is used
* Merge remote-tracking branch 'autobuild/master' into orocos_master
* Merge remote-tracking branch 'autobuild/next'
  This reverts the (wrongly) pushed rtt_next changes
* Fixed cmake 2.8.9 bugg during include, Cmake only acceps true if find_package is used

* catkin: added run_depend from catkin to package.xml for released packages
  Signed-off-by: Johannes Meyer <johannes@intermodalics.eu>
* Merge branch 'catkin' into prepare-catkin-merge
  Conflicts:
  UseOROCOS-RTT.cmake
* os: fix for bug 1046: OSX 10.8, boost 1.54, apple-gcc-4.2 RTT main-test FAILS
  Apparently copying the boost functions went wrong and we ended up with a NULL pointer.
  Fixed by directly using the function pointer instead of the boost function.
  Signed-off-by: Ruben Smits <ruben@intermodalics.eu>
* fix for bug 1047: Added missing boost version check
  Signed-off-by: Ruben Smits <ruben@intermodalics.eu>
* tests: disable testThreadPID for macosx, this is not supported yet
  Signed-off-by: Ruben Smits <ruben@intermodalics.eu>
* cmake: fix the SelectOneLibrary macro, it added one } too many
  Signed-off-by: Ruben Smits <ruben.smits@intermodalics.eu>
* Merge pull request #17 from jhu-lcsr-forks/remove_duplicates_from_use_orocos_variables-updated
  Removing duplicates, with merged conflicts
* resolving conflicts in duplicate removal in orocos_use_package
* Merge pull request #16 from jhu-lcsr-forks/find-vs-use-macro
  Fixing the distinction between "find" and "use"
* Merge pull request #18 from jhu-lcsr-forks/rosbuild_orocos_libdir_with_package_name-updated
  Add package name to the orocos libdir also for rosbuild packages Edit
* Resolving conflict
* Merge branch 'catkin' into rosbuild_orocos_libdir_with_package_name
* Fixing the distinction between "find" and "use"
  orocos_find_package() now only finds and sets individual ${${PACKAGE}_*}
  variables, and orocos_use_package() now adds those variables to the
  aggregated ${USE_OROCOS_*} variables. As such, the catkin package.xml
  parsing now uses orocos_use_package() but with OROCOS_NO_AUTO_LINKING
  enabled so that nothing gets auto-linked. This enables people to _find_
  orocos packages withoug having other side-effects not related to that
  package.
* Adding xpath to manifest rosdep
* increasing number of travis cores
* Merge pull request #10 from jhu-lcsr-forks/repair
  Additional features / robustness for orocos_use_package and orocos_find_package
* Merge branch 'catkin' into repair
* disabling more tests which don't work on CI
* Merge branch 'catkin' of github.com:jhu-lcsr-forks/rtt into catkin
* adding xpath rosdep
* missing xpath dep
* compatibility with rosdep just isn't worth this
* only install package.xml.in files automatically
* fixing package.xml rename in install target
* Merge branch 'repair' of github.com:jhu-lcsr-forks/rtt into repair
* Adding install targets for orocos sub-packages
* Merge pull request #12 from jhu-lcsr-forks/disable-affinity-test-on-ci
  disabling cpu affinity tests with the CI environment variable is defined
* disabling cpu affinity tests with the CI environment variable is defined, workaround for #11
* adding travis build config
* Adding more status output
* adding STATUS, un-caching orocos_find and orocos_use macro variables
* removing auto-link from targets
* cleaning up
* Adding options to orocos_find_package and orocos_use_package, adding more debug output, adding link commands to orocos_* targets
* wip: repair
* Merge remote-tracking branch 'autobuild/master' into next
* macosx: fix clashing Posix features defines
  Signed-off-by: Ruben Smits <ruben@intermodalics.eu>
* cmake: fix APPLE LINKFLAGS wrt rpath settings
  Signed-off-by: Ruben Smits <ruben@intermodalics.eu>
* cmake: add project/package name suffix to the orocos libdir
* Fix building with boost 1.53: this header has been deprecated since boost 1.53
  Signed-off-by: Ruben Smits <ruben.smits@mech.kuleuven.be>
* Merge remote-tracking branch 'orocos/master'
* Default to dry setup for Groovy and wet for Hydro or later. Set NO_ROS_BUILD to force wet/catkin.
  Signed-off-by: Peter Soetens <peter@thesourceworks.com>
* Merge remote-tracking branch 'orocos/master'
* extras: fix ROPtr::reset(ptr) when 'ptr' is the actual pointer that 'this' already manages
* useorocos: generate typekits in the project source dir
  Required in case we use catkin.
  Signed-off-by: Peter Soetens <peter@thesourceworks.com>
* Merge remote-tracking branch 'orocos/master'
* add missing setter for RTT::os::Thread::stopTimeout
* os: add a way to set the stop timeout on a per-activity basis
* cmake: UseOrocos macros should use the correct way of rpath setting on OSX
  Signed-off-by: Ruben Smits <ruben@intermodalics.eu>
* Also append target suffix for macosx to not confuse the OCL utility scripts
  Signed-off-by: Ruben Smits <ruben@intermodalics.eu>
* deregister the dataflow servant in cleanupServer
* corba: CORBA was doing an unnecessary copy in CorbaTemplateProtocol
  The issue was a mismatch between RemoteChannelElement using
  Late(Const)ReferenceDataSource and CorbaTemplateProtocol looking
  for (Const)ReferenceDataSource. Both have been moved to the Late*
  versions.
  This most likely will improve performance of the CORBA transport
* mqueue: factor the createStream in a base class
  The issue this commit tries to fix is that all subclasses of
  MQTemplateProtocol have the restriction that T needs to have
  no virtual tables, as a static assert in MQTemplateProtocol
  checks this.
  However, there is no need for such a check in the general case,
  only the "use memcpy to marshal" case has this issue. So,
  create MQTemplateProtocolBase to get the completely generic
  createStream method, and make the other marshaller implementations
  depend on it
* Merge remote-tracking branch 'autobuild/master' into next
* Merge remote-tracking branch 'autobuild/stable' into next
* Allow customization of lock timeout for periodic and non periodic threads
* Fixed cmake 2.8.9 bugg during include, Cmake only acceps true if find_package is used
* Merge remote-tracking branch 'autobuild/master' into orocos_master
* Merge remote-tracking branch 'autobuild/next'
  This reverts the (wrongly) pushed rtt_next changes
* Fixed cmake 2.8.9 bugg during include, Cmake only acceps true if find_package is used
