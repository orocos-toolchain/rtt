#!/bin/bash
# restart using a Tcl shell \
    exec sh -c 'for tclshell in tclsh tclsh83 cygtclsh80 ; do \
            ( echo | $tclshell ) 2> /dev/null && exec $tclshell "`( cygpath -w \"$0\" ) 2> /dev/null || echo $0`" "$@" ; \
        done ; \
        echo "orocos-install.tcl: cannot find Tcl shell" ; exit 1' "$0" "$@"

# {{{  Banner

#===============================================================================
#
#       orocos-install.tcl
#
#       Installation utility for eCos.
#       Adapted by Peter Soetens for Orocos. 
#       All original copyrights remain. This program is Free Software.
#
#===============================================================================
#####HOSTGPLCOPYRIGHTBEGIN####
## -------------------------------------------
## This file is part of eCos, the Embedded Configurable Operating System.
## Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
## Copyright (C) 2003 eCosCentric Ltd.
##
## eCos is free software; you can redistribute it and/or modify it under
## the terms of the GNU General Public License as published by the Free
## Software Foundation; either version 2 or (at your option) any later version.
##
## eCos is distributed in the hope that it will be useful, but WITHOUT ANY
## WARRANTY; without even the implied warranty of MERCHANTABILITY or
## FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
## for more details.
##
## You should have received a copy of the GNU General Public License along
## with eCos; if not, write to the Free Software Foundation, Inc.,
## 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
## -------------------------------------------
#####HOSTGPLCOPYRIGHTEND####
#===============================================================================
# TODO list:
# Incorporate tarball and uncompressed sizes into orocos-install.db
# Allow tarballs to be unzipped as they are downloaded for better disk space
# usage
# Tk version
# Proper version selection, not just latest (both eCos and tools)
# Generate a batch file, put things into registry if needed, install desktop
# start menu icons (both windows and linux). Best wait for Tk version for that.
# Optionally download and incorporate PDF docs
# Uninstall might be nice, but by this point perhaps we should be using a proper
# installer really!
# Checksums on downloaded data in orocos-install.db. Then we can also check for
# existing images downloaded to /opt/ecos rather than bombing out.
# 
#===============================================================================
# }}}
# {{{  Version check

# ----------------------------------------------------------------------------
# orocos-install.tcl requires at least version 8.0 of Tcl, since it makes use of
# namespaces. It is possible that some users still have older versions.

if { [info tclversion] < 8.0 } {
        puts "This script requires Tcl 8.0 or later. You are running Tcl [info patchlevel]."
        return
}

# }}}
# {{{  Namespace definition

# ----------------------------------------------------------------------------
# Namespaces. All code and variables in this script are kept in the namespace
# "ecos_install". This is not really necessary for stand-alone operation, but
# if it ever becomes desirable to embed this script in a larger application then
# using a namespace is a lot easier.
#
# As a fringe benefit, all global variables can be declared inside this
# namespace and initialised.
#

namespace eval ecos_install {
        # What OS is this?
        variable os $tcl_platform(platform)
        # "unix" isn't specific enough, so:
        if { $os == "unix" } {
                set os $tcl_platform(os)
        }

        variable original_pwd [pwd]
        # Where this script lives. On a CD-ROM this will be at a known place relative to
        # the rest of the install.
        variable script_base $original_pwd
        if { [file dirname $argv0] != "." } {
                set script_base [ file join $script_base [file dirname $argv0] ]
        }

        # Mirror URL base to use. For a CDROM install, this can be
        # changed to refer to $script_base
#        variable mirror $script_base
        variable mirror ""
        
        # Where we get the install data from. A list of built-in options, but this is *not*
        # meant to be a rehash of the mirrors list.
	# 	   
        variable metadata_url { \
			         "http://people.mech.kuleuven.ac.be/~psoetens/orocos-packages/orocos-install.db"\
				 "http://localhost/orocos-install.db"\
				}

        # Base of installation
        variable install_dir ""

        # Tool prefixes to install
        variable tool_prefixes { }

        # Tarballs to unpack
        variable tarballs_to_unpack ""
        
        # ECOS_REPOSITORY suffix to use
        variable ecos_repo_suffix ""
        # PATH element suffix for eCos tools
        variable ecos_path_suffixes { }
        # Ditto for GNU tools and PATH
        variable gnutool_path_suffixes { }

        # Other values associated with arguments
        variable tools_only 0
        variable force 0
        variable verbose 0
        variable metadata_url_supplied 0

        # proxy details
        variable proxy ""
        variable proxy_user ""
        variable proxy_pass ""

        # What routines should be invoked for outputting fatal errors and
        # for warning messages ?
        variable fatal_error_handler ecos_install::cli_fatal_error_handler
        variable warning_handler     ecos_install::cli_warning_handler
        variable report_handler      ecos_install::cli_report_handler

        # Data from metadata script
        array set ecosreleases { }
        array set toolchains { }
        variable ecosmirrors ""
	variable chosenecos  ""
}

# }}}
# {{{  Infrastructure

# ----------------------------------------------------------------------------
# Minimal infrastructure support.
#
# There must be some way of reporting fatal errors, of outputting warnings,
# and of generating report messages. The implementation of these things
# obviously depends on whether or not TK is present. In addition, if this
# script is being run inside a larger application then that larger
# application must be able to install its own versions of the routines.
#
# Once it is possible to report fatal errors, an assertion facility becomes
# feasible.
#
# These routines output fatal errors, warnings or miscellaneous messages.
# Their implementations depend on the mode in which this script is operating.
#
proc ecos_install::fatal_error { msg } {
        $ecos_install::fatal_error_handler "$msg"
}

proc ecos_install::warning { msg } {
        $ecos_install::warning_handler "$msg"
}

proc ecos_install::report { msg } {
        $ecos_install::report_handler "$msg"
}

#
# Command line versions.
# NOTE: some formatting so that there are linebreaks at ~72 columns would be
# a good idea.
#
proc ecos_install::cli_fatal_error_handler { msg } {
        if { $ecos_install::verbose != 0 } {
                error "$msg" $::errorInfo
        } else {
                puts "\n*** orocos-install.tcl error: $msg"
                exit 1
        }
}

proc ecos_install::cli_warning_handler { msg } {
        puts "ecos_install.tcl warning: $msg"
}

proc ecos_install::cli_report_handler { msg } {
        puts "$msg"
}

#
# Determine the default destination for warnings and for fatal errors.
# After the first call to this function it is possible to use assertions.
#
proc ecos_install::initialise_error_handling { } {
        set ecos_install::fatal_error_handler ecos_install::cli_fatal_error_handler
        set ecos_install::warning_handler     ecos_install::cli_warning_handler
        set ecos_install::report_handler      ecos_install::cli_report_handler
}

#
# These routines can be used by containing programs to provide their
# own error handling.
#
proc ecos_install::set_fatal_error_handler { fn } {
        ASSERT { $fn != "" }
        set ecos_install::fatal_error_handler $fn
}

proc ecos_install::set_warning_handler { fn } {
        ASSERT { $fn != "" }
        set ecos_install::warning_handler $fn
}

proc ecos_install::set_report_handler { fn } {
        ASSERT { $fn != "" }
        set ecos_install::report_handler $fn
}

#
# A very simple assertion facility. It takes a single argument, an expression
# that should be evaluated in the calling function's scope, and on failure it
# should generate a fatal error.
#
proc ecos_install::ASSERT { condition } {
        set result [uplevel 1 [list expr $condition]]
        
        if { $result == 0 } {
                fatal_error "assertion predicate \"$condition\"\nin \"[info level -1]\""
        }
}

proc ecos_install::file_or_url_join { base element } {
        if { [ string index [ string trim $base ] 0 ] == "/" } {
                # Starts with / so is a file
                return [ file join $base $element ]
        } else {
                # Is a URL
                return [ append dummy $base "/$element" ]
        }
}

proc ecos_install::file_or_url_basename { url } {
        set lastelement [string last "/" $url ]
        incr lastelement
        return [string range $url $lastelement [string length $url]]
}

proc ecos_install::is_file_not_url { url } {
        # If it begins with a / or doesn't contain a :// we guess it's a file.
        if { [ string index [string trim $url] 0 ] == "/" || [ string first "://" $url ] == -1 } {
                return 1
        }
        return 0
}

# A replacement for string is digit which doesn't exist in earlier TCL evidently
proc ecos_install::stringisdigit { s } {
        for { set i 0 } { $i < [string length $s] } { incr i } {
                set c [string index $s $i]
                if { $c != "0" && $c != "1" && $c != "2" && $c != "3" && $c != "4" && \
                         $c != "5" && $c != "6" && $c != "7" && $c != "8" && $c != "9" } {
                        return 0
                }
        }
        return 1
} 

# }}}
# {{{  Argument parsing

# ----------------------------------------------------------------------------
# The argv0 argument should be the name of this script. It can be used
# to get at the component repository location. If this script has been
# run incorrectly then currently it will fail: in future it may be
# desirable to check an environment variable instead.
#
# The argv argument is a string containing the rest of the arguments.
# If any of the arguments contain spaces then this argument will be
# surrounded by braces. If any of the arguments contain braces then
# things will break.
#

proc ecos_install::parse_arguments { argv0 argv } {

        if { $argv != "" } {
                # There are arguments. If any of the arguments contained
                # spaces then these arguments will have been surrounded
                # by braces, which is a nuisance. So start by turning the
                # arguments into a numerically indexed array.

                set argc 0
                array set args { }
                foreach arg $argv {
                        set args([incr argc]) $arg
                }
                # Now examine each argument.
                for { set i 1 } { $i <= $argc } { incr i } {

                        # Catch options beginning with -
                        if { [ string index $args($i) 0 ] == "-" } {
                                switch -regexp -- $args($i) {
                                        "^(-v|--verbose)" { incr ecos_install::verbose }
                                        "^(-m|--mirror)"
                                        { 
                                                if { $i == $argc } {
                                                        fatal_error "No argument given to \"$args($i)\""
                                                }
                                                incr i
                                                set ecos_install::mirror $args($i)
                                        }
                                        "^(-t|--tools-only)" { set ecos_install::tools_only 1 }
                                        "^(-f|--force)" { set ecos_install::force 1 }
                                        "^(-c|--config)"
                                        {
                                                if { $i == $argc } {
                                                        fatal_error "No argument given to \"$args($i)\""
                                                }
                                                incr i
                                                set ecos_install::metadata_url [list $args($i)]
                                                set ecos_install::metadata_url_supplied 1
                                        }
                                        "^(-U|--proxy-user)"
                                        {
                                                if { $i == $argc } {
                                                        fatal_error "No argument given to \"$args($i)\""
                                                }
                                                incr i
                                                set ecos_install::proxy_user $args($i)
                                        }
                                        "^(-P|--proxy-passwd)"
                                        {
                                                if { $i == $argc } {
                                                        fatal_error "No argument given to \"$args($i)\""
                                                }
                                                incr i
                                                set ecos_install::proxy_pass $args($i)
                                        }
                                        "^(-p|--proxy)"
                                        {
                                                if { $i == $argc } {
                                                        fatal_error "No argument given to \"$args($i)\""
                                                }
                                                incr i
                                                set ecos_install::proxy $args($i)
                                        }
                                        "^(-h|-H|-help|--help|--H)" { 
                                                # check for --help or any of the variants.
                                                ecos_install::argument_help
                                                exit 0
                                        }
                                        default { fatal_error "unknown option: \"$args($i)\"" }
                                }
                                continue
                        }
                        # Set installdir if we haven't already set it
                        if { $ecos_install::install_dir == "" } {
                                # Set install dir, converting cygwin to windows TCL paths as necessary
                                set ecos_install::install_dir [get_pathname_for_tcl $args($i)]
                                continue
                        }

                        # Add a tool prefix
                        lappend ecos_install::tool_prefixes $args($i)
                }
        }
}

# ----------------------------------------------------------------------------
#
# Display help information if the user has typed --help, -H, --H, or -help.
# The help text uses two hyphens for consistency with configure.
# Arguably this should change.

proc ecos_install::argument_help { } {

        puts "Usage: orocos-install.tcl \[OPTION\] \[INSTALLPATH\] \[ toolchain-prefix \]..."
        puts "  e.g. orocos-install.tcl /opt/ecos arm-elf i386-elf"
        puts ""
        puts "Options:"
        puts "  -c <file|URL>   Get install metadata configuration from the specified"
        puts "                  file or URL."
        puts "  -f              Force. Overwrite existing files."
        puts "  -m <URL>        Specify the supplied URL as the eCos distribution"
        puts "                  mirror to use."
        puts "  -p <proxy:port> Use a proxy to connect, e.g. if necessary to connect via"
        puts "                  a firewall. e.g. -p proxy.me.com:3128"
        puts "  -U <user>       An optional username to pass to the specified proxy if it"
        puts "                  requires authentication."
        puts "  -P <passwd>     An optional password to pass to the specified proxy if it"
        puts "                  requires authentication."
        puts "  -t              Tools only, no eCos."
        puts "  -v              Verbose output."
}

# }}}
# {{{
# ----------------------------------------------------------------------------
# Take a cygwin filename such as /d/tmp/pkgobj and turn it into something
# acceptable to Tcl, i.e. d:/tmp/pkgobj. There are a few other complications...

proc ecos_install::get_pathname_for_tcl { name } {

        if { $ecos_install::os == "windows" } {

                # If there is no logical drive letter specified
                if { [ string match "?:*" $name ] == 0 } {

                        # Invoke cygpath to resolve the POSIX-style path
                        if { [ catch { exec cygpath -w $name } result ] != 0 } {
                                fatal_error "processing filepath $name:\n$result"
                        }
                } else {
                        set result $name
                }

                # Convert backslashes to forward slashes
                regsub -all -- {\\} $result "/" name
        }

        return $name
}

# }}}
# {{{
# ----------------------------------------------------------------------------
# retrieve_url

proc ecos_install::retrieve_url { url quiet outputfilename } {
        if { $ecos_install::verbose > 0 } { 
                set quiet 0
        }

        # If this is a local file, just copy it rather than invoking wget.
        # If it begins with a / or doesn't contain a :// we guess it's a file.
        if { [ is_file_not_url $url ] } {
                # may be a relative path, so lets go back to where we were temporarily just in case
                set saved_pwd [pwd]
                cd $ecos_install::original_pwd
                puts "Copying from [ file nativename [ get_pathname_for_tcl $url ] ]"
                if { $ecos_install::force } {
                        file copy -force -- [ get_pathname_for_tcl $url ] [ file join $saved_pwd $outputfilename ]
                } else {
                        file copy -- [ get_pathname_for_tcl $url ] [ file join $saved_pwd $outputfilename ]
                }
                cd $saved_pwd
                return
        }
        for {set retries 3} {$retries > 0} {incr retries -1} {
        
                set wget_extra_args ""
                if { $ecos_install::proxy != "" } {
                        set ::env(http_proxy) "http://$ecos_install::proxy/"
                        set ::env(ftp_proxy) "http://$ecos_install::proxy/"
                        if { $ecos_install::proxy_user != "" } {
                                set wget_extra_args "$wget_extra_args --proxy-user=$ecos_install::proxy_user"
                        }
                        if { $ecos_install::proxy_pass != "" } {
                                set wget_extra_args "$wget_extra_args --proxy-passwd=$ecos_install::proxy_pass"
                        }
                }

                if { $outputfilename != "" } {
                        set wget_extra_args "$wget_extra_args -O $outputfilename"
                }

                set wget_output ""
                set wget_fd [ open "|wget -c -t 3 $wget_extra_args --passive-ftp $url |& cat -" "r" ]
                # yes, really set lastpercent to 1, we don't want to match 0 in the modulo further down
                set lastpercent 1    

                while { -1 != [ gets $wget_fd line ] } {
                        if { $quiet == 0 } {
                                ecos_install::report $line
                        } else {
                                # Parse wget output and map it into something more tractable.

                                lappend wget_output $line
                                switch -regexp -- $line {
                                        "^Connecting.*connected" { 
                                                puts "Connected... "
                                                flush stdout
                                        }
                                        "^Length:" {
                                                ecos_install::report "Downloading [file_or_url_basename $url]... "
                                                ecos_install::report "File size [ lindex $line 1 ] bytes"
                                                puts -nonewline "\[                                                  \]"
                                                for { set i 0 } { $i < 51 } { incr i } {
                                                        puts -nonewline "\b"
                                                }
                                                flush stdout
                                        }
                                        "%" {
                                                set percentindex [ string first "%" $line ]

                                                # identify space containing percentage
                                                set percentstart 0
                                                for { set i [expr $percentindex - 1] } \
                                                    { $i > 0 && [ stringisdigit [ string index $line $i ] ] } \
                                                    { incr i -1 } {
                                                            set percentstart $i
                                                    }        
                                                
                                                set percent [ string trim [string range $line $percentstart [expr $percentindex - 1 ] ] ]
                                                # puts "$percent $lastpercent"
                                                for { } { $lastpercent <= $percent } { incr lastpercent } {
                                                        if { [expr ($lastpercent % 2) ] == 0 } {
                                                                puts -nonewline "*"
                                                        }
                                                }
                                                flush stdout
                                        }
                                        "file is already fully retrieved" {
                                                ecos_install::report $line
                                        }
                                }
                        }
                        
                        fileevent $wget_fd readable ""
                }
                puts ""
                # gets has returned -1 by here
                if { [catch "close $wget_fd" code] } {
                        # If we had not been requested to be quiet, the wget output would already
                        # be visible. So if we are "quiet", display the last bit since there was an error.
                        if { $quiet != 0 } {
                                ecos_install::report "*** wget returned error. Last five lines of wget output:"
                                set outputlist [ lrange $wget_output [ expr [llength $wget_output] - 5 ] [llength $wget_output] ]

                                for { set i 0 } { $i < [ llength $outputlist ] } { incr i } {
                                        ecos_install::report [ lindex $outputlist $i ]
                                }
                                ecos_install::report "---------------------------------------------------------"
                                ecos_install::report "Download failed. Perhaps you need to set a proxy to"
                                ecos_install::report "connect via a firewall?"
                                set status -1
                                while { $status < 0 } {
                                        puts -nonewline "\nConfigure a proxy? \[Y/n\] "
                                        set status [gets stdin userin]
                                        # default return pressed?
                                        if { $status == 0 } {
                                                set userin "y"
                                                set status 1
                                        }
                                        if { $status > 0 } {
                                                set userin [string toupper $userin]
                                                if { $userin == "Y" || $userin == "YES" } {
                                                        set status -1
                                                        while { $status <= 0 } {
                                                                puts -nonewline "\nProxy hostname:port (e.g. proxy.me.com:3128): "
                                                                set status [gets stdin userin]
                                                                if { $status > 0 } {
                                                                        set ecos_install::proxy $userin
                                                                }
                                                        }
                                                } elseif { $userin == "N" || $userin == "NO" } {
                                                        fatal_error "Installation aborted."
                                                } else {
                                                        set status -1
                                                }
                                        } else {
                                                fatal_error "Installation aborted"
                                        }
                                }
                                if { $ecos_install::proxy != "" } {
                                        puts -nonewline "\nConfigure username/password to access proxy? \[y/N\] "
                                        set status [gets stdin userin]
                                        # default return pressed?
                                        if { $status == 0 } {
                                                set userin "n"
                                                set status 1
                                        }
                                        if { $status > 0 } {
                                                set userin [string toupper $userin]
                                                if { $userin == "Y" || $userin == "YES" } {
                                                        set status -1
                                                        while { $status <= 0 } {
                                                                puts -nonewline "\nProxy username: "
                                                                set status [gets stdin userin]
                                                                if { $status > 0 } {
                                                                        set ecos_install::proxy_user $userin
                                                                }
                                                        }
                                                        set status -1
                                                        while { $status <= 0 } {
                                                                puts -nonewline "\nProxy password: "
                                                                set status [gets stdin userin]
                                                                if { $status > 0 } {
                                                                        set ecos_install::proxy_pass $userin
                                                                }
                                                        }
                                                } elseif { $userin != "N" && $userin != "NO" } {
                                                        set status -1
                                                }
                                        } else {
                                                fatal_error "Installation aborted"
                                        }
                                }
                        }
                } else {
                        # completed successfully
                        return
                }
        }
        fatal_error "Installation aborted due to excessive download failures"
}

# }}}
# {{{
# ----------------------------------------------------------------------------
# select_gnu_tools

proc ecos_install::select_gnu_tools { } {
        ecos_install::report "---------------------------------------------------------"
        ecos_install::report "Available prebuilt GNU tools:\n"
        set status -1
        set userin ""
        while { 1 } {
                for { set i 0 } { $i < [llength $ecos_install::toolchains($ecos_install::os)] } { incr i } {
                        set optionnumtxt [expr ($i+1)]
                        set tool [lindex $ecos_install::toolchains($ecos_install::os) $i]
                        set currprefix [lindex $tool 0]
                        if { [lsearch -exact $ecos_install::tool_prefixes $currprefix] != -1 } {
                                set optionnumtxt "*"
                        }
                        ecos_install::report "\[$optionnumtxt\]\t$currprefix"
                }
                ecos_install::report "\[q\]\tFinish selecting GNU tools"
                ecos_install::report "\n(\"*\" indicates tools already selected)"
                puts -nonewline "\nPlease select GNU tools to download and install: "
                set status [gets stdin userin]
                if { $status > 0 } {
                        if { [string tolower $userin] == "q" } {
                                break
                        }
                        if { ($userin < 1) || ($userin > [llength $ecos_install::toolchains($ecos_install::os)]) } {
                                set status -1
                                ecos_install::report "Invalid selection!"
                        } else {
                                set chosenprefix [lindex [lindex $ecos_install::toolchains($ecos_install::os) [expr ($userin-1)]] 0]
                                # check not already selected
                                if { [lsearch -exact $ecos_install::tool_prefixes $chosenprefix] == -1 } {
                                        lappend ecos_install::tool_prefixes $chosenprefix
                                }
                        }
                }
        }
}

# }}}
# {{{
# ----------------------------------------------------------------------------
# mk_install_dir

proc ecos_install::mk_install_dir { } {
        if { $ecos_install::install_dir == "" } {
                # Ask for install dir

                # Choose an appropriate default
                if { ([file exists /opt/ecos] && [file writable /opt/ecos]) || \
                         (![file exists /opt/ecos] && [file writable /opt]) || \
                         (![file exists /opt] && [file writable /]) } {
                        set defaultdir "/opt/orocos"
                } else {
                        set defaultdir "$::env(HOME)/src"
                }
                ecos_install::report "---------------------------------------------------------"
                set status -1
                while { $status < 0 } {
                        puts -nonewline "\nPlease select a directory for installation\n\[Default $defaultdir\]: "
                        set status [gets stdin userin]
                        if { $status == 0 } {
                                set userin $defaultdir
                                set status 1
                        }
                        if { $status > 0 } {
                                set ecos_install::install_dir $userin
                        }
                }

                # Since they didn't set an install dir on the command line, they also
                # can't have passed in tools, but might still want them. Let's ask here
                # rather than later as we know the situation here.
                # select_gnu_tools
        }
        if { ![file exists $ecos_install::install_dir] } {
                report "Directory [file nativename $ecos_install::install_dir] does not exist... creating."
                file mkdir $ecos_install::install_dir
        } else {
                if { ![file isdir $ecos_install::install_dir] } {
                        fatal_error "$ecos_install::install_dir exists but is not a directory"
                }

                # It exists.... hmmm.... perhaps there's something installed already, so check
                # if we're going to overwrite now so that we don't have to complain more
                # obscurely later *after* downloading everything!
                if { $ecos_install::force == 0 } {
                        if { !$ecos_install::tools_only } {
                                # FIXME: when we support older versions this needs fixing too.
                                #set chosenecos [lindex $ecos_install::ecosreleases($ecos_install::os) 0]
                                set ecospkgdir [file join $ecos_install::install_dir [ lindex $ecos_install::chosenecos 2 ]]
                                if { [ file exists $ecospkgdir ] } {
                                        fatal_error "Orocos [lindex $ecos_install::chosenecos 0] already appears to be\ninstalled at $ecospkgdir.\nPlease either remove it or choose to overwrite it\nby invoking this installer with the -f option."
                                }
                        }
                        foreach prefix $ecos_install::tool_prefixes {
                                # FIXME: when we support older versions this needs fixing too.
                                foreach tool $ecos_install::toolchains($ecos_install::os) {
                                        if { [lindex $tool 0] == $prefix } {
                                                set toolbinpath [lindex $tool 2]
                                                set fullpath [ file join $ecos_install::install_dir $toolbinpath ]
                                                if { [ file exists $fullpath ] } {
                                                        fatal_error "The $prefix GNU tools already appear to be\ninstalled at $fullpath.\nPlease either remove them or choose to overwrite them\nby invoking this installer with the -f option."
                                                }
                                        }
                                }                                        
                        }
                }
        }
        report "Entering [ file nativename $ecos_install::install_dir ]"
        cd $ecos_install::install_dir
}

# }}}
# {{{
# ----------------------------------------------------------------------------
# get_ecos_install_data

proc ecos_install::get_ecos_install_data { } {
        report "Retrieving installer metadata information..."
        if { $ecos_install::mirror != "" && $ecos_install::metadata_url_supplied == 0 } {
                # If someone specifies a mirror and doesn't also give a config file we
                # change from the default config file location to be at the mirror base
            set ecos_install::metadata_url [ list [ ecos_install::file_or_url_join $ecos_install::mirror "orocos-install.db" ] ]
        }
        set tmpfile "/tmp/orocos-install.db.[pid]"
        if { [file exists $tmpfile] } {
                fatal_error "Temporary file $tmpfile already exists! Delete it"
        }
        # FIXME: potential security issue with tmp file and race condition.
        
        set status -1        
        for { set i 0 } { $i < [ llength $ecos_install::metadata_url ] } { incr i } {
                set status [ catch {
                        ecos_install::retrieve_url [ lindex $ecos_install::metadata_url $i ] 1 $tmpfile
                } message ]
                if { $status == 0 } {
                        break
                }
        }
        if { $status != 0 } {
                # Only the last error, but probably good enough
                file delete $tmpfile
                ecos_install::fatal_error "Failed to retrieve metadata:\n$message"
        }
#        set mfile [ecos_install::file_or_url_basename [ lindex $ecos_install::metadata_url $i ]]
        set mfile $tmpfile

        # Use safe TCL interpreter

        # proc used by safe interpreter to indicate file version expected by installer
        proc get_install_data_version { } { return 1 }
        proc set_ecosreleases { value } { array set ecos_install::ecosreleases $value }
        proc set_toolchains { value } { array set ecos_install::toolchains $value }
        proc set_ecosmirrors { value } { set ecos_install::ecosmirrors $value }

        # Create the parser, add the aliased commands.
        set parser [interp create -safe]
        $parser alias get_install_data_version ecos_install::get_install_data_version
        $parser alias set_ecosreleases ecos_install::set_ecosreleases
        $parser alias set_toolchains ecos_install::set_toolchains
        $parser alias set_ecosmirrors ecos_install::set_ecosmirrors

        set status [ catch {
                set fd [open $mfile r]
                set script [read $fd]
                close $fd
                $parser eval $script
        } message ]

        file delete $mfile

        if { $status != 0 } {
                ecos_install::fatal_error "parsing $mfile:\n$message"
        }

        # The interpreter and the aliased commands are no longer required.
        rename get_install_data_version {}
        rename set_ecosreleases {}
        rename set_toolchains {}
        rename set_ecosmirrors {}
        interp delete $parser
}

# }}}
# {{{
# ----------------------------------------------------------------------------
# determine_mirror
# In fact we always set a "mirror" even if it's the primary site!

proc ecos_install::determine_mirror { } {
        if { $ecos_install::mirror != "" } {
                # Already set. Nothing to do.
                return
        }
        ecos_install::report "---------------------------------------------------------"
        ecos_install::report "Available distribution sites:\n"
        for { set i 0 } { $i < [llength $ecos_install::ecosmirrors] } { incr i } {
                ecos_install::report "\[[expr ($i+1)]\]\t[lindex $ecos_install::ecosmirrors $i]"
        }
        set status -1
        set mnum 0

        while { $status <= 0 } {
                puts -nonewline "\nPlease select a distribution site: "
                set status [gets stdin mnum]
                if { $status > 0 } {
                        if { ($mnum < 1) || ($mnum > [llength $ecos_install::ecosmirrors]) } {
                                set status -1
                                ecos_install::report "Invalid selection!"
                        }
                }
        }
        set ecos_install::mirror [lindex $ecos_install::ecosmirrors [expr ($mnum-1)]]
}

# }}}
# {{{
# ----------------------------------------------------------------------------
# determine_release
# In fact we always set a "release" even if it's the primary site!

proc ecos_install::determine_release { } {
        if { $ecos_install::chosenecos != "" } {
                # Already set. Nothing to do.
                return
        }
        ecos_install::report "---------------------------------------------------------"
        ecos_install::report "Available Releases :\n"
        for { set i 0 } { $i < [llength $ecos_install::ecosreleases($ecos_install::os)] } { incr i } {
	    ecos_install::report "\[[expr ($i+1)]\]\t[ lindex [lindex $ecos_install::ecosreleases($ecos_install::os) $i] 0]"
        }
        set status -1
        set mnum 0

        while { $status <= 0 } {
                puts -nonewline "\nPlease select a release : "
                set status [gets stdin mnum]
                if { $status > 0 } {
                        if { ($mnum < 1) || ($mnum > [llength $ecos_install::ecosreleases($ecos_install::os)]) } {
                                set status -1
                                ecos_install::report "Invalid selection!"
                        }
                }
        }
    set ecos_install::chosenecos [lindex $ecos_install::ecosreleases($ecos_install::os) [expr ($mnum-1)]]
}

# }}}
# {{{
# ----------------------------------------------------------------------------
# get_ecos

proc ecos_install::get_ecos { } {
    # FIXME: support downloading older versions ; DONE
        report "Retrieving Orocos version [lindex $ecos_install::chosenecos 0]"

        if { $ecos_install::mirror == "" } {
                fatal_error "Distribution site or mirror unset!"
        }
        set url $ecos_install::mirror
        set url [ ecos_install::file_or_url_join $url [lindex $ecos_install::chosenecos 1]]
        
        ecos_install::retrieve_url $url 1 ""
        lappend ecos_install::tarballs_to_unpack [ecos_install::file_or_url_basename $url]
        set ecos_install::ecos_repo_suffix [lindex $ecos_install::chosenecos 2]
        set ecos_install::ecos_path_suffixes [lindex $ecos_install::chosenecos 3]
}


# }}}
# {{{
# ----------------------------------------------------------------------------
# get_tools

proc ecos_install::get_tools { } {

        foreach prefix $ecos_install::tool_prefixes {
                # FIXME: support downloading older versions
                report "Retrieving GNU tools for $prefix"

                if { $ecos_install::mirror == "" } {
                        fatal_error "Distribution site or mirror unset!"
                }
                set url $ecos_install::mirror
                set tooltarball ""
                foreach tool $ecos_install::toolchains($ecos_install::os) {
                        if { [lindex $tool 0] == $prefix } {
                                set tooltarball [lindex $tool 1]
                                set toolbinpath [lindex $tool 2]
                        }
                }
                if { $tooltarball == "" } {
                        fatal_error "Tools for \"$prefix\" not found"
                }
                set url [ ecos_install::file_or_url_join $url $tooltarball]
                
                ecos_install::retrieve_url $url 1 ""
                lappend ecos_install::tarballs_to_unpack [ecos_install::file_or_url_basename $url]
                lappend ecos_install::gnutool_path_suffixes $toolbinpath
        }
}


# }}}
# {{{
# ----------------------------------------------------------------------------
# unpack_tarballs

proc ecos_install::unpack_tarballs { } {
        if { $ecos_install::mirror == "" || [is_file_not_url $ecos_install::mirror] == 0 } {
                ecos_install::report "Downloads complete."
                ecos_install::report "If you wish to disconnect from the internet you may do so now."
        }
        foreach tb $ecos_install::tarballs_to_unpack {
                ecos_install::report "Unpacking $tb..."
                set tar_opts ""
                if { $ecos_install::force == 0 } {
                        append tar_opts "k"
                }
                if { $ecos_install::verbose != 0 } {
                        append tar_opts "v"
                }
                append tar_opts "vx"
                set tar_fd [ open "|bzip2 -d < $tb | tar $tar_opts |& cat -" "r" ]
                set tar_output ""
                while { -1 != [ gets $tar_fd line ] } {
                        if { $ecos_install::verbose != 0 } {
                                ecos_install::report $line
                        } else {
                                lappend tar_output $line
                        }
                }
                if { [catch "close $tar_fd" code] } {
                        # If we had been requested to be verbose, the tar output would already
                        # be visible. So if not, display the last bit since there was an error.
                        if { $ecos_install::verbose == 0 } {
                                ecos_install::report "*** tar returned error."
                                ecos_install::report "*** First five lines of error output believed to be:"
                                set outputlines 0
                                for { set i 0 } { $i < [ llength $tar_output ] } { incr i } {
                                        if { [ regexp -lineanchor -- "^tar:" [lindex $tar_output $i]] == 1 } {
                                                ecos_install::report [lindex $tar_output $i]
                                                incr outputlines
                                                if { $outputlines > 5 } {
                                                        break
                                                }
                                        }
                                }
                        }
                        fatal_error "tar unpack failed"
                }
                file delete $tb
                set tar_output ""
        }
}

# }}}
# {{{
# ----------------------------------------------------------------------------
# preconfigure

proc ecos_install::preconfigure { } {
    ecos_install::report "Preconfiguring the downloaded packages."
    # All packages are present
    # But we need to run :
    # cd packages; ./configure ; make db
    set idir $ecos_install::install_dir
    set pdir [ file join $idir $ecos_install::ecos_repo_suffix ]
    cd $pdir
    exec sh -c "./configure 2> config.errors"
    exec sh -c "make db 2> config.errors"
    file delete config.errors
}

# }}}
# {{{
# ----------------------------------------------------------------------------
# generate_ecos_script
# Generate a script that someone can use with "source" or "." to munge PATHs, etc.

proc ecos_install::make_file_executable { f } {
        if { $::tcl_platform(platform) == "unix" } {
                set perm [file attributes $f -permissions]
                set umaskstr [exec sh -c umask]
                scan $umaskstr "%o" umask
                set perm [expr ($perm|0111) & ~$umask]
                file attributes $f -permissions $perm
        }
}

proc ecos_install::generate_ecos_script { } {
#        if { $ecos_install::os == "windows" } {
#                # We generate/use a batch file
#                if { [file exists ecos.bat] } {
#                        ecos_install::report "Updating [file nativename [ file join [pwd] ecos.bat]]"
#                        set bat_fd [open "ecos.bat" "r+" ]
#                        set newbat_fd [open "ecos.bat.new" "w" ]
#                        while { -1 != [gets $bat_fd line] } {
#                                switch -regexp -- $line {
#                                        "ECOS_REPOSITORY" {
#                                                puts $newbat_fd "set ECOS_REPOSITORY="
#                                        }
#                                        default {
#                                                puts $newbat_fd $line
#                                        }
#                                }
#                        }
#                } else {
#                        ecos_install::report "Creating [file nativename [ file join [pwd] ecos.bat]]"
#                }
#        }
        set idir $ecos_install::install_dir
        if { $ecos_install::os == "windows" } {
                # Need to convert back to cygwin path. Maybe we should just store the cygwin path at the start?
                # But then we wouldn't able to accept non-cygwin paths as arguments.
                set idir [ exec -- cygpath -u $idir ]
        }
	cd $idir

        if { [file exists orocosenv.sh] } {
                ecos_install::report "Updating [ file nativename [ file join $idir orocosenv.sh ]]"
                set scr_fd [open "orocosenv.sh" "r" ]
                set newscr_fd [open "orocosenv.sh.new" "w" ]
                set toolpath_list [list]
                foreach toolpath $ecos_install::gnutool_path_suffixes {
                        lappend toolpath_list "PATH=[file join $idir $toolpath]:\$PATH ; export PATH"
                }
                set ecos_repository_set 0
                set ignore_lines 0
                while { -1 != [gets $scr_fd line] } {
                        switch -regexp -- $line {
                                "^ECOS_REPOSITORY=" {
                                        if { $ecos_install::ecos_repo_suffix != "" } {
                                                puts $newscr_fd "ECOS_REPOSITORY=[file join $idir $ecos_install::ecos_repo_suffix] ; export ECOS_REPOSITORY"
                                        } else {
                                                puts $newscr_fd $line
                                        }
                                        set ecos_repository_set 1
                                }
                                "^# Orocos paths - do not modify this line, it is used by the installer" {
                                        puts $newscr_fd $line
                                        if { $ecos_install::ecos_repo_suffix != "" } {
                                                set ignore_lines 1
                                                foreach ecospath $ecos_install::ecos_path_suffixes {
                                                        puts $newscr_fd "PATH=[file join $idir $ecospath]:\$PATH ; export PATH"
                                                }
                                        }
                                }
                                "^# End Orocos paths - do not modify this line, it is used by the installer" {
                                        puts $newscr_fd $line
                                        set ignore_lines 0
                                }
                                "^PATH=" {
                                        if { $ignore_lines == 0 } {
                                                # If the PATH command already exists and is the same as one we are about to set,
                                                # don't print the current line as it will get set at the end with the rest.
                                                if { [lsearch -exact $toolpath_list $line] == -1 } {
                                                        puts $newscr_fd $line
                                                }
                                        }
                                }
                                default {
                                        if { $ignore_lines == 0 } {
                                                puts $newscr_fd $line
                                        }
                                }
                        }
                }
                if { ($ecos_repository_set == 0) && ($ecos_install::ecos_repo_suffix != "") } {
                        puts $newscr_fd "ECOS_REPOSITORY=[file join $idir $ecos_install::ecos_repo_suffix] ; export ECOS_REPOSITORY"
                        foreach ecospath $ecos_install::ecos_path_suffixes {
                                puts $newscr_fd "PATH=[file join $idir $ecospath]:\$PATH ; export PATH"
                        }
                }
                foreach toolpath $ecos_install::gnutool_path_suffixes {
                        puts $newscr_fd "PATH=[file join $idir $toolpath]:\$PATH ; export PATH"
                }
                close $scr_fd
                close $newscr_fd
                make_file_executable "orocosenv.sh.new"
                file rename -force -- orocosenv.sh.new orocosenv.sh
        } else {
                ecos_install::report "Generating [file nativename [ file join $idir orocosenv.sh]]"
                set newscr_fd [open "orocosenv.sh" "w"]
                puts $newscr_fd "# Warning! This is a generated file used by the Orocos installer."
                puts $newscr_fd "# If you edit this file, you may break future upgrades using the installer.\n"
                if { $ecos_install::ecos_repo_suffix != "" } {
                        puts $newscr_fd "ECOS_REPOSITORY=[file join $idir $ecos_install::ecos_repo_suffix] ; export ECOS_REPOSITORY\n"
                        puts $newscr_fd "# Orocos paths - do not modify this line, it is used by the installer"
                        foreach ecospath $ecos_install::ecos_path_suffixes {
                                puts $newscr_fd "PATH=[file join $idir $ecospath]:\$PATH ; export PATH"
                        }
                        puts $newscr_fd "# End Orocos paths - do not modify this line, it is used by the installer\n"
                }
                foreach toolpath $ecos_install::gnutool_path_suffixes {
                        puts $newscr_fd "PATH=[file join $idir $toolpath]:\$PATH ; export PATH"
                }
                close $newscr_fd
                make_file_executable "orocosenv.sh"
        }
        if { [file exists orocosenv.csh] } {
                ecos_install::report "Updating [ file join $idir orocosenv.csh ]"
                set scr_fd [open "orocosenv.csh" "r" ]
                set newscr_fd [open "orocosenv.csh.new" "w" ]
                set toolpath_list [list]
                foreach toolpath $ecos_install::gnutool_path_suffixes {
                        lappend toolpath_list "setenv PATH [file join $idir $toolpath]:\$PATH"
                }
                set ecos_repository_set 0
                set ignore_lines 0
                while { -1 != [gets $scr_fd line] } {
                        switch -regexp -- $line {
                                "^setenv ECOS_REPOSITORY " {
                                        if { $ecos_install::ecos_repo_suffix != "" } {
                                                puts $newscr_fd "setenv ECOS_REPOSITORY [file join $idir $ecos_install::ecos_repo_suffix]"
                                        } else {
                                                puts $newscr_fd $line
                                        }
                                        set ecos_repository_set 1
                                }
                                "^# Orocos paths - do not modify this line, it is used by the installer" {
                                        puts $newscr_fd $line
                                        if { $ecos_install::ecos_repo_suffix != "" } {
                                                set ignore_lines 1
                                                foreach ecospath $ecos_install::ecos_path_suffixes {
                                                        puts $newscr_fd "setenv PATH [file join $idir $ecospath]:\$PATH"
                                                }
                                        }
                                }
                                "^# End Orocos paths - do not modify this line, it is used by the installer" {
                                        puts $newscr_fd $line
                                        set ignore_lines 0
                                }
                                "^setenv PATH " {
                                        if { $ignore_lines == 0 } {
                                                # If the PATH command already exists and is the same as one we are about to set,
                                                # don't print the current line as it will get set at the end with the rest.
                                                if { [lsearch -exact $toolpath_list $line] == -1 } {
                                                        puts $newscr_fd $line
                                                }
                                        }
                                }
                                default {
                                        if { $ignore_lines == 0 } {
                                                puts $newscr_fd $line
                                        }
                                }
                        }
                }
                if { ($ecos_repository_set == 0) && ($ecos_install::ecos_repo_suffix != "") } {
                        puts $newscr_fd "setenv ECOS_REPOSITORY [file join $idir $ecos_install::ecos_repo_suffix]"
                        foreach ecospath $ecos_install::ecos_path_suffixes {
                                puts $newscr_fd "setenv PATH [file join $idir $ecospath]:\$PATH"
                        }
                }
                foreach toolpath $ecos_install::gnutool_path_suffixes {
                        puts $newscr_fd "setenv PATH [file join $idir $toolpath]:\$PATH"
                }
                close $scr_fd
                close $newscr_fd
                make_file_executable "orocosenv.csh.new"
                file rename -force -- orocosenv.csh.new orocosenv.csh
        } else {
                ecos_install::report "Generating [file nativename [ file join $idir orocosenv.csh]]"
                set newscr_fd [open "orocosenv.csh" "w"]
                puts $newscr_fd "# Warning! This is a generated file used by the Orocos installer."
                puts $newscr_fd "# If you edit this file, you may break future upgrades using the installer.\n"
                if { $ecos_install::ecos_repo_suffix != "" } {
                        puts $newscr_fd "setenv ECOS_REPOSITORY [file join $idir $ecos_install::ecos_repo_suffix]\n"
                        puts $newscr_fd "# Orocos paths - do not modify this line, it is used by the installer"
                        foreach ecospath $ecos_install::ecos_path_suffixes {
                                puts $newscr_fd "setenv PATH [file join $idir $ecospath]:\$PATH"
                        }
                        puts $newscr_fd "# End Orocos paths - do not modify this line, it is used by the installer\n"
                }
                foreach toolpath $ecos_install::gnutool_path_suffixes {
                        puts $newscr_fd "setenv PATH [file join $idir $toolpath]:\$PATH"
                }
                close $newscr_fd
                make_file_executable "orocosenv.csh"
        }
        ecos_install::report "---------------------------------------------------------"
        ecos_install::report "In future, to establish the correct environment for Orocos,"
        ecos_install::report "run one of the following commands:"
        ecos_install::report "   . [file join $idir orocosenv.sh] \t\t(for sh/bash users); or"
        ecos_install::report "   source [file join $idir orocosenv.csh] \t(for csh/tcsh users)\n"
        ecos_install::report "It is recommended you append these commands to the end of your"
        ecos_install::report "shell startup files such as \$HOME/.profile or \$HOME/.login"
        ecos_install::report "---------------------------------------------------------"
}

# }}}
# {{{
# ----------------------------------------------------------------------------
# Main(). This code only runs if the script is being run stand-alone rather
# than as part of a larger application. The controlling predicate is the
# existence of the variable ecos_install_not_standalone which can be set by
# the containing program if any.
#

if { ! [info exists ecos_install_not_standalone] } {
        # catch any errors while processing the specified command
        if { [ catch {
        
                # Parse the arguments and set the global variables appropriately.
                ecos_install::parse_arguments $argv0 $argv
                
                ecos_install::report "Orocos installer v1.0 starting..."
                ecos_install::report "Written and maintained by Jonathan Larmour <jifl@eCosCentric.com>"
                ecos_install::report "Adapted for Orocos by Peter Soetens <peter.soetens@mech.kuleuven.ac.be>\n"
                ecos_install::get_ecos_install_data
                ecos_install::determine_mirror
		ecos_install::determine_release
                ecos_install::mk_install_dir
                #ecos_install::get_tools
                if { !$ecos_install::tools_only } {
                        ecos_install::get_ecos
                }
                ecos_install::unpack_tarballs
		ecos_install::preconfigure
                ecos_install::generate_ecos_script

        } error_message ] != 0 } { 

                # handle error message
                if { [ info exists gui_mode ] } {
                        return $error_message
                }
                ecos_install::fatal_error "*** orocos-install.tcl error: $error_message"
        }
        ecos_install::report "Installation complete!"
}

# }}}
# EOF orocos-install.tcl
