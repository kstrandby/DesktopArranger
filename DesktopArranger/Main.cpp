#include "stdafx.h"
#include "DesktopArranger.h"
#include <string>
#include <iostream>

using namespace NsDesktopArranger;

static void ShowHelp()
{
	std::cerr << "Usage: DesktopArranger.exe [options]\n"
			  << "Options: \n"
			  << "\t-h, --help\t			Show this help message.\n"
			  << "\t-a, --arrange <NameOfFile>\t	Arrange the desktop according to the pattern specified in the file (file should contain 18 * 32 grid of 0's and 1's specifiying item positions).\n"
			  << "\t-f, --fakefile <NameOfFile>\t	If this option is set, fake files will be named accordingly, if created.\n"
			  << "\t-s, --save <NameOfFile>\t		Save the current item positions on the desktop to a file to allow restoring positions.\n"
			  << "\t-r, --restore <NameOfFile>\t	Restore the item positions saved in the file.\n"
			  << std::endl;
}

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		ShowHelp();
		return -1;
	}
	else
	{
		std::string fakeFileName = "foo.bar";
		std::string configFileName = "";


		for (int i = 1; i < argc; i++)
		{
			std::string argument(argv[i]);
			std::string value;
			
			if (i < argc - 1) // check for single argument or argument with corresponding value
			{
				value = argv[i + 1];
			}
			else if (argument == "-h" || argument == "--help")
			{
				ShowHelp();
				return 0;
			}
			else 
			{
				std::cerr << "Missing value for argument. For usage, please see help using -h or --help." << std::endl;
				return -1;
			}

			// if we get here, we have an argument with corresponding value
			if (argument == "-a" || argument == "--arrange")
			{
				configFileName = argv[i + 1];
				i++;
			}
			else if (argument == "-f" || argument == "--fakefile")
			{
				fakeFileName = argv[i + 1];
				i++;
			}
			else if (argument == "-s" || argument == "--save")
			{
				std::cerr << "Not implemented yet" << std::endl;
				return -1;
			}
			else if (argument == "-r" || argument == "--restore")
			{
				std::cerr << "Not implemented yet" << std::endl;
				return -1;
			}
			else
			{
				std::cerr << "Invalid argument option. To see valid options, please see help using -h or --help." << std::endl;
				return -1;
			}
		}
		DesktopArranger desktopArranger(configFileName, fakeFileName);

		if (desktopArranger.Initialize())
		{
			desktopArranger.ArrangeDesktop();
		}
		else
		{
			std::cerr << "Error initializing DesktopArranger!" << std::endl;
		}
	}

	std::cout << "Press enter to exit" << std::endl;
	std::cin.ignore();
}
