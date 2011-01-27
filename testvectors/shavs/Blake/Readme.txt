
 Contents                  Description
---------------           ------------------------------------------------
genKAT.c                  Source to generate KAT files for required digest sizes

ShortMsgKAT.txt           Input file for Short Message KAT Test
LongMsgKAT.txt            Input file for Long Message KAT Test
ExtremelyLongMsg.txt      Input file for Extremely Long Message KAT Test
MonteCarlo.txt            Input file for Monte Carlo Test

Readme.txt                This file



Directions
------------------

1. Edit the header of each of the four KAT input files.  Insert the appropriate
    "Algorithm Name" and "Principal Submitter".  Make sure you put these on the
    same line as the title (e.g., "Pricipal Submitter") and that you don't have
    any line breaks in the entry.  Also, limit these entries to a maximum of 127
    characters each.

2. Edit genKAT.c to specify the appropriate #include for the algorithm's API
    definitions.

3. Add algorithm code.

4. Compile

5. Place the KAT input file in the same directory as genKAT.exe.

6. Execute genKAT.exe to create the KAT files for the various digest sizes.
