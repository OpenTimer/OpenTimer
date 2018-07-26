#!/bin/sh

################################################################################
#                       >  COPYRIGHT NOTICE  <
#
#                    Copyright 2011 Synopsys, Inc.
#
#
################################################################################
#
# Title        : SDC-parser command line application
#
################################################################################\
exec tclsh "$0" "$@"

source [file join [file dirname [info script]] sdcparsercore.tcl]

proc usage {} {
global argv0
return "
Usage:
$argv0
<filename> \[filename \[...\]\] - source file name(s)
-f <filename>               - read command line arguments from file
-d<debug_level>             - set debug level
    -d0 - no error messages, quiet mode
    -d1 - brief error messages (default)
    -d2 - extended error messages
-l <log_filename>           - set log file name
-v <sdc_version>            - set default sdc version
-eda <eda_source_file>      - file name which contains tcl source of eda tool
"
}

#-------------------------------------------------------------------------------
# Function       :  process_command_line
# Description    :  Analize command line
# Paremeters     :  argv        - list of command line parameters
# Return         :  Result data structure
#-------------------------------------------------------------------------------
proc process_command_line {argv {startdir ""}} {
    if {$argv==""} {
        puts [usage]
        exit
    } else {
        set argwait ""
        set files ""
        foreach arg $argv {
            switch -- $argwait {
                f {
                    set argwait ""
                    if {[catch {set fname [::open $arg]}]!=0} {
                        puts stderr "Can't open file: $arg"
                    } else {
                        while {![eof $fname]} {
                            append data "[gets $fname] "
                        }
                        catch {close $fname}
                        set lparams [split $data]
                        while {[set pos [lsearch $lparams ""]]!=-1} {
                            set lparams [lreplace $lparams $pos $pos]
                        }
                        set files [concat $files [process_command_line \
                                   $lparams [file dirname $arg]]]
                    }
                }

                e {
                    set argwait ""
                    catch {source $arg}
                }

                l {
                    set argwait ""
                    sdc::set_log_file $arg
                    catch {file delete $arg}
                }

                v {
                    set argwait ""
                    sdc::set_version $arg
                }

                default {
                    switch -glob -- $arg {
                        -f {
                            set argwait f
                            continue
                        }

                        -l {
                            set argwait l
                            continue
                        }

                        -d[0-2] {
                            sdc::set_debug_level [string index $arg 2]
                        }

                        -eda {
                            set argwait e
                            continue
                        }

                        -v {
                            set argwait v
                            continue
                        }

                        -help {
                            puts [usage]
                            continue
                        }

                        default {
                            lappend files [file join $startdir $arg]
                        }
                    }
                }
            }
        }
    }
    return $files
} 

#-------------------------------------------------------------------------------
#   Main section
#-------------------------------------------------------------------------------

set filelist [process_command_line $argv]

if {$filelist!=""} {
    foreach file $filelist {
        sdc::parse_file $file
    }

    #if {$sdc::debuglevel>0} {
        if {$sdc::errorstotal} {
            puts "Found $sdc::errorstotal error(s) total."
        } else {
            puts "No errors found."
        }
    #}
} else {
    puts "No files found."
    puts [usage]
}
