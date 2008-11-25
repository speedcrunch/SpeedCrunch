#  $Id: template.py 36 2007-08-05 15:03:34Z ogre.crewman $
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
This module contains the classes and functions necessary for dealing with
UT3 template files.
"""

__version__ = "$Revision: 36 $"


# modules
import logging, os, re, sys
# classes
from ut3.store import UT3Item, UT3Store
# functions
from ut3.support import print_error_message


# -----------------------------------------------------------------------------
# Classes
# -----------------------------------------------------------------------------

class UT3Template:
    """
    This class represents a UT3 template file.

    @author: Marco Wegner
    """


    # -------------------------------------------------------------------------
    # Public methods
    # -------------------------------------------------------------------------

    def __init__(self, name, delims):
        """
        Creates a new UT3 template file instance.

        @param name: The template file name.
        @param delims: The delimiters options as a dictionary.
        """

        # check if the template is readable
        if name is None or not os.access(name, os.R_OK):
            print_error_message("specified template file does not exist " \
                + "or is not readable")
            sys.exit( )

        self.__name = name
        self.__delims = delims

        return

    # -------------------------------------------------------------------------

    def parse(self):
        """
        Parses the template file and extracts a list of UT3 translation items
        as an UT3 translation store.

        @return: The translation store filled with the parsed entries.
        """

        tbase = os.path.basename(self.__name)

        ldelim = self.__delims['ldelim']
        rdelim = self.__delims['rdelim']

        pattern = _assemble_split_pattern(ldelim, rdelim)

        tstore = UT3Store( )

        inside = False    #  whether or not we're inside a translatable
        trstr = ""        #  the current translatable string

        for curl in self._get_file_lines( ):
            curl = curl.rstrip('\n')
            slist = re.split(pattern, curl)
            for item in slist:
                if item == ldelim:
                    # reset the translatable string
                    trstr = ""
                    # translatable: begin
                    inside = True
                elif item == rdelim:
                    # extract the comment from the translatable string
                    (trstr, comment) = self._extract_hint(trstr.strip( ))
                    # create new translation entry for this string
                    entry = UT3Item(trstr)
                    entry.add_source(tbase)
                    entry.comment = comment
                    tstore.append(entry)
                    # translatable: end
                    inside = False
                else:
                    # the current string is no delimiter, so it must either
                    # be a translatable (if inside) or not
                    if inside:
                        trstr = trstr + item + " "

        return tstore

    # -------------------------------------------------------------------------

    def convert(self, tstore, outfile):
        """
        Assembles a file from the original template file and a list of
        translations.

        @param tstore: The UT3 translation store.
        @param outfile: The output file name or None for writing to stdout.
        @return: The total number of applied messages and the number of
            untranslated messages for the specified template.
        """

        if not isinstance(tstore, UT3Store):
            raise TypeError

        lines = self._get_file_lines( )

        ldelim = self.__delims['ldelim']
        rdelim = self.__delims['rdelim']

        pattern = _assemble_split_pattern(ldelim, rdelim)

        trlines = []      #  the list keeping all the assembled and
                          #+ translated lines
        inside = False    #  whether or not we're inside a translatable
        trstr = ""        #  the current translatable string
        assembled = ""    #  the current assembled line from the HTML file
        num_msg = 0       #  the total number of applied messages
        num_untr = 0      #  the number of untranslated message used for
                          #+ the current file

        for curl in lines:
            curl = curl.rstrip('\n')
            slist = re.split(pattern, curl)
            for item in slist:
                if item == ldelim:
                    # reset the translatable string
                    trstr = ""
                    # translatable: begin
                    inside = True
                elif item == rdelim:
                    # strip the comment from the translatable string but
                    # ignore it
                    trstr = self._extract_hint(trstr.strip( ))[0]

                    entry = tstore.get(trstr)
                    if entry is not None and entry.translation is not None and entry.translation != "":
                        # we found the correct message and it is even
                        # translated :)
                        trlate = entry.translation
                    else:
                        # there was either no such entry or the entry was not
                        # translated
                        trlate = trstr
                        num_untr = num_untr + 1

                    assembled = assembled + trlate
                    inside = False
                    num_msg = num_msg + 1
                else:
                    if inside:
                        trstr = trstr + item + " "
                    else:
                        assembled = assembled + item

            if not inside:
                trlines.append(assembled)
                assembled = ""

        _write(trlines, outfile)

        return num_msg, num_untr


    # -------------------------------------------------------------------------
    # Helper methods
    # -------------------------------------------------------------------------

    def _get_file_lines(self):
        """
        Returns the list of lines read from a template file.

        @return: The list of lines.
        """

        # read the template file
        tmpl = file(self.__name, 'r')
        lines = tmpl.readlines( )
        tmpl.close( )

        msg = "read %i lines from the template file" % (len(lines))
        logging.info(msg)

        return lines

    # -------------------------------------------------------------------------

    def _extract_hint(self, trstr):
        """
        Extracts a hint/comment from an already extracted translatable string.
        The hint is optional and does not have to be specified at all. If it
        specified it must be located at the start of the translatable string.

        @param trstr: The complete translatable string which contain a hint
            (or possibly not).
        @return: Both the translatable string stripped of the hint and the
            hint itself as a tuple.
        """

        hint = None

        if self.__delims.has_key('comm_ldelim') and self.__delims.has_key('comm_rdelim'):

            ldelim = self.__delims['comm_ldelim']
            rdelim = self.__delims['comm_rdelim']

            if trstr.find(ldelim) == 0:
                epos = trstr.find(rdelim)

                hint = trstr[len(ldelim):epos].strip( )
                trstr = trstr[epos+len(rdelim):].strip( )

        return (trstr, hint)


# -----------------------------------------------------------------------------
# Helper functions
# -----------------------------------------------------------------------------

def _write(lines, outfile):
    """
    Writes a re-assembled file. This file is basically the original template
    with replaced (translated) strings.

    @param lines: The re-assembled (translated) lines from the original
        template.
    @param outfile: The output file name or None for writing to stdout.
    """

    if outfile is not None:
        # check if the file is writable
        if not os.access(os.path.dirname(outfile), os.W_OK):
            print_error_message("PO file not writable")
            sys.exit( )
        out = file(outfile, 'w')
    else:
        out = sys.stdout

    for curl in lines:
        out.write(curl + '\n')

    if not outfile is None:
        out.close( )

    return

# -----------------------------------------------------------------------------

def _assemble_split_pattern(left, right):
    """
    Assembles the regular expression pattern for splitting strings with
    delimiters.

    @param left: The left delimiter.
    @param right: The right delimiter.
    @return: The assembled regular expression pattern.
    """

    if left == right:
        pattern = left
    else:
        pattern = "(%s|%s)" % (left, right)

    logging.debug("message split pattern assembled: %s" % (pattern))

    return pattern
