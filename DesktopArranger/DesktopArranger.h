#ifndef _DESKTOP_ARRANGER_H_
#define _DESKTOP_ARRANGER_H_

#include <Windows.h>
#include <Commctrl.h>
#include <Shlobj.h>
#include <list>
#include <memory>
#include <string>

namespace NsDesktopArranger
{
	class DesktopArranger
	{
	public:
		DesktopArranger(std::string fileSpec, std::string fakeFileName);
		~DesktopArranger();

		bool Initialize();
		void PrintIconLocations();
		void ArrangeDesktop();
		bool DisableAutoArrangeAndSnapToGrid();
		void RestartExplorer();

	private:
		struct IconPosition
		{
			int x;
			int y;

			IconPosition(int xpos, int ypos) 
				: x(xpos)
				, y(ypos) 
			{}
		};

		PWSTR FindDesktopPath();
		void FindIconLocations();
		bool FindUserDesktop();
		static BOOL CALLBACK EnumWindowsCallback(__in HWND hWnd, __in LPARAM lParam);
		void ReadDesktopSpecification(const char * file);
		void MakeFakeDesktopItems(int numOfItems);
		void MoveItemsToNewPositions();
		void KillExplorer();
		void StartExplorer();

		HWND m_desktopHwnd;
		std::string m_fileSpecification;
		std::string m_fakeFileName;

		std::list<std::shared_ptr<IconPosition>> m_currentPositions;
		std::list<std::shared_ptr<IconPosition>> m_newPositions;
	};
}

#endif // _DESKTOP_ARRANGER_H_
