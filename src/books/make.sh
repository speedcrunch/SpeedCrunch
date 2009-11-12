#! /usr/bin/env bash


#------------------------------------------------------------------------------#
# This file is part of the SpeedCrunch project.                                #
#                                                                              #
# Helder Correia <helder.pereira.correia@gmail.com>                            #
#                                                                              #
# It generates all the book pages (including translations).                    #
#                                                                              #
# It uses the Universal Template Translation Tools - bundled -                 #
# (http://code.google.com/p/ut3/). The msgmerge tool from GNU Gettext          #
# (http://www.gnu.org/software/gettext/), the bash shell                       #
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

$T2P -o locale/books.pot templates/*.template

#------------------------------------------------------------------------------#
# merge and generate pages                                                     #
#------------------------------------------------------------------------------#

LANGS="ar ca cs de en es es_AR eu fi fr he hu id it ja ko nb nl pl pt pt_BR ro ru sv tr zh_CN"

for lang in $LANGS
do
	touch locale/books.$lang.po
	mkdir -p $lang
	$MSM locale/books.$lang.po locale/books.pot
        for temp in templates/*
        do
            woSuffix=${temp%.template}
            final=${woSuffix#templates/}
            $P2H -t $temp -i locale/books.$lang.po -o $lang/$final
        done
done

