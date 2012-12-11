#  $Id: pofile.py 39 2007-08-06 10:48:59Z ogre.crewman $
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
This module contains classes and functions for dealing with PO and POT files.
"""

__version__ = "$Revision: 39 $"


# -----------------------------------------------------------------------------
# Import declarations
# -----------------------------------------------------------------------------

# modules
import logging, os, re, sys
from ut3 import store
# classes
from ut3.store import UT3Item, UT3Store
# functions
from time import gmtime, strftime
from ut3.support import print_error_message


# -----------------------------------------------------------------------------
# Global constants
# -----------------------------------------------------------------------------

LINE_UNKNOWN = -1
LINE_EMPTY   =  0
LINE_COMMENT =  1
LINE_MSGID   =  2
LINE_MSGSTR  =  3


# -----------------------------------------------------------------------------
# Classes
# -----------------------------------------------------------------------------

class Ut3PoFile:
    """
    This class represents a PO or POT file.

    @author: Marco Wegner
    """


    # -------------------------------------------------------------------------
    # Public methods
    # -------------------------------------------------------------------------

    def __init__(self, name):
        """
        Creates a new PO or POT file instance.
        """

        self.__name = name

        return

    # -------------------------------------------------------------------------

    def parse(self):
        """
        Parse the PO file.

        @return: The UT3 translation store filled with the parsed entries.
        """

        lines = self._get_file_lines( )

        message, translation     = None, None
        comment, status, sources = None, None, None
        temp_msgid, temp_msgstr  = None, None
        previous, current        = None, None

        tstore = UT3Store( )

        for curl in lines:

            curl = curl.strip( )

            if len(curl) == 0:
                current = LINE_EMPTY
            elif curl[0] == '#':
                current = LINE_COMMENT
                status, comment, sources = \
                    _extract_comment_values(curl, status, comment, sources)
            else:
                keyword, message = _parse_line(curl)
                if keyword is not None:
                    if keyword == "msgid":
                        current = LINE_MSGID
                        # and now initialise them for later use
                        temp_msgid = message
                        temp_msgstr = ""
                    elif keyword == "msgstr":
                        current = LINE_MSGSTR
                        temp_msgstr = message
                    else:
                        current = LINE_UNKNOWN
                        logging.error("unknown keyword: %s" % (keyword))
                else:
                    if message is not None:
                        if current == LINE_MSGID:
                            temp_msgid = temp_msgid + message
                        elif current == LINE_MSGSTR:
                            temp_msgstr = temp_msgstr + message
                        else:
                            logging.error("unknown mode")

            if previous == LINE_MSGSTR and current != LINE_MSGSTR:
                # we're not in msgstr mode anymore --> save the current entry
                entry = _make_item(message, translation, \
                    sources, comment, status)
                if entry is not None:
                    tstore.append(entry)

                # reset the item values
                message, translation     = None, None
                comment, status, sources = None, None, None

            # save msgid and msgstr for storing them later
            message = temp_msgid
            translation = temp_msgstr
            # save line state
            previous = current

        # finally append the last pair
        if previous == LINE_MSGSTR:
            entry = _make_item(message, translation, sources, comment, status)
            if entry is not None:
                tstore.append(entry)

        return tstore

    # -------------------------------------------------------------------------

    def write(self, tstore, pot_mode = False):
        """
        Actually writes the POT file.

        @param tstore: The UT3 translation store.
        @param pot_mode: If true then write a POT file.
        """

        if not isinstance(tstore, UT3Store):
            raise TypeError

        if self.__name is not None:
            # check if the file is writable
            if not os.access(os.path.dirname(self.__name), os.W_OK):
                print_error_message("PO file not writable")
                sys.exit( )
            out = file(self.__name, 'w')
        else:
            out = sys.stdout

        self._write_header(out, pot_mode)

        for ent in tstore.get_all( ):
            out.write('\n')

            if ent.comment is not None:
                out.write('#. %s\n' % (ent.comment))

            sources = ent.get_sources( )
            if len(sources) > 1 or (len(sources) == 1 and not "" in sources):
                out.write('#: %s\n' % (", ".join(sources)))

            if ent.translation is not None and ent.translation != "" and ent.status == store.STATUS_UNFINISHED:
                out.write('#, fuzzy\n')

            out.write('msgid \"%s\"\n' % (ent.message.replace('\"', '\\"')))
            tr = ""
            if ent.translation is not None:
                tr = ent.translation.replace('\"', '\\"')
            out.write('msgstr \"%s\"\n' % (tr.encode("utf-8")))

        if not self.__name is None:
            out.close( )

        return

    # -------------------------------------------------------------------------

    def _get_file_lines(self):
        """
        Extract the lines of the PO file and return it as string list.

        @return: The file's lines as string list
        """

        # check if the PO/POT file is readable
        if self.__name is None or not os.access(self.__name, os.R_OK):
            print_error_message("PO file does not exist or is not readable")
            sys.exit( )

        # read the PO file
        pofile = file(self.__name, 'r')
        lines = pofile.readlines( )
        pofile.close( )

        return lines

    # -------------------------------------------------------------------------

    def _write_header(self, out, pot_mode = False):
        """
        Write a PO or POT file's header.

        @param out: The handle to the file.
        @param pot_mode: If true then write a POT file.
        """

        if pot_mode:
            out.write('# This template has been generated by template2pot.py which is\n')
            out.write('# part of the Universal Template Translation Tools (UT3)\n')
            out.write('#\n')
            out.write('#, fuzzy\n')

        out.write('msgid \"\"\n')
        out.write('msgstr \"\"\n')

        package = 'PACKAGE VERSION'
        if not (pot_mode or self.__name is None):
            pname = os.path.basename(self.__name)
            wlist = re.split('\.po[t]?$', pname)
            if len(wlist) > 0 and len(wlist[0]) > 0:
                package = wlist[0]
        out.write('\"Project-Id-Version: %s\\n\"\n' % (package))

        out.write('\"Report-Msgid-Bugs-To: \\n\"\n')

        dstr = strftime('%Y-%m-%d %H:%M+0000', gmtime( ))
        if pot_mode:
            out.write('\"POT-Creation-Date: %s\\n\"\n' % (dstr))
            out.write('\"PO-Revision-Date: YEAR-MO-DA HO:MI+ZONE\\n\"\n')
        else:
            out.write('\"POT-Creation-Date: YEAR-MO-DA HO:MI+ZONE\\n\"\n')
            out.write('\"PO-Revision-Date: %s\\n\"\n' % (dstr))

        out.write('\"Last-Translator: FULL NAME <EMAIL@ADDRESS>\\n\"\n')
        out.write('\"Language-Team: LANGUAGE <LL@li.org>\\n\"\n')
        out.write('\"MIME-Version: 1.0\\n\"\n')

        charset = 'CHARSET'
        if not pot_mode:
            charset = 'UTF-8'
        out.write('\"Content-Type: text/plain; charset=%s\\n\"\n' % (charset))

        out.write('\"Content-Transfer-Encoding: 8bit\\n\"\n')

        return


# -----------------------------------------------------------------------------
# Helper functions
# -----------------------------------------------------------------------------

def _parse_line(line):
    """
    Parses a single (non-comment and non-empty) line from the PO file and
    returns the keyword (if any) and message text for this line.

    @param line: The line to be parsed.
    @return: The extracted values for the keyword (or None for no keyword)
        and message text (or None in case of comments or empty lines)
        for the current line.
    """

    line = line.strip( )

    keyword = None

    spos = line.find('\"')
    if spos > 0:
        # the quote character is preceded by a keyword here
        pos = line.find(' ')
        keyword = line[0:pos]

    message = line[spos+1:-1]

    return keyword, message

# -----------------------------------------------------------------------------

def _extract_comment_values(line, status, comment, sources):
    """
    Extract the comments from the current line but change only the relevant 
    variable and keep the others.
    
    @param line: The current line.
    @param status: The current value of the translation status.
    @param comment: The current value of the translation comment.
    @param sources: The current value of the string sources.
    @return: The new values for status, comment and sources.
    """
    
    line = line.strip( )

    if len(line) > 1:
        if line[1] == ':':
            sources = re.split(',[\s]*', line[2:].strip( ))
        elif line[1] == '.':
            comment = line[2:].strip( )
        elif line[1] == ',':
            flags = re.split(',[\s]*', line[2:].strip( ))
            if not 'fuzzy' in flags:
                status = store.STATUS_COMPLETE
            
    return status, comment, sources

# -----------------------------------------------------------------------------

def _make_item(message, translation, sources, comment, status):
    """
    Creates a single UT3 translation item from a parsed entry in a PO file.

    @param message: The original (translatable) string.
    @param translation: The translation of the original string.
    @param sources: The source files (references) for this string.
    @param comment: An optional comment (or hint).
    @param status: The status of the translation (complete or unfinished).
    @return: The generated UT3 translation entry or None if the translatable
        string is empty or even None.
    """

    entry = None
    if (message is not None) and (message != "") and (translation is not None):
        # both the message and its translation are escaped in the PO file;
        # we need to undo this before storing the strings in the
        # translation entry
        entry = UT3Item(message.replace('\\"', '\"'))
        entry.translation = translation.replace('\\"', '\"')
        entry.extend_sources(sources)
        entry.comment = comment
        if status is None:
            status = store.STATUS_UNFINISHED
            if translation != "":
                status = store.STATUS_COMPLETE
        entry.status = status

    return entry
