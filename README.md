DesktopArranger arranges desktop icons according to a specified pattern. 
The pattern is specified in a text file of 32 characters in width and 18 characters in height, separated by spaces.
You specify the pattern by putting 1's in the positions where you want the icons to be placed, and 0's where there should be no icons.
If the specified pattern contains more 1's than there are icons available on the desktop, the program will create extra icons.
The extra created icons will be empty textfiles - there is an option to specify a name for the textfiles.
If the specified pattern contains less 1's than there are icons available on the desktop, the excess icons will be placed on top of each other.

To run the program:

DesktopArranger.exe [options]
- where the options are:
      -h, --help			Shows a help message.
      -a, --arrange <NameOfFile>	Arranges the desktop according to the pattern specified in the file
      -f, --fakefile <NameOfFile>	If this option is set, fake files will be named accordingly, if created
