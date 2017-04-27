// DesktopArranger.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "DesktopArranger.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <iterator>
#include <vector>
#include <atlstr.h>

namespace NsDesktopArranger
{
	DesktopArranger::DesktopArranger(std::string fileSpec, std::string fakeFileName)
		: m_desktopHwnd(NULL)
		, m_fileSpecification(fileSpec)
		, m_fakeFileName(fakeFileName)
	{}

	DesktopArranger::~DesktopArranger()
	{}

	bool DesktopArranger::Initialize()
	{
		return FindUserDesktop() ? true : false;
	}

	void DesktopArranger::PrintIconLocations()
	{
		if (m_currentPositions.size() == 0)
		{
			FindIconLocations();
		}
		std::cout << "Number of desktop items: " << m_currentPositions.size() << std::endl;
		for (std::shared_ptr<IconPosition> pos : m_currentPositions)
		{
			std::cout << "(" << pos->x << "," << pos->y << ")" << std::endl;
		}
	}

	void DesktopArranger::ArrangeDesktop()
	{
		// Read spec and figure out if we need to create new fake desktop items
		ReadDesktopSpecification(m_fileSpecification.c_str());
		std::cout << "Specified desktop configuration requires " << m_newPositions.size() << " items" << std::endl;

		// specification requires more items that are currently on desktop
		// so we need to create fake ones
		if (m_newPositions.size() > m_currentPositions.size()) 
		{
			int numOfMissingItems = m_newPositions.size() - m_currentPositions.size();
			MakeFakeDesktopItems(numOfMissingItems);
		}

		MoveItemsToNewPositions();
		
	}

	PWSTR DesktopArranger::FindDesktopPath()
	{
		PWSTR path = NULL;
		HRESULT result = SHGetKnownFolderPath(FOLDERID_Desktop, 0, NULL, &path);
		if (SUCCEEDED(result))
		{
			return path;
		}
		else return NULL;
	}
	
	bool DesktopArranger::FindUserDesktop()
	{
		HWND _ProgMan = GetShellWindow();
		HWND _SHELLDLL_DefViewParent = _ProgMan;
		HWND _SHELLDLL_DefView = FindWindowEx(_ProgMan, nullptr, "SHELLDLL_DefView", NULL);
		m_desktopHwnd = FindWindowEx(_SHELLDLL_DefView, nullptr, "SysListView32", "FolderView");

		if (_SHELLDLL_DefView == nullptr)
		{
			if (EnumWindows(EnumWindowsCallback, reinterpret_cast<LPARAM>(this)))
			{
				return false;
			}
		}
		return true;
	}

	void DesktopArranger::FindIconLocations()
	{
		PWSTR path = FindDesktopPath();

		int numOfIcons = ListView_GetItemCount(m_desktopHwnd);

		// Get process ID of desktop
		DWORD desktopProcessId = NULL;
		GetWindowThreadProcessId(m_desktopHwnd, &desktopProcessId);

		// Get handle to the desktop process
		HANDLE desktopProcessHandle = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE, NULL, desktopProcessId);

		// Allocate shared memory to get the POINT
		PSTR sharedMemoryPointer = (PSTR)VirtualAllocEx(desktopProcessHandle, nullptr, sizeof(POINT), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

		// Loop through all icons
		for (int i = 0; i < numOfIcons; i++)
		{
			POINT iconPoint;
			DWORD numOfBytes = 0;

			WriteProcessMemory(desktopProcessHandle, sharedMemoryPointer, &iconPoint, sizeof(POINT), &numOfBytes);
			SendMessage(m_desktopHwnd, LVM_GETITEMPOSITION, i, (LPARAM)sharedMemoryPointer);

			// Get the data from the shared memory
			ReadProcessMemory(desktopProcessHandle, sharedMemoryPointer, &iconPoint, sizeof(POINT), &numOfBytes);

			std::shared_ptr<IconPosition> pos = std::make_shared<IconPosition>(iconPoint.x, iconPoint.y);
			m_currentPositions.push_back(pos);
		}

		// Remember to clean up
		if (sharedMemoryPointer != nullptr)
		{
			VirtualFreeEx(desktopProcessHandle, sharedMemoryPointer, 0, MEM_RELEASE);
		}
		if (desktopProcessHandle != nullptr)
		{
			CloseHandle(desktopProcessHandle); // TODO check return
		}
	}
	
	BOOL DesktopArranger::EnumWindowsCallback(HWND hWnd, LPARAM lParam)
	{
		char lpClassName[256]; 
		GetClassName(hWnd, lpClassName, 256);
		DesktopArranger * pThis = reinterpret_cast<DesktopArranger *>(lParam);

		if (strstr(lpClassName, "WorkerW"))
		{
			HWND child = FindWindowEx(hWnd, nullptr, "SHELLDLL_DefView", NULL);
			if (child != NULL)
			{
				pThis->m_desktopHwnd = FindWindowEx(child, nullptr, "SysListView32", "FolderView"); ;
				return false;
			}
		}
		return true;
	}
	
	void DesktopArranger::ReadDesktopSpecification(const char * file)
	{
		int row = 0;

		std::ifstream ifs(file);
		for (std::string line; std::getline(ifs, line);)
		{
			int column = 0;

			std::stringstream ss(line);
			int item;
			while (ss >> item)
			{
				if (item == 1)
				{					
					std::shared_ptr<IconPosition> newPos = std::make_shared<IconPosition>((column * (1920 / 16)) / 2, (row * (1080 / 9)) / 2);
					m_newPositions.push_back(newPos);
				}
				column++;
			}
			row++;
		}
	}

	void DesktopArranger::MakeFakeDesktopItems(int numOfItems)
	{
		std::cout << "Creating " << numOfItems << " fakes ....." << std::endl;

		PWSTR desktopPath = FindDesktopPath();
		std::string filePath = CW2A(desktopPath);
		for (int i = 0; i < numOfItems; i++)
		{
			std::stringstream ss;
			ss << filePath << "\\" << m_fakeFileName << i;

			std::ofstream newItem(ss.str());
		}
	}

	void DesktopArranger::MoveItemsToNewPositions()
	{
		size_t index = 0;
		int newXPos = 0, newYPos = 0;
		std::list<std::shared_ptr<IconPosition>>::iterator it = m_newPositions.begin();
		for (auto pos : m_currentPositions)
		{
			// we still have enough new positions
			if (index < m_newPositions.size())
			{
				newXPos = it->get()->x;
				newYPos = it->get()->y;
				ListView_SetItemPosition(m_desktopHwnd, index, newXPos, newYPos);
			}
			else // specification is done and there are still items on old positions - just place the rest on last position on top of each other
			{
				ListView_SetItemPosition(m_desktopHwnd, index, newXPos, newYPos);
			}
			if(it != m_newPositions.end()) it++;
			index++;
		}
	}

}
