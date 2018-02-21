/*
 * MenuDef.h
 *
 *  Created on: Jan 24, 2017
 *      Author: jakub
 */
/**
 * definition of MENU items.
 * The lower level needs to be defined as first.
 */
#ifndef MODULE_MENUDEF_H_
#define MODULE_MENUDEF_H_

#include "menu.h"

// submenu

//define all structures first:
const menu_item_t MainMenu;
const menu_item_t setTime;
const menu_item_t setTemp;
const menu_item_t extUSB;
//TIME submenus
const menu_item_t setDate;
const menu_item_t setClock;

/*
 * uint8_t *menuHeader;
	uint8_t numberOfChoices;
	struct menu_item_s *submenu[MAX_MENU_ITEM];
	struct menu_item_s *upmenu;
	EnPushMenu action;
 */


// MAIN MENU - root
const menu_item_t MainMenu = {
		"MENU",
		3,
		{&setTemp,&setTime,&extUSB},
		NULL,
		next
};

const menu_item_t setTime = {
		"TIME",
		2,
		{&setClock,&setDate},
		&MainMenu,
		next
};

const menu_item_t setTemp = {
		"TEMPERATURE",
		0,
		{NULL},
		&MainMenu,
		setTemperature
};
const menu_item_t extUSB = {
		"USB PRINT",
		0,
		{NULL},
		&MainMenu,
		usbPrint
};

/* submenu for TIME */

const menu_item_t setClock = {
		"Clock",
		0,
		{NULL},
		&setTime,
		clock
};

const menu_item_t setDate = {
		"Date",
		0,
		{NULL},
		&setTime,
		date
};

#endif /* MODULE_MENUDEF_H_ */
