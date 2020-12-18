#!/bin/sh

mydir=$(dirname $0)
flames_all_file=flames_all.out

if [ ! -f Indicator_Release_NBIs.csv ]; then
    wget --quiet https://raw.githubusercontent.com/fireeye/sunburst_countermeasures/main/indicator_release/Indicator_Release_NBIs.csv
else
    >&2 echo "- Required file already exists: Indicator_Release_NBIs.csv"
fi

if [ ! -f uniq-hostnames.txt ]; then
    wget --quiet https://raw.githubusercontent.com/bambenek/research/main/sunburst/uniq-hostnames.txt
else
    >&2 echo "- Required file already exists: uniq-hostnames.txt"
fi

if [ ! -x $mydir/flames_pub ]; then
    >&2 echo "- Binary not yet compiled - compiling"
    gcc -std=c99 -o flames_pub flames_pub.c
else
    >&2 echo "- Binary found"
fi

if [ ! -x $mydir/flames_pub ]; then
    >&2 echo "- Binary still not found - aborting"
    exit
fi

# Get full list of strings.
$mydir/flames_pub < uniq-hostnames.txt | grep domain  > $flames_all_file
>&2 echo "- Full list output to ${flames_all_file} - $(cat uniq-hostnames.txt | wc -l) lines"

>&2 echo "- Strings that can be reassembled:"
# Display string fragments in original order.
for uid in $(cut -d , -f 3 Indicator_Release_NBIs.csv | $mydir/flames_pub | grep NextString | cut -d ' ' -f 4); do
    grep $uid $flames_all_file | sort -k 6 -n
done
echo ""

#rm $flames_all_file

