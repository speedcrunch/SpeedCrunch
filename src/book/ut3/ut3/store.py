#  $Id: store.py 39 2007-08-06 10:48:59Z ogre.crewman $
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
This module contains the classes necessary for intermediate storing of
translation items and their properties.
"""

__version__ = "$Revision: 39 $"


# modules
import types


# -----------------------------------------------------------------------------
# Global constants
# -----------------------------------------------------------------------------

STATUS_COMPLETE   = 0
STATUS_UNFINISHED = 1

# -----------------------------------------------------------------------------
# Classes
# -----------------------------------------------------------------------------

class UT3Item:
    """
    This class represents a single translation item.

    @author: Marco Wegner
    """

    STATUS_COMPLETE = 0
    STATUS_UNFINISHED = 1

    def __init__(self, msg):
        """
        Creates a new translation item.

        @param msg: The translatable string.
        """

        self.message = msg
        self.translation = None
        self.status = STATUS_UNFINISHED
        self.hint = None
        self.__sources = set( )

        return

    # -------------------------------------------------------------------------

    def __setattr__(self, name, value):
        """
        Checks types and values for the attributes for validity.

        @param name: The attribute's name.
        @param value: The attribute's value.
        """

        if name == 'message' and (value is None or type(value) not in types.StringTypes):
            raise TypeError
        if name == 'translation' and not (value is None or type(value) in types.StringTypes):
            raise TypeError
        if name == 'comment' and not (value is None or type(value) in types.StringTypes):
            raise TypeError
        if name == 'status' and not (value == STATUS_COMPLETE or value == STATUS_UNFINISHED):
            raise ValueError
        self.__dict__[name] = value

        return

    # -------------------------------------------------------------------------

    def __eq__(self, other):
        """
        Compares this entry and the specified other for equality. Two UT3
        items are considered equal if their (original) translatable strings
        are the same.

        @param other: The other object to compare.
        @return: True if both objects are UT3 items and have the same message
            string, otherwise False.
        """

        return isinstance(other, UT3Item) and self.message == other.message

    # -------------------------------------------------------------------------

    def add_source(self, source):
        """
        Adds a single source file name to the current list of this entry's
        source files. The source is not added if it is already contained in
        the list of sources.

        @param source: The source to add.
        """

        if source is not None:
            self.__sources.add(source.strip( ))
        return

    # -------------------------------------------------------------------------

    def extend_sources(self, source_list):
        """
        Extends the list of the entry's source file names by the specified
        list of sources. Duplicate entries are removed from the resulting
        list.

        @param source_list: The list of sources
        """

        if source_list is not None:
            for source in source_list:
                self.add_source(source)
            
        return

    # -------------------------------------------------------------------------

    def get_sources(self):
        """
        Return a list of the entry's source file names.

        @return: The list of source files.
        """

        return self.__sources

    # -------------------------------------------------------------------------

    def has_source(self, source):
        """
        Checks if the current entry has the specified source among its list of
        source file names.

        @param source: The source file name.
        @return: True if the source is among the entry's sources, else False.
        """

        return source.strip( ) in self.get_sources( )

# -----------------------------------------------------------------------------

class UT3Store:
    """
    This class represents a store of UT3 translation entries.

    @author: Marco Wegner
    """

    def __init__(self):
        """
        Creates a new UT3 translation store.
        """

        self.__entries = []
        self.__sources = set( )

        return

    # -------------------------------------------------------------------------

    def extend(self, other):
        """
        Extend the currenty list of entries by the entries from another store.

        @param other: The other UT3 translation store.
        """

        if not isinstance(other, UT3Store):
            raise TypeError

        self.append_entries(other.get_all( ))

        return

    # -------------------------------------------------------------------------

    def append(self, entry):
        """
        Updates or adds a single entry. If no entry with the same message
        exists in the list, then the complete entry is added to the list.
        Otherwise only the source file names and the hint are updated.

        @param entry: A translatable entry.
        """

        if not isinstance(entry, UT3Item):
            raise TypeError

        contained = False
        for ent in self.__entries:
            if ent == entry:
                sources = entry.get_sources( )
                ent.extend_sources(sources)
                self.__sources = self.__sources.union(set(sources))
                # only copy the translation over if ones wasn't set yet
                if ent.translation == None:
                    ent.translation = entry.translation
                # only copy the comment over if ones wasn't set yet
                if ent.comment == None:
                    ent.comment = entry.comment
                contained = True
                break

        if not contained:
            self.__entries.append(entry)
            self.__sources = self.__sources.union(entry.get_sources( ))

        return

    # -------------------------------------------------------------------------

    def append_entries(self, entry_list):
        """
        Appends a list of translation entries.

        @param entry_list: The list of entries.
        """

        for entry in entry_list:
            self.append(entry)

        return

    # -------------------------------------------------------------------------

    def get_sources(self):
        """
        Return a list of source file names for all the currently stored
        entries.

        @return: The list of source names.
        """

        return self.__sources

    # -------------------------------------------------------------------------

    def get(self, message):
        """
        Return a single entry with the specified original message.

        @param message: The original (translatable) string.
        @return: The found entry or None if no such entry exists in the store.
        """

        entry = None
        for cur_ent in self.__entries:
            if cur_ent.message == message:
                entry = cur_ent
                break
        return entry

    # -------------------------------------------------------------------------

    def get_all(self):
        """
        Return a list of all entries currently in the store.

        @return: The list of all entries.
        """

        return self.__entries

    # -------------------------------------------------------------------------

    def get_unfinished(self):
        """
        Return all unfinished entries.

        @return: The list of unfinished entries.
        """

        return [ent for ent in self.__entries if ent.status == STATUS_UNFINISHED]

    # -------------------------------------------------------------------------

    def get_by_source(self, source):
        """
        Return all entries which have the specified source among their list
        of sources.

        @param source: The source name.
        @return: The list of entries.
        """
        
        if source == "":
            return [ent for ent in self.__entries if len(ent.get_sources( )) == 0]

        return [ent for ent in self.__entries if ent.has_source(source)]

    # -------------------------------------------------------------------------

    def get_unfinished_by_source(self, source):
        """
        Return all unfinished entries which have the specified source among
        their list of sources.

        @param source: The source name.
        @return: The list of unfinished entries.
        """

        if source == "":
            return [ent for ent in self.get_unfinished( ) if len(ent.get_sources( )) == 0]

        return [ent for ent in self.get_unfinished( ) if ent.has_source(source)]
