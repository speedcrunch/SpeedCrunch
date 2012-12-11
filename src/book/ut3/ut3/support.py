#  $Id: support.py 36 2007-08-05 15:03:34Z ogre.crewman $
#
#  This file is part of the UT3 project
#
#  Copyright (C) 2007 Marco Wegner <ogre.crewman@googlemail.com>
#
#  This program is free software; you can redistribute it and/or
#  modify it under the terms of the GNU General Public License
#  as published by the Free Software Foundation; either version 2
#  of the License, or (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; see the file COPYING.  If not, write to
#  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
#  Boston, MA 02110-1301, USA.


"""
This module contains support functions.
"""

__version__ = "$Revision: 36 $"


# modules
import sys
# functions
from optparse import make_option


# -----------------------------------------------------------------------------
# Global functions
# -----------------------------------------------------------------------------

def get_global_options( ):
    """
    Creates a list of global options (those common to all scripts).

    @return: The option list.
    """

    option_list = [
       make_option("-q", "--quiet", action="store_false", dest="verbose", default=True, help="don't print status messages"),
       make_option("-d", "--delimiter", dest="delim", help="specify the delimiter string"),
       make_option("-l", "--left-delim", dest="ldelim", help="specify the left delimiter string"),
       make_option("-r", "--right-delim", dest="rdelim", help="specify the right delimiter string"),
       make_option("-c", "--comment-delimiter", dest="commdelim", help="specify the delimiter for comments"),
       make_option("-s", "--start-comment", dest="startcomm", help="specify the left delimiter for comments"),
       make_option("-e", "--end-comment", dest="endcomm", help="specify the left delimiter for comments")
   ]

    return option_list

# -----------------------------------------------------------------------------

def print_error_message(message):
    """
    Print an error message in a standardized way.

    @param message: The actual error message.
    """

    sys.stderr.write("WARNING: %s\n" % (message))

    return

# -----------------------------------------------------------------------------

def print_warning_message(message):
    """
    Print a warning message in a standardized way.

    @param message: The actual warning message.
    """

    sys.stderr.write("WARNING: %s\n" % (message))

    return

# -----------------------------------------------------------------------------

def extract_delimiters(options):
    """
    Checks the parser options for sufficient definition of delimiters and then
    extracts these delimiters.

    @param options: The options object as created by the OptionParser.
    @return: A dictionary containing the extracted delimiters.
    """

    delim_dict = {}

    _extract_string_delimiters(options, delim_dict)
    _extract_comment_delimiters(options, delim_dict)

    return delim_dict

# -----------------------------------------------------------------------------

def _extract_string_delimiters(options, delim_dict):
    """
    Extracts the delimiters for translatable strings.

    @param options: The instance containing the options.
    @param delim_dict: The current dictionary for adding the new delimiters.
    """

    # check if any delimiters are defined
    if options.delim is None and options.ldelim is None and options.rdelim is None:
        print_error_message("no delimiters defined!")
        sys.exit( )

    # check for conflicting delimiter options
    if options.delim is not None:
        # make sure that neither ldelim nor rdelim are specified if delim
        # is specified at the same time
        if options.ldelim is not None or options.rdelim is not None:
            print_error_message("delim and ldelim/rdelim can't be " \
                + "specified at the same time!")
            sys.exit( )
        delim_dict['ldelim'] = options.delim
        delim_dict['rdelim'] = options.delim
    else:
        # if delim is not defined then make sure that both ldelim and relim
        # are defined
        if options.ldelim is None or options.rdelim is None:
            print_error_message("both ldelim and rdelim must be specified!")
            sys.exit( )
        delim_dict['ldelim'] = options.ldelim
        delim_dict['rdelim'] = options.rdelim

    return

# -----------------------------------------------------------------------------

def _extract_comment_delimiters(options, delim_dict):
    """
    Extracts the delimiters for comments/hints.

    @param options: The instance containing the options.
    @param delim_dict: The current dictionary for adding the new delimiters.
    """

    if options.commdelim is None and options.startcomm is None and options.endcomm is None:
        # no hint/comment delimiters defined
        return

    if options.commdelim is not None:
        if options.startcomm is not None or options.endcomm is not None:
            print_error_message("commdelim and startcomm/endcomm can't be " \
                + "specified at the same time!")
            sys.exit( )
        delim_dict['comm_ldelim'] = options.commdelim
        delim_dict['comm_rdelim'] = options.commdelim
    else:
        if (options.startcomm is not None and options.endcomm is None) or (options.startcomm is None and options.endcomm is not None):
            print_error_message("both start and end delimiters must be defined for comments")
            sys.exit( )
        delim_dict['comm_ldelim'] = options.startcomm
        delim_dict['comm_rdelim'] = options.endcomm

    return
