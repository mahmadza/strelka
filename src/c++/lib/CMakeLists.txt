#
# Strelka - Small Variant Caller
# Copyright (c) 2009-2017 Illumina, Inc.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
#

################################################################################
##
## Configuration file for the lib subfolder
##
## author Ole Schulz-Trieglaff
##
################################################################################

##
## List of libraries
##
## note that order approximates dependency chain, where libraries are listed
## after libraries upon which they depend:
set (THIS_ALL_LIBRARIES blt_util common htsapi appstats options errorAnalysis calibration blt_common assembly alignment starling_common strelka_common)

##
## Build all the libraries for the project
## THIS_AVAILABLE_LIBRARIES is incrementally updated
##
set (THIS_AVAILABLE_LIBRARIES "")
foreach (THIS_LIB_DIR ${THIS_ALL_LIBRARIES})
    add_subdirectory(${THIS_LIB_DIR})

    set(THIS_AVAILABLE_LIBRARIES ${THIS_PROJECT_NAME}_${THIS_LIB_DIR} ${THIS_AVAILABLE_LIBRARIES})
endforeach ()

set (THIS_AVAILABLE_LIBRARIES ${THIS_AVAILABLE_LIBRARIES} PARENT_SCOPE)


#
# handle applications separately
#
add_subdirectory(applications)

