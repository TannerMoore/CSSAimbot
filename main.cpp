#include <Windows.h>
#include <ctime>
#include <string>
#include <list>
#include <sstream>
#include <conio.h>
#include <math.h>
#include <iostream>
#include <exception>
#include <time.h>

#include "MainPlayer.h"
#include "PlayerPointers.h"
#include "MousePosition.h"
#include "LastEnemyState.h"
#include "AutoTrigger.h"
#include "PlayerJump.h"

using namespace std;

float Get3dDistance(MainPlayer* me, PlayerObject* otherPlayer);

DWORD WINAPI MainThread(LPVOID lpThreadParameter);

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, PVOID lpvReserved)
{
	if(fdwReason == DLL_PROCESS_ATTACH)
	{
		AllocConsole();
		AttachConsole(GetProcessId(hinstDLL));
		CreateThread(NULL, NULL, MainThread, NULL, NULL, NULL);
	}

	return TRUE; // TRUE MEANS SUCCESSFUL INJECT!
}

DWORD WINAPI MainThread(LPVOID lpThreadParameter)
{
	MousePosition* mousePosition = NULL;
	MainPlayer* mainPlayer = NULL;
	PlayerPointers* playerPointers = NULL;
	AutoTrigger* autoTrigger = NULL;
	PlayerJump* bHop = NULL;

	boolean enemyFound = false;
	boolean targetLock = false;

	boolean aimActive = false;
	boolean aquireAddresses = false;
	boolean autoTriggerStatus = false;
	boolean noBangStatus = false;
	boolean noSmogStatus = false;
	boolean noRecoilStatus = false;

	float  PI = 3.14159265358979f;

	float pitch;
	float yaw;

	float crouchOffset = 0;

	float closestEnemyDistance;

	int closestEnemy = 0;
	clock_t time = 0;

	LastEnemyState lastEnemyState[32];

	while(true)
	{

		if(GetAsyncKeyState(VK_F1) && clock() - time > 1000)
		{
			aquireAddresses = !aquireAddresses;
			time = clock();

			if(!aquireAddresses)
			{

				aimActive = false;
				autoTriggerStatus = false;
				noBangStatus = false;
				noSmogStatus = false;
				noRecoilStatus = false;

			}
		}

		if(GetAsyncKeyState(VK_F2) && clock() - time > 1000)
		{
			aimActive = !aimActive;
			time = clock();
		}

		if(GetAsyncKeyState(VK_F3) && clock() - time > 1000)
		{
			autoTriggerStatus = !autoTriggerStatus;
			time = clock();
		}

		if(GetAsyncKeyState(VK_F4) && clock() - time > 1000)
		{
			noBangStatus = !noBangStatus;
			time = clock();
		}

		if(GetAsyncKeyState(VK_F5) && clock() - time > 1000)
		{
			noSmogStatus = !noSmogStatus;
			time = clock();
		}

		if(GetAsyncKeyState(VK_F6) && clock() - time > 1000)
		{
			noRecoilStatus = !noRecoilStatus;
			time = clock();
		}

		if(aquireAddresses)
		{
			_cprintf("[F1] Load Addresses: ON\n");
		}
		else
		{
			_cprintf("[F1] Load Addresses: OFF\n");
		}
		if(aimActive)
		{
			_cprintf("[F2] Aimbot: ON\n");
		}
		else
		{
			_cprintf("[F2] Aimbot: OFF\n");
		}
		if(autoTriggerStatus)
		{
			_cprintf("[F3] Auto Trigger: ON\n");
		}
		else
		{
			_cprintf("[F3] Auto Trigger: OFF\n");
		}

		if(noBangStatus)
		{
			_cprintf("[F4] No Bang: ON\n");
		}
		else
		{
			_cprintf("[F4] No Bang: OFF\n");
		}

		if(noSmogStatus)
		{
			_cprintf("[F5] No Smog: ON\n");
		}
		else
		{
			_cprintf("[F5] No Smog: OFF\n");
		}

		if(noRecoilStatus)
		{
			_cprintf("[F6] No Recoil: ON\n");
		}
		else
		{
			_cprintf("[F6] No Recoil: OFF\n");
		}

		if(aquireAddresses)
		{
			DWORD mouseBaseAddress = (DWORD)GetModuleHandleA("engine.dll") + 0x466E8C;
			mousePosition = (MousePosition*) mouseBaseAddress;

			DWORD BaseAddress = (DWORD)GetModuleHandleA("client.dll") + 0x539984;
			mainPlayer = *((MainPlayer**)BaseAddress);

			DWORD playerArrayPointer = (DWORD)GetModuleHandleA("client.dll") + 0x546204;
			playerPointers =  (PlayerPointers*) playerArrayPointer;

			DWORD autoTriggerBase = (DWORD)GetModuleHandleA("client.dll") + 0x565498;
			autoTrigger =  (AutoTrigger*) autoTriggerBase;

			DWORD bHopBase = (DWORD)GetModuleHandleA("client.dll") + 0x5654A4;
			bHop = (PlayerJump*) bHopBase;



			if(playerPointers->PlayerArray[closestEnemy]->isDead)
			{
				targetLock = false;
			}

			if(!targetLock)
			{
				closestEnemyDistance = 9999;
				enemyFound = false;

				int i = 0;

				while(i < 128)
				{
					// Is the pointer valid and is the pointer not pointing at me
					if(!(playerPointers->PlayerArray[i] == NULL) && playerPointers->PlayerArray[i]->posX != mainPlayer->posX)
					{
						//Is the player dead/has hp or is the enmy on my team
						if(playerPointers->PlayerArray[i]->isDead == 0 && playerPointers->PlayerArray[i]->health > 0 && mainPlayer->team != playerPointers->PlayerArray[i]->team)
						{
							//Is the enemy moving? (This solves "ghost bodies" issue
							//Should add lastEnemyStat with xpos, mouseX, mouseY
							if(lastEnemyState[i/4].posX != playerPointers->PlayerArray[i]->posX)
							{
								if(Get3dDistance(mainPlayer, playerPointers->PlayerArray[i]) < closestEnemyDistance)
								{
									closestEnemyDistance = Get3dDistance(mainPlayer, playerPointers->PlayerArray[i]);
									closestEnemy = i;
									enemyFound = true;

									lastEnemyState[i/4].posX = playerPointers->PlayerArray[i]->posX;
									//lastEnemyState[i/4].mouseY = playerPointers->PlayerArray[i]->mouseY;
									//lastEnemyState[i/4].mouseX = playerPointers->PlayerArray[i]->mouseX;
								}
							}
						}
					}
					i = i+4;
				}
			}
		}
			
			if(aimActive && aquireAddresses)
		{

			if(GetAsyncKeyState(VK_MBUTTON) && enemyFound)
			{
					targetLock = true;
					crouchOffset = 0;

					if(playerPointers->PlayerArray[closestEnemy]->bodyState == 7 || playerPointers->PlayerArray[closestEnemy]->bodyState == 5)
					{
						crouchOffset = -16;
					}

					// Up/down motion
					pitch = -((float)asin((((playerPointers->PlayerArray[closestEnemy]->posZ + crouchOffset) - mainPlayer->posZ)) / Get3dDistance(mainPlayer, playerPointers->PlayerArray[closestEnemy]))
						* 180 / PI);
			
					// left/right motion
					yaw = -atan2f(playerPointers->PlayerArray[closestEnemy]->posX - mainPlayer->posX, playerPointers->PlayerArray[closestEnemy]->posY - mainPlayer->posY)
						* 180 / PI + 90.0f;

					if(yaw > 180)
					{
						yaw -= 360; 
					}
					else if(yaw < -180)
					{
						yaw += 360;
					}

					mousePosition->mouseY = pitch;
					mousePosition->mouseX = yaw;

			}

			else
			{
				targetLock = false;
			}
		
			if(enemyFound)
			{
				if(mainPlayer->lookingAtPlayer > 0 && mainPlayer->team != playerPointers->PlayerArray[closestEnemy]->team)
				{
					_cprintf("Looking at enemy: TRUE\n");
				}
				else
				{
					_cprintf("Looking at enemy: FALSE\n\n");
				}
				_cprintf("Enemy Stats: \n\n");
				_cprintf("Your body state: %x\n", mainPlayer->bodyState);
				_cprintf("Your jump value: %i\n", bHop->jump);
				_cprintf("Health:  %i\n", playerPointers->PlayerArray[closestEnemy]->health);
				_cprintf("BodyState: %x\n\n", playerPointers->PlayerArray[closestEnemy]->bodyState);
				_cprintf("Team: %x\n", playerPointers->PlayerArray[closestEnemy]->team);
			}
			
		}

		if(aquireAddresses && autoTriggerStatus && mainPlayer->lookingAtPlayer > 0 && mainPlayer->team != playerPointers->PlayerArray[closestEnemy]->team)
		{
			autoTrigger->shoot = 5;
		}
		else if(aquireAddresses && autoTriggerStatus && mainPlayer->lookingAtPlayer == 0)
		{
			autoTrigger->shoot = 4;
		}

		if(aquireAddresses && noBangStatus)
		{
			mainPlayer->flashAlpha = 0;
		}
		else if(aquireAddresses && !noBangStatus)
		{
			mainPlayer->flashAlpha = 255;
		}

		if(aquireAddresses && noRecoilStatus)
		{

		}

		if(aquireAddresses && mainPlayer->bodyState == 0)
		{
			bHop->jump = 4;
		}

		if(aquireAddresses && GetAsyncKeyState(VK_SPACE) && mainPlayer->bodyState == 1)
		{
			bHop->jump = 5;	
		}

		system("cls");
	}
	return NULL;
}

float Get3dDistance(MainPlayer* me, PlayerObject* otherPlayer)
{
	return (float) (sqrt(((me->posX - otherPlayer->posX) * (me->posX - otherPlayer->posX)) + ((me->posY - otherPlayer->posY) * (me->posY - otherPlayer->posY)) + ((me->posZ - otherPlayer->posZ) * (me->posZ - otherPlayer->posZ))));
}
