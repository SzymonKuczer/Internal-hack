// dllmain.cpp : Defines the entry point for the DLL application.
#include <iostream> // for input/output to console
#include "internal.h" //our helper functions
#include <vector> // to store offsets for multi level pointers
#include <string> // for using wstrings
#include "resource.h"
#include <tchar.h> // to convert text to int
#include "AmmoAndHeadPos.h"
#include "HealthAndPos.h"


//signals we send from the GUI:
constexpr int SELECT_CHAINSAW = (WM_APP + 100);
constexpr int SELECT_SHOTGUN = (WM_APP + 101);
constexpr int SELECT_MACHINEGUN = (WM_APP + 102);
constexpr int SELECT_RPG = (WM_APP + 103);
constexpr int SELECT_SNIPER = (WM_APP + 104);
constexpr int SELECT_PISTOL = (WM_APP + 105);
constexpr int SELECT_GRENADE_LAUNCHER = (WM_APP + 106);

constexpr int FREEZE_AMMO = (WM_APP + 107);
constexpr int GOD_MODE = (WM_APP + 108);
constexpr int ULTRA_RAPID_FIRE = (WM_APP + 109);
constexpr int CREATOR_MODE = (WM_APP + 110);


constexpr int UPDATE = (WM_APP + 111);

constexpr int EDIT_CHAINSAW_AMMO = (WM_APP + 112);
constexpr int EDIT_SHOTGUN_AMMO = (WM_APP + 113);
constexpr int EDIT_MACHINEGUN_AMMO = (WM_APP + 114);
constexpr int EDIT_RPG_AMMO = (WM_APP + 115);
constexpr int EDIT_SNIPER_AMMO = (WM_APP + 116);
constexpr int EDIT_PISTOL_AMMO = (WM_APP + 117);
constexpr int EDIT_GRENADE_LAUNCHER_AMMO = (WM_APP + 118);

//patches:
//handling the code that decreases ammo
NopInternal* freezeAmmo;
//handling the code that writes to weapon cooldown
NopInternal* ultraRapidFire;
//handles the code that decreases our health
NopInternal* godMode;

//edit boxes where we can enter new values
HWND hEditChainsawAmmo;
HWND hEditShotgunAmmo;
HWND hEditMachinegunAmmo;
HWND hEditRPGAmmo;
HWND hEditSniperAmmo;
HWND hEditPistolAmmo;
HWND hEditGrenadeLauncherAmmo;

//buttons that activate/deactivate our patches:
HWND hFreezeAmmoButton;
HWND hGodModeButton;
HWND hUltraRapidFireButton;
HWND hCreatorModeButton;

INT* selectedWeaponAddress; // address of the selected weapon id in game
INT* (ammoAddresses[8]); //addresses of our ammo values in game

TCHAR buffer[16]; //buffer for our input fields

//bools to keep track of wheather a patch is activated or not;
bool isAmmoFrozen{ false };
bool isUltraRapidFireEnabled{ false };
bool isGodModeEnabled{ false };
bool isCreatorModeEnabled{ false };

//was our window closed?
DWORD dwExit{ STILL_ACTIVE };

//helper function to write the weapon we want to select to the address of the selected weapon in game memory:
inline void selectWeapon(int index)
{
	*selectedWeaponAddress = index;
}

//reads the current ammo values from memory and updates our GUI:
void updateAmmoValues()
{
	int ammoValue;

	ammoValue = *(ammoAddresses[0]);
	SetWindowText(hEditChainsawAmmo, std::to_wstring(ammoValue).c_str());

	ammoValue = *(ammoAddresses[1]);
	SetWindowText(hEditShotgunAmmo, std::to_wstring(ammoValue).c_str());

	ammoValue = *(ammoAddresses[2]);
	SetWindowText(hEditMachinegunAmmo, std::to_wstring(ammoValue).c_str());

	ammoValue = *(ammoAddresses[3]); //self note: the "&" means reference (a variable meant to be an alias of some other variable)
	SetWindowText(hEditRPGAmmo, std::to_wstring(ammoValue).c_str());

	ammoValue = *(ammoAddresses[4]);
	SetWindowText(hEditSniperAmmo, std::to_wstring(ammoValue).c_str());

	ammoValue = *(ammoAddresses[5]);
	SetWindowText(hEditPistolAmmo, std::to_wstring(ammoValue).c_str());

	ammoValue = *(ammoAddresses[6]);
	SetWindowText(hEditGrenadeLauncherAmmo, std::to_wstring(ammoValue).c_str());
}

//message handler that handles all messages that get sent from our GUI:
LRESULT CALLBACK MessageHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CLOSE:
	case WM_DESTROY:
		dwExit = 0;
		break;
		//whenever something regular happens with the gui
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case SELECT_CHAINSAW:
			selectWeapon(0);
			break;

		case SELECT_SHOTGUN:
			selectWeapon(1);
			break;

		case SELECT_MACHINEGUN:
			selectWeapon(2);
			break;

		case SELECT_RPG:
			selectWeapon(3);
			break;

		case SELECT_SNIPER:
			selectWeapon(4);
			break;

		case SELECT_GRENADE_LAUNCHER:
			selectWeapon(5);
			break;

		case SELECT_PISTOL:
			selectWeapon(6);
			break;

		case FREEZE_AMMO:
			isAmmoFrozen = !isAmmoFrozen;
			if (isAmmoFrozen) freezeAmmo->enable();
			else freezeAmmo->disable();
			SetWindowText(hFreezeAmmoButton, isAmmoFrozen ? L"Unfreeze Ammo" : L"Freeze Ammo");
			break;

		case GOD_MODE:
			isGodModeEnabled = !isGodModeEnabled;
			if (isGodModeEnabled) godMode->enable();
			else godMode->disable();
			SetWindowText(hGodModeButton, isGodModeEnabled ? L"Disable God Mode" : L"Enable God Mode"); //self note: the "?" means if the condition before it which in this case was "isGodModeEnabled" is true it will show "Disable" if else then the other text will appear
			break;

		case ULTRA_RAPID_FIRE:
			isUltraRapidFireEnabled = !isUltraRapidFireEnabled;
			if (isUltraRapidFireEnabled) ultraRapidFire->enable();
			else ultraRapidFire->disable();
			SetWindowText(hUltraRapidFireButton, isUltraRapidFireEnabled ? L"Disable Ultra Rapid" : L"Enable Ultra Rapid Fire");
			break;

		case CREATOR_MODE:
			isCreatorModeEnabled = !isCreatorModeEnabled;
			if (isCreatorModeEnabled)
			{
				godMode->enable();
				ultraRapidFire->enable();
				freezeAmmo->enable();
			}
			else
			{
				godMode->disable();
				ultraRapidFire->disable();
				freezeAmmo->disable();
			}
			SetWindowText(hCreatorModeButton, isGodModeEnabled ? L"Disable Creator Mode" : L"Made By Szymon Kuczer"); //self note: the "?" means if the condition before it which in this case was "isGodModeEnabled" is true it will show "Disable" if else then the other text will appear
			break;

		case UPDATE:
			updateAmmoValues();
			break;
		}

	case EN_CHANGE: //whenever an edit field gets changed
		static int ammoValue[12]{};
		int tmpAmmoValue[12]{};

		switch (LOWORD(wParam))
		{
		case EDIT_CHAINSAW_AMMO:
			GetWindowText(hEditChainsawAmmo, buffer, 16);
			tmpAmmoValue[0] = _ttoi(buffer); //self note: _ttoi() converts an integer to a char.
			if (ammoValue[0] != tmpAmmoValue[0])
			{
				ammoValue[0] = tmpAmmoValue[0];
				*ammoAddresses[0] = ammoValue[0];
			}
			break;

		case EDIT_SHOTGUN_AMMO:
			GetWindowText(hEditShotgunAmmo, buffer, 16);
			tmpAmmoValue[1] = _ttoi(buffer); //self note: _ttoi() converts an integer to a char.
			if (ammoValue[1] != tmpAmmoValue[1])
			{
				ammoValue[1] = tmpAmmoValue[1];
				*ammoAddresses[1] = ammoValue[1];
			}
			break;

		case EDIT_MACHINEGUN_AMMO:
			GetWindowText(hEditMachinegunAmmo, buffer, 16);
			tmpAmmoValue[2] = _ttoi(buffer); //self note: _ttoi() converts an integer to a char.
			if (ammoValue[2] != tmpAmmoValue[2])
			{
				ammoValue[2] = tmpAmmoValue[2];
				*ammoAddresses[2] = ammoValue[2];
			}
			break;

		case EDIT_RPG_AMMO:
			GetWindowText(hEditRPGAmmo, buffer, 16);
			tmpAmmoValue[3] = _ttoi(buffer); //self note: _ttoi() converts an integer to a char.
			if (ammoValue[3] != tmpAmmoValue[3])
			{
				ammoValue[3] = tmpAmmoValue[3];
				*ammoAddresses[3] = ammoValue[3];
			}
			break;

		case EDIT_SNIPER_AMMO:
			GetWindowText(hEditSniperAmmo, buffer, 16);
			tmpAmmoValue[4] = _ttoi(buffer); //self note: _ttoi() converts an integer to a char.
			if (ammoValue[4] != tmpAmmoValue[4])
			{
				ammoValue[4] = tmpAmmoValue[4];
				*ammoAddresses[4] = ammoValue[4];
			}
			break;

		case EDIT_PISTOL_AMMO:
			GetWindowText(hEditPistolAmmo, buffer, 16);
			tmpAmmoValue[5] = _ttoi(buffer); //self note: _ttoi() converts an integer to a char.
			if (ammoValue[5] != tmpAmmoValue[5])
			{
				ammoValue[5] = tmpAmmoValue[5];
				*ammoAddresses[5] = ammoValue[5];
			}
			break;

		case EDIT_GRENADE_LAUNCHER_AMMO:
			GetWindowText(hEditGrenadeLauncherAmmo, buffer, 16);
			tmpAmmoValue[6] = _ttoi(buffer); //self note: _ttoi() converts an integer to a char.
			if (ammoValue[6] != tmpAmmoValue[6])
			{
				ammoValue[6] = tmpAmmoValue[6];
				*ammoAddresses[6] = ammoValue[6];
			}
			break;
		}
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

DWORD WINAPI InternalMain(HMODULE hMod)
{
#ifdef _DEBUG
	AllocConsole();
	FILE* f;
	freopen_s(&f, "CONOUT$", "w", stdout);
#endif

	//name of exe of our game
	wchar_t gameName[]{ L"sauerbraten.exe" };

	/* in cheat engine we found this static address
	of the player pointer (sauerbraten.exe + 2A3528): */
	size_t playerOffset{ 0x2A3528 };
	size_t healthOffset{ 0x312930 };
	size_t noAnimOffset{ 0x3120D0 };

	//get the process id of our game:
	uintptr_t moduleBase = GetModuleBaseAddress(gameName);

	//where the pointer to our player is located
	uintptr_t playerBase{ moduleBase + playerOffset };
	PlayerAmmo* playerAmmo{* reinterpret_cast<PlayerAmmo**>(moduleBase + playerOffset) };
	PlayerHealth* playerHealth{ *reinterpret_cast<PlayerHealth**>(moduleBase + healthOffset) };


	//uintptr_t healthBase{ moduleBase + healthOffset };
	//uintptr_t noAnimBase{ moduleBase + noAnimOffset };



	//all our offsets
	//std::vector<unsigned int>ammoOffsets = { 0x0 + 0x1a0 };
	std::vector<unsigned int>healthOffsets = { 0x0, 0x118, 0x340 };
	//std::vector<unsigned int>noAnimOffsets = { 0x0 + 0x190 };


	//finds the health address through multi level pointers 
	uintptr_t healthAddress = FindDynamicAddress((uintptr_t)playerHealth, healthOffsets);
	//uintptr_t ammoAddress = FindDynamicAddress(playerBase, ammoOffsets);
	//uintptr_t noAnimAddress = FindDynamicAddress(noAnimBase, noAnimOffsets);

	//read the address where our player is from the player pointer
	BYTE* playerAddress{*(BYTE**)playerBase};



	//address of selected weapon
	selectedWeaponAddress = { (INT*)playerAddress + 0x018C };
	ammoAddresses[0] = (INT*)(playerAddress + 0x0194); //static chainsaw ammo
	ammoAddresses[1] = (INT*)(playerAddress + 0x198); // static shotgun ammo (remember to find this dumbass) 
	ammoAddresses[2] = (INT*)(playerAddress + 0x019C);  //static mingun ammo
	ammoAddresses[3] = (INT*)(playerAddress + 0x01A0);  //static rocket launcher ammo
	ammoAddresses[4] = (INT*)(playerAddress + 0x01A4);  //static sniper ammo
	ammoAddresses[5] = (INT*)(playerAddress + 0x01AC);  //static pistol ammo address	
	ammoAddresses[6] = (INT*)(playerAddress + 0x01A8);  //static Grenade launcher ammo


	const size_t weaponCount{ sizeof(ammoAddresses) / sizeof(BYTE*) };

	//variable to store our original health value in
	int originalHealth{ 100 };

	//variable to store the slected weapon id
	int selectedWeapon;

	//array to store our original ammo values in
	int originalAmmoValues[weaponCount];

	//new value we want to write
	int hackedValue{ 1337 };


	//self note: fix the godamn addresses they arent the right ones P.S FindDynamicAddress bb
	freezeAmmo = new NopInternal((BYTE*)(moduleBase + 0x1DB5E0), 8);
	godMode = new NopInternal((BYTE*)(moduleBase + 0x1ECB97), 6);
	ultraRapidFire = new NopInternal((BYTE*)(moduleBase + 0x1DBA02), 7);

	WNDCLASSEX wc{};
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = MessageHandler;
	wc.hInstance = hMod;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszClassName = L"Trainer_GUI";
	RegisterClassEx(&wc);

	HWND hWnd
	{
		CreateWindowEx(0, L"TRAINER_GUI", L"Second External Trainer for \"Cube 2: sauerbraten\"", WS_VISIBLE | WS_EX_LAYERED | WS_BORDER, CW_USEDEFAULT, CW_USEDEFAULT, 500, 700, 0, 0, hMod, 0)
	};

	size_t yOff{ 0 };
	size_t spacePerItem{ 50 };

	yOff *= spacePerItem;
	CreateWindowEx(0, L"Static", L"Chainsaw:", WS_TABSTOP | WS_CHILD | WS_VISIBLE, 10, 5 + yOff, 200, 45, hWnd, NULL, hMod, 0);

	HWND hSelectChainsawBtn
	{
		CreateWindowEx(0, L"Button", L"Select", WS_TABSTOP | WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | BS_FLAT, 100, 15 + yOff, 100, 30, hWnd, (HMENU)SELECT_CHAINSAW, hMod, 0)
	};

	hEditChainsawAmmo = CreateWindowEx(0, L"Edit", L"ammo", WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_BORDER, 15, 25 + yOff, 60, 20, hWnd, (HMENU)EDIT_CHAINSAW_AMMO, 0, 0);

	//-----------------------------------------
	yOff = 1;
	yOff *= spacePerItem;
	CreateWindowEx(0, L"Static", L"Shotgun:", WS_TABSTOP | WS_CHILD | WS_VISIBLE, 10, 5 + yOff, 200, 45, hWnd, NULL, hMod, 0);

	HWND hSelectShotgunBtn
	{
		CreateWindowEx(0, L"Button", L"Select", WS_TABSTOP | WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | BS_FLAT, 100, 15 + yOff, 100, 30, hWnd, (HMENU)SELECT_SHOTGUN, hMod, 0)
	};

	hEditShotgunAmmo = CreateWindowEx(0, L"Edit", L"ammo", WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_BORDER, 15, 25 + yOff, 60, 20, hWnd, (HMENU)EDIT_SHOTGUN_AMMO, 0, 0);
	//-------------------------------------------
	yOff = 2;
	yOff *= spacePerItem;
	CreateWindowEx(0, L"Static", L"Machine Gun:", WS_TABSTOP | WS_CHILD | WS_VISIBLE, 10, 5 + yOff, 200, 45, hWnd, NULL, hMod, 0);

	HWND hSelectMachineGunBtn
	{
		CreateWindowEx(0, L"Button", L"Select", WS_TABSTOP | WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | BS_FLAT, 100, 15 + yOff, 100, 30, hWnd, (HMENU)SELECT_MACHINEGUN, hMod, 0)
	};

	hEditMachinegunAmmo = CreateWindowEx(0, L"Edit", L"ammo", WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_BORDER, 15, 25 + yOff, 60, 20, hWnd, (HMENU)EDIT_MACHINEGUN_AMMO, 0, 0);
	//-------------------------------------------
	yOff = 3;
	yOff *= spacePerItem;
	CreateWindowEx(0, L"Static", L"RPG:", WS_TABSTOP | WS_CHILD | WS_VISIBLE, 10, 5 + yOff, 200, 45, hWnd, NULL, hMod, 0);

	HWND hSelectRPGBtn
	{
		CreateWindowEx(0, L"Button", L"Select", WS_TABSTOP | WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | BS_FLAT, 100, 15 + yOff, 100, 30, hWnd, (HMENU)SELECT_RPG, hMod, 0)
	};

	hEditRPGAmmo = CreateWindowEx(0, L"Edit", L"ammo", WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_BORDER, 15, 25 + yOff, 60, 20, hWnd, (HMENU)EDIT_RPG_AMMO, 0, 0);
	//-------------------------------------------
	yOff = 4;
	yOff *= spacePerItem;
	CreateWindowEx(0, L"Static", L"Sniper:", WS_TABSTOP | WS_CHILD | WS_VISIBLE, 10, 5 + yOff, 200, 45, hWnd, NULL, hMod, 0);

	HWND hSelectSniperBtn
	{
		CreateWindowEx(0, L"Button", L"Select", WS_TABSTOP | WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | BS_FLAT, 100, 15 + yOff, 100, 30, hWnd, (HMENU)SELECT_SNIPER, hMod, 0)
	};

	hEditSniperAmmo = CreateWindowEx(0, L"Edit", L"ammo", WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_BORDER, 15, 25 + yOff, 60, 20, hWnd, (HMENU)EDIT_SNIPER_AMMO, 0, 0);
	//-------------------------------------------
	yOff = 5;
	yOff *= spacePerItem;
	CreateWindowEx(0, L"Static", L"Pistol:", WS_TABSTOP | WS_CHILD | WS_VISIBLE, 10, 5 + yOff, 200, 45, hWnd, NULL, hMod, 0);

	HWND hSelectPistolBtn
	{
		CreateWindowEx(0, L"Button", L"Select", WS_TABSTOP | WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | BS_FLAT, 100, 15 + yOff, 100, 30, hWnd, (HMENU)SELECT_PISTOL, hMod, 0)
	};

	hEditPistolAmmo = CreateWindowEx(0, L"Edit", L"ammo", WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_BORDER, 15, 25 + yOff, 60, 20, hWnd, (HMENU)EDIT_PISTOL_AMMO, 0, 0);
	//-------------------------------------------

	yOff = 6;
	yOff *= spacePerItem;
	CreateWindowEx(0, L"Static", L"Grenade Launcher:", WS_TABSTOP | WS_CHILD | WS_VISIBLE, 10, 5 + yOff, 200, 45, hWnd, NULL, hMod, 0);

	HWND hSelectGrenadeLauncherBtn
	{
		CreateWindowEx(0, L"Button", L"Select", WS_TABSTOP | WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | BS_FLAT, 100, 15 + yOff, 100, 30, hWnd, (HMENU)SELECT_GRENADE_LAUNCHER, hMod, 0)
	};

	hEditGrenadeLauncherAmmo = CreateWindowEx(0, L"Edit", L"ammo", WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_BORDER, 15, 25 + yOff, 60, 20, hWnd, (HMENU)EDIT_GRENADE_LAUNCHER_AMMO, 0, 0);
	//-------------------------------------------

	yOff = 7;
	yOff *= spacePerItem;

	HWND hUpdateBtn
	{
		CreateWindowEx(0, L"button", L"Update Ammo Values", WS_TABSTOP | WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | BS_FLAT, 10, yOff, 200, 50, hWnd, (HMENU)UPDATE, hMod, 0)
	};
	//-------------------------------------------
	spacePerItem = 60;

	yOff = 0;
	yOff *= spacePerItem;

	hFreezeAmmoButton = CreateWindowEx(0, L"button", L"Freeze Ammo", WS_TABSTOP | WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | BS_FLAT, 220, 10 + yOff, 250, 50, hWnd, (HMENU)FREEZE_AMMO, hMod, 0);
	//-------------------------------------------
	yOff = 1;
	yOff *= spacePerItem;

	hGodModeButton = CreateWindowEx(0, L"button", L"Enable God Mode", WS_TABSTOP | WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | BS_FLAT, 220, 10 + yOff, 250, 50, hWnd, (HMENU)GOD_MODE, hMod, 0);
	//-------------------------------------------
	yOff = 2;
	yOff *= spacePerItem;

	hUltraRapidFireButton = CreateWindowEx(0, L"button", L"Enable Ultra Rapid Fire", WS_TABSTOP | WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | BS_FLAT, 220, 10 + yOff, 250, 50, hWnd, (HMENU)ULTRA_RAPID_FIRE, hMod, 0);

	//-------------------------------------------

	yOff = 3;
	yOff *= spacePerItem;

	hCreatorModeButton = CreateWindowEx(0, L"button", L"Created By Szymon Kuczer", WS_TABSTOP | WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | BS_FLAT, 220, 10 + yOff, 250, 50, hWnd, (HMENU)CREATOR_MODE, hMod, 0);

	//-------------------------------------------

	//custom icon:
	//load custom icon:
	//HICON hicon = (HICON)LoadImageW(hInstance, MAKEINTRESOURCEW(IDI_ICON1), IMAGE_ICON, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), 0);

	//set custom icon:
	//SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hicon);

	//call updateAmmoValues to update the GUI to display the current values:
	updateAmmoValues();

	MSG msg; //hold the message sent from the GUI

	//as long as our window did not get closed:
	while (dwExit == STILL_ACTIVE)
	{
		//try to get  a new message from our GUI
		BOOL result = GetMessage(&msg, 0, 0, 0);
		if (result > 0) //if we were able to get a new message
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg); //sned to our message handlet
		}
		else // else return error code
		{
			return result;
		}
	}

	//disable the hacks:
	godMode->disable();
	ultraRapidFire->disable();
	freezeAmmo->disable();

	//clean up our dynamically allocated memory
	delete godMode;
	delete freezeAmmo;
	delete ultraRapidFire;

#ifdef _DEBUG //close our console
	if (f != nullptr) fclose(f);
	FreeConsole();
#endif
	//eject our library
	FreeLibraryAndExitThread(hMod, 0);

	return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		//start a new thread for our cheat to run in so this thread can return to the injector
		HANDLE tHndl = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)InternalMain, hModule, 0, 0);

		//if we were able to create a thread, close the handle to it
		if (tHndl)
		{
			CloseHandle(tHndl);
		}

		//if we where unable to create a thread, return false
		else
		{
			return FALSE;
		}
		break;
	}
	return TRUE; //everything went fine
}

