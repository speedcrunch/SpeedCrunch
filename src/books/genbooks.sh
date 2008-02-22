#! /usr/bin/env bash


#------------------------------------------------------------------------------#
# This file is part of the SpeedCrunch project.                                #
#                                                                              #
# It generates the whole book pages (including translations).                  #
# It makes use of the template2pot and po2final template translation tools,    #
# as well as the msgmerge tool.                                                #
#                                                                              #
# Helder Correia <helder.pereira.correia@gmail.com>                            #
#------------------------------------------------------------------------------#


#------------------------------------------------------------------------------#
# macros                                                                       #
#------------------------------------------------------------------------------#

T2P="python ../../../tools/template2pot.py -l ""<i18n>"" -r ""</i18n>"" -s ""<hint>"" -e ""</hint>"""
P2H="python ../../../tools/po2final.py     -l ""<i18n>"" -r ""</i18n>"" -s ""<hint>"" -e ""</hint>"""
MSM="msgmerge -U --quiet"


#------------------------------------------------------------------------------#
# extract marked strings and create POT                                        #
#------------------------------------------------------------------------------#

$T2P -o i18n/books.pot templates/*.template


#------------------------------------------------------------------------------#
# merge and generate pages                                                     #
#------------------------------------------------------------------------------#

LANGS="de en pt fi"

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
