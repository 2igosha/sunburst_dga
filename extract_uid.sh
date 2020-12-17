#!/bin/sh
# Required files:
# https://raw.githubusercontent.com/fireeye/sunburst_countermeasures/main/indicator_release/Indicator_Release_NBIs.csv
# https://raw.githubusercontent.com/bambenek/research/main/sunburst/uniq-hostnames.txt
flames_all=$(mktemp)
mydir=$(dirname $0)
$mydir/flames_pub < uniq-hostnames.txt | grep domain  > $flames_all
for uid in $(cut -d , -f 3 Indicator_Release_NBIs.csv | $mydir/flames_pub | grep NextString | cut -d ' ' -f 4); do grep $uid $flames_all; done
rm $flames_all
