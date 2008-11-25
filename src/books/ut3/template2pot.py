#! /usr/bin/env python

#  $Id: template2pot.py 37 2007-08-05 21:30:24Z ogre.crewman $
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


__version__ = "$Revision: 37 $"


# -----------------------------------------------------------------------------
# Import declarations
# -----------------------------------------------------------------------------

# modules
import os, sys
from ut3 import support
# classes
from optparse import OptionParser
from ut3.pofile import Ut3PoFile
from ut3.store import UT3Store
from ut3.template import UT3Template
# functions
from optparse import make_option


# -----------------------------------------------------------------------------
# Global functions
# -----------------------------------------------------------------------------

def main( ):
    """
    The main function.

    @author: Marco Wegner
    """

    usage = "usage: %prog [options] template1 [template2, ...]"
    parser = OptionParser(usage, conflict_handler="resolve")
    parser.add_options(support.get_global_options( ))
    parser.add_options(_get_local_options( ))

    (options, args) = parser.parse_args( )

    # the script has been called without any options or arguments
    if len(sys.argv) == 1:
        parser.print_help( )
        sys.exit( )

    delimiters = support.extract_delimiters(options)

    # check if the template file is defined
    if len(args) == 0:
        support.print_error_message("no template file(s) specified")
        sys.exit( )

    if options.verbose:
        print "\nExtracting translatable strings from the template files:"

    super_store = UT3Store( )

    count = 0
    for tmpl_file in args:
        tmpl = UT3Template(tmpl_file, delimiters)
        tmpl_store = tmpl.parse( )
        num_rem = len(tmpl_store.get_all( ))
        count = count + num_rem

        super_store.extend(tmpl_store)

        if options.verbose:
            tname = os.path.basename(tmpl_file)
            print " * " + tname + ": " + str(num_rem) + " messages"

    if options.verbose:
        num_rem = len(super_store.get_all( ))
        print "\nHandling duplicates: removing %i strings" % (count - num_rem)
        print "\nRemaining to be written: %i strings" % (num_rem)
        print "\nWriting the template...\n"

    pofile = Ut3PoFile(options.outfile)
    pofile.write(super_store, True)

    if options.verbose:
        print "\nDone."

    return


# -----------------------------------------------------------------------------
# Helper functions
# -----------------------------------------------------------------------------

def _get_local_options( ):
    """
    Returns a list of options which are only used in this script.

    @return: The option list.
    """

    option_list = [
       make_option("-o", "--output", dest="outfile", help="specify the output file")
    ]

    return option_list

# -----------------------------------------------------------------------------

if __name__ == "__main__":
    main( )
