# SUNBURST DGA decoder

Details on Securelist: [https://securelist.com/sunburst-connecting-the-dots-in-the-dns-requests/99862/](https://securelist.com/sunburst-connecting-the-dots-in-the-dns-requests/99862/)

## Compiling

Windows: CL.EXE flames\_pub.c (start a Visual Studio Command Prompt)
 *binary for Windows included*

Linux/Unix: gcc -std=c99 -o flames\_pub flames\_pub.c

## Match the domain requests published by FireEye with target names

1. Compile the binary.

2. Download the datasets.

```
https://raw.githubusercontent.com/fireeye/sunburst_countermeasures/main/indicator_release/Indicator_Release_NBIs.csv
https://raw.githubusercontent.com/bambenek/research/main/sunburst/uniq-hostnames.txt
```

3. Execute in the same directory as datasets:

Windows: extract\_uids.bat

Linux/Unix: extract\_uids.sh

