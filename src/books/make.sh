#! /usr/bin/env bash


#------------------------------------------------------------------------------#
# This file is part of the SpeedCrunch project.                                #
#                                                                              #
# Helder Correia <helder.pereira.correia@gmail.com>                            #
#                                                                              #
# It generates all the book pages (including translations).                    #
# It uses the Universal Template Translation Tools                             #
# (http://code.google.com/p/ut3/), as well as the msgmerge tool from the GNU   #
# Gettext project (http://www.gnu.org/software/gettext/). The bash shell       #
# (http://www.gnu.org/software/bash/bash.html) and Python (http://python.org/) #
# are required to run this script.                                             #
#------------------------------------------------------------------------------#

#------------------------------------------------------------------------------#
# macros                                                                       #
#------------------------------------------------------------------------------#

T2P="python ut3/template2pot.py -l ""<i18n>"" -r ""</i18n>"" -s ""<hint>"" -e ""</hint>"""
P2H="python ut3/po2final.py     -l ""<i18n>"" -r ""</i18n>"" -s ""<hint>"" -e ""</hint>"""
MSM="msgmerge -U --quiet"

#------------------------------------------------------------------------------#
# extract marked strings and create POT                                        #
#------------------------------------------------------------------------------#

$T2P -o i18n/books.pot templates/*.template

#------------------------------------------------------------------------------#
# merge and generate pages                                                     #
#------------------------------------------------------------------------------#

LANGS="ar ca cs de en es es_AR eu fi fr he id it nb nl pl pt pt_BR ro ru sv tr zh_CN"

for lang in $LANGS
do
	touch i18n/books.$lang.po
	mkdir -p $lang
	$MSM i18n/books.$lang.po i18n/books.pot
        for temp in templates/*
        do
            woSuffix=${temp%.template}
            final=${woSuffix#templates/}
            $P2H -t $temp -i i18n/books.$lang.po -o $lang/$final
        done
done

