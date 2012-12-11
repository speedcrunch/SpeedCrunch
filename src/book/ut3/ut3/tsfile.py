#  $Id: tsfile.py 40 2007-08-06 11:18:04Z ogre.crewman $
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
This module contains classes and functions necessary for parsing and writing
Qt's TS files.
"""

__version__ = "$Revision: 40 $"


# -----------------------------------------------------------------------------
# Import statements
# -----------------------------------------------------------------------------

# modules
import os, sys, xml.dom
from xml.dom import minidom
from ut3 import store, support
# classes
from ut3.store import UT3Item, UT3Store
# functions
from xml.dom.ext import PrettyPrint


# -----------------------------------------------------------------------------
# Classes
# -----------------------------------------------------------------------------

class Ut3TsFile:
    """
    This class represents a Qt TS file.

    @author: Marco Wegner
    """

    # -------------------------------------------------------------------------
    # Public methods
    # -------------------------------------------------------------------------

    def __init__(self, name):
        """
        Creates a new TS file instance.

        @param name: The TS file name or None for writing the file to stdout.
        """

        self.__name = name

        return

    # -------------------------------------------------------------------------

    def parse(self):
        """
        Parse the TS file.

        @return: A translation entry store containing the parsed entries.
        """

        # check if the PO/POT file is readable
        if self.__name is None or not os.access(self.__name, os.R_OK):
            support.print_error_message("TS file does not exist or is not readable")
            sys.exit( )

        doc = minidom.parse(self.__name).documentElement

        tstore = UT3Store( )

        context_elems = doc.getElementsByTagName('context')
        entry_list = _handle_context_elements(context_elems)
        tstore.append_entries(entry_list)

        doc.unlink( )

        return tstore

    # -------------------------------------------------------------------------

    def write(self, tstore):
        """
        Writes a Qt TS file.

        @param tstore: The UT3 translation store with the items to be written.
        """

        if not isinstance(tstore, UT3Store):
            raise TypeError

        if self.__name is not None:
            # check if the file is writable
            if not os.access(os.path.dirname(self.__name), os.W_OK):
                support.print_error_message("PO file not writable")
                sys.exit( )
            out = file(self.__name, 'w')
        else:
            out = sys.stdout

        impl = xml.dom.getDOMImplementation( )
        doctype = impl.createDocumentType('TS', None, None)
        doc = impl.createDocument(None, "TS", doctype)
        root = doc.documentElement

        source_list = tstore.get_sources( )
        if len(source_list) == 0:
            context_entries = tstore.get_by_source("")
            root.appendChild(_generate_context_element(doc, "", context_entries))
        else:
            for context_name in source_list:
                context_entries = tstore.get_by_source(context_name)
                root.appendChild(_generate_context_element(doc, context_name, context_entries))

        PrettyPrint(root, out)

        if not self.__name is None:
            out.close( )

        doc.unlink( )

        return


# -----------------------------------------------------------------------------
# Helper functions
# -----------------------------------------------------------------------------

def _handle_context_elements(elem_list):
    """
    Creates a list of UT3 translation items from a list of context elements.

    @param elem_list: The list of context elements.
    @return: The list of generated UT3 translation items.
    """

    entry_list = []
    for elem in elem_list:
        name_elem = elem.getElementsByTagName('name')[0]
        name = _handle_name_element(name_elem)
        message_elems = elem.getElementsByTagName('message')
        entry_list.extend(_handle_message_elements(message_elems, name))
    return entry_list

# -----------------------------------------------------------------------------

def _handle_name_element(elem):
    """
    Extracts the context name from a name element.

    @param elem: The name element.
    @return: The extracted context name.
    """

    return _get_node_text(elem.childNodes)

# -----------------------------------------------------------------------------

def _handle_message_elements(elem_list, context):
    """
    Creates a list of UT3 translation items from a list of message elements
    belonging to the specified context.

    @param elem_list: The list of message elements.
    @param context: The context of the message elements.
    @return: The list of generated UT3 translation items.
    """

    entry_list = []
    for elem in elem_list:
        entry = _make_entry(elem, context)
        entry_list.append(entry)
    return entry_list

# -----------------------------------------------------------------------------

def _make_entry(message, context):
    """
    Creates a UT3 translation item from a message element and the context this
    message element belongs to.

    @param message: The message element.
    @param context: The context of the message element.
    @return: The generated UT3 translation item.
    """

    source = _extract_source(message)
    entry = UT3Item(source)
    translation, status = _extract_translation(message)
    entry.translation = translation
    entry.status = status
    entry.comment = _extract_comment(message)
    entry.add_source(context)
    return entry

# -----------------------------------------------------------------------------

def _extract_source(message):
    """
    Extracts a source message from a message element.

    @param message: The current message element.
    @return: The extracted source message text.
    """

    elem = message.getElementsByTagName('source')[0]
    return _get_node_text(elem.childNodes)

# -----------------------------------------------------------------------------

def _extract_translation(message):
    """
    Extracts a translation from a message element.

    @param message: The current message element.
    @return: The extracted translation text and the status of the translation.
    """

    elem = message.getElementsByTagName('translation')[0]
    status = store.STATUS_COMPLETE
    if elem.hasAttribute('type') and elem.getAttribute('type') == "unfinished":
        status = store.STATUS_UNFINISHED
    translation = _get_node_text(elem.childNodes)
    return translation, status

# -----------------------------------------------------------------------------

def _extract_comment(message):
    """
    Extracts a comment from a message element.

    @param message: The current message element.
    @return: The extracted comment text.
    """

    comment = None
    elem_list = message.getElementsByTagName('comment')
    if len(elem_list) > 0:
        elem = elem_list[0]
        comment = _get_node_text(elem.childNodes)
    return comment

# -----------------------------------------------------------------------------

def _get_node_text(node_list):
    """
    Extracts the complete node text from a list of nodes.

    @param node_list: The list of nodes.
    @return: The assembled text.
    """

    text = ""
    for node in node_list:
        if node.nodeType == node.TEXT_NODE:
            text = text + node.data
    return text

# -----------------------------------------------------------------------------

def _generate_context_element(doc, source, entries):
    """
    Generates a context element.

    @param doc: The DOM document.
    @param source: The actual source file name.
    @param entries: The list of UT3 translation items in this context.
    @return: The generated element.
    """

    elem = doc.createElement('context')
    elem.appendChild(_generate_name_element(doc, source))
    for ent in entries:
        elem.appendChild(_generate_message_element(doc, source, ent))

    return elem

# -----------------------------------------------------------------------------

def _generate_name_element(doc, source):
    """
    Generates a name element. This element represents the name of the context.

    @param doc: The DOM document.
    @param source: The actual source file name.
    @return: The generated element.
    """

    elem = doc.createElement('name')
    if source is not None and source != "":
        elem.appendChild(doc.createTextNode(source))

    return elem

# -----------------------------------------------------------------------------

def _generate_message_element(doc, source, ent):
    """
    Generates a message element.

    @param doc: The DOM document.
    @param source: The actual source file name.
    @param ent: The current UT3 translation item where to create the message
        element from.
    @return: The generated element.
    """

    elem = doc.createElement('message')

    elem.appendChild(_generate_location_element(doc, source))
    elem.appendChild(_generate_source_element(doc, ent.message))
    elem.appendChild(_generate_translation_element(doc, ent.translation, ent.status))
    if ent.comment is not None:
        elem.appendChild(_generate_comment_element(doc, ent.comment))

    return elem

# -----------------------------------------------------------------------------

def _generate_location_element(doc, source):
    """
    Generates a location element.

    @param doc: The DOM document.
    @param source: The actual source file name.
    @return: The generated element.
    """

    elem = doc.createElement('location')
    elem.setAttribute('filename', source)
    elem.setAttribute('line', str(0))

    return elem

# -----------------------------------------------------------------------------

def _generate_source_element(doc, message):
    """
    Generates a source element.

    @param doc: The DOM document.
    @param message: The actual source message text.
    @return: The generated element.
    """

    elem = doc.createElement('source')
    elem.appendChild(doc.createTextNode(message))

    return elem

# -----------------------------------------------------------------------------

def _generate_translation_element(doc, translation, status):
    """
    Generates a translation element.

    @param doc: The DOM document.
    @param translation: The actual translation text.
    @param status: The status of the translation.
    @return: The generated element.
    """

    elem = doc.createElement('translation')

    if translation is not None and translation != "":
        elem.appendChild(doc.createTextNode(translation))

    if status == store.STATUS_UNFINISHED:
        elem.setAttribute('type', 'unfinished')

    return elem

# -----------------------------------------------------------------------------

def _generate_comment_element(doc, comment):
    """
    Generates a comment element.

    @param doc: The DOM document.
    @param comment: The actual comment text.
    @return: The generated element.
    """

    elem = doc.createElement('comment')
    elem.appendChild(doc.createTextNode(comment))

    return elem
