//----------------------------------------------------------------------------
//                  CS 215 - Project 2 - MiniMud
//----------------------------------------------------------------------------
// Author: Ethan Gamble
// Date:  3/19/2021
// Description: A MiniMud game where the player has to find the correct item and place them in the win room
//----------------------------------------------------------------------------

#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include "gameutils.h"
using namespace std;

//----------------------------------------------------------------------------
//                          printLogo
//----------------------------------------------------------------------------
void printLogo() {
	cout << "+--------------------+\n";
	cout << "|     MiniMUD game   |\n";
	cout << "|   by: Ethan Gamble |\n";
	cout << "+--------------------+\n";
}//printLogo

//----------------------------------------------------------------------------
//                          getInputFileName
//----------------------------------------------------------------------------
string getInputFileName() {
	string fileName;
	cout << "Enter the name of game to play: ";
	getline(cin, fileName);
	fileName = fileName + ".txt";
	return fileName;
}//getInputFileName

//----------------------------------------------------------------------------
//                            initItems
//----------------------------------------------------------------------------
void initItems(item a[], int & numItems) {
	for (int i = 0; i < MAX_ITEMS; i++) {
		a[i].shortDesc = " ";
		a[i].longDesc = " ";
	}
	numItems = 0;
}//initItems

//----------------------------------------------------------------------------
//                           initRooms
//----------------------------------------------------------------------------
void initRooms(room a[]) {
	for (int i = 0; i < MAX_ROOMS; i++) {
		a[i].shortDesc = " ";
		a[i].longDesc = " ";
		a[i].north = NO_EXIT;
		a[i].south = NO_EXIT;
		a[i].west = NO_EXIT;
		a[i].east = NO_EXIT;
		initItems(a[i].items, a[i].numItems);
	}
}//initRooms

//----------------------------------------------------------------------------
//                          initWinData
//----------------------------------------------------------------------------
void initWinData(winDataStruct &w) {
		w.winMessage = " ";
		w.winRoom = NO_EXIT;
		initItems(w.winItem, w.numWinItems);
}//initWinData

//----------------------------------------------------------------------------
//                          addItem 
//----------------------------------------------------------------------------
void addItem(item newItem, item a[], int& numItems) {
	string message = "addItem: maximum number of items exceeded!";
	if (numItems < MAX_ITEMS) {
		numItems++;
		a[numItems - 1] = newItem;
	}
	else {
		gameAbort(message);
	}
}//addItem

//----------------------------------------------------------------------------
//                        addItemOverload
//----------------------------------------------------------------------------
void addItem(string shortDesc, string longDesc, item a[], int& numItems) {
	item newItem;
	newItem.shortDesc = shortDesc;
	newItem.longDesc = longDesc;
	addItem(newItem, a, numItems);
}// addItemOverload

//----------------------------------------------------------------------------
//                           doLook
//----------------------------------------------------------------------------
void doLook(room a, bool doShort) {
	if (doShort) {
		cout << a.shortDesc << endl;
	}
	else {
		cout << a.longDesc << endl;
	}
	if (a.numItems == 1) {
		cout << "You notice the following items: " << a.items[0].shortDesc << "." << endl;
	}
	else if (a.numItems > 1) {
		cout << "You notice the following: ";
		for (int i = 0; i < a.numItems - 1; i++) {
			cout << a.items[i].shortDesc << ", ";
		}
		cout << a.items[a.numItems - 1].shortDesc << "." << endl;
	}
}//doLook

//----------------------------------------------------------------------------
//                          doLookOverload
//----------------------------------------------------------------------------
void doLook(room a) {
	doLook(a, false);
}

//----------------------------------------------------------------------------
//							makeExitList
//----------------------------------------------------------------------------
string makeExitList(room thisRoom) {
	string exitList = "You can go:";
	if (thisRoom.north != NO_EXIT) exitList += " north,";
	if (thisRoom.south != NO_EXIT) exitList += " south,";
	if (thisRoom.east != NO_EXIT) exitList += " east,";
	if (thisRoom.west != NO_EXIT) exitList += " west.";
	exitList[exitList.length() - 1] = '.'; // change last comma to a period
	return exitList;
} // makeExitList()

//----------------------------------------------------------------------------
//							loadData
//----------------------------------------------------------------------------
void loadData(room rooms[], int& numRooms, winDataStruct& winData) {
	// variable declarations
	ifstream f;				// input file handle
	string tag;				// first word extracted from each line
	string filename;  // name of input file
	string data;			// data read from file using cleanGetline()
	int roomNdx = -1;		// index of the "current room" being read/populated
							// this should always be rumRooms-1 
	// initialize the rooms and winData
	initRooms(rooms);
	initWinData(winData);
	// get input file name and open the input file; abort on failure
	filename = getInputFileName();
	f.open(filename);
	if (f.fail()) {
		gameAbort("Unable to open: ");
		cout << filename;
	}

	// read line-by-line and store data in the game data structures
	f >> tag;
	while (tag != "END:") {
		if (tag == "ROOM:") {
			numRooms++;		// starting a new room
			roomNdx++;		// increment every time numRooms is incremented
			cleanGetline(f, rooms[roomNdx].shortDesc); // short desc on remainder of ROOM: line
			cleanGetline(f, rooms[roomNdx].longDesc);  // long desc on next line by itself (no tag)
		}
		else if (tag == "ITEM:") {
			string shortDesc, longDesc;
			cleanGetline(f, shortDesc);
			cleanGetline(f, longDesc);
			addItem(shortDesc, longDesc, rooms[roomNdx].items, rooms[roomNdx].numItems);
		}
		else if (tag == "WIN_ITEM:") {
			cleanGetline(f, data);
			addItem(data, "", winData.winItem, winData.numWinItems);
		}
		else if (tag == "NORTH:")
			f >> rooms[roomNdx].north;
		else if (tag == "SOUTH:")
			f >> rooms[roomNdx].south;
		else if (tag == "EAST:")
			f >> rooms[roomNdx].east;
		else if (tag == "WEST:")
			f >> rooms[roomNdx].west;
		else if (tag == "WIN_ROOM:")
			f >> winData.winRoom;
		else if (tag == "REMARK:")
			cleanGetline(f, data);		// data is junk, throw it away
		else if (tag == "WIN_TEXT:")
			cleanGetline(f, winData.winMessage);
		else if (tag == "END:")
			f.close();
		else
			gameAbort("Unknown tag in " + filename + ": " + tag);

		// check for read failure and read the next tag
		if (f.fail())
			gameAbort("Failure while reading input file " + filename);
		f >> tag;
	}
} // loadData()

//----------------------------------------------------------------------------
//                          startUp
//----------------------------------------------------------------------------
void startUp(room rooms[], int& numRooms, item items[], int& numItems, winDataStruct& w) {
	printLogo();
	printHelp();
	loadData(rooms, numRooms, w);
	initItems(items, numItems);
	cout << "You fall asleep and dream of a place far away...\n";
	doLook(rooms[START_ROOM], false);
	cout << makeExitList(rooms[START_ROOM]) << endl;
}//startUp

//----------------------------------------------------------------------------
//                          removeItem
//----------------------------------------------------------------------------
item removeItem(int idx, item a[], int & numItems) {
	string message = "removeItem: invalid index = " + idx;
	item copy;
	if (idx >= 0 && idx < numItems) {
		copy = a[idx];
		numItems--;
		for (int i = idx; i < numItems; i++) {
			a[i] = a[i + 1];
		}
	}
	else {
		gameAbort(message);
	}
	return copy;
}//removeItem

//----------------------------------------------------------------------------
//                             findItem
//----------------------------------------------------------------------------
int findItem(string itemName, item a[], int numItems) {
	int idx = NOT_FOUND;
	for (int i = 0; i < numItems; i++) {
		if (a[i].shortDesc == itemName) {
			return i;
		}
	}
	return idx;
}//findItem

//----------------------------------------------------------------------------
//                          doExamine
//----------------------------------------------------------------------------
void doExamine(string name, item roomInventory[], int numRoomItems, item player[], int numItems) {
	int idx;
	idx = findItem(name, roomInventory, numRoomItems);
	if (idx > -1) {
		cout << roomInventory[idx].longDesc << endl;
	}
	else{
		idx = findItem(name, player, numItems);
		if (idx > -1) {
			cout << player[idx].longDesc << endl;
		}
		else {
			cout << "You see no " << name << endl;
		}
	}
}//doExamine

//----------------------------------------------------------------------------
//                            doTake
//----------------------------------------------------------------------------
void doTake(string itemName, item roomInventory[], int & numRoomItems, item player[], int & numItems) {
	int idx;
	item item;
	idx = findItem(itemName, roomInventory, numRoomItems);
	if (idx > -1) {
		if (numItems < MAX_ITEMS) {
			item = removeItem(idx, roomInventory, numRoomItems);
			addItem(item, player, numItems);
			cout << "You take the " << itemName << endl;
		}
		else {
			cout << "Your hands are full, you can't take that." << endl;
		}
	}
	else {
		idx = findItem(itemName, player, numItems);
		if (idx > -1) {
			cout << "You already have the " << itemName << endl;
		}
		else {
			cout << "You see no " << itemName << endl;
		}
	}
}//doTake

//----------------------------------------------------------------------------
//                            doDrop
//----------------------------------------------------------------------------
void doDrop(string itemName, item roomInventory[], int& numRoomItems, item player[], int& numItems) {
	int idx;
	item item;
	idx = findItem(itemName, player, numItems);
	if (idx > -1) {
		if (numItems <= MAX_ITEMS) {
			item = removeItem(idx, player, numItems);
			addItem(item.shortDesc,item.longDesc, roomInventory, numRoomItems);
			cout << "You drop the " << itemName << endl;
		}
		else {
			cout << "The room is full of junk; you can't drop that.";
		}
	}
	else {
		idx = findItem(itemName, roomInventory, numRoomItems);
		if (idx != -1) {
			cout << "You don't have the " << itemName << endl;
		}
		else {
			cout << "You have no " << itemName << endl;
		}
	}
}//doDrop

//----------------------------------------------------------------------------
//                            getCmd
//----------------------------------------------------------------------------
string getCmd(string & remainder) {
	string input;
	cout << "===> ";
	if (cin.peek() == '\n') cin.ignore();
	getline(cin, input);
	string cmd = split(input, remainder);
	return cmd;
}//getCmd

//----------------------------------------------------------------------------
//							doInventory
//----------------------------------------------------------------------------
// Given: the player's inventory (partial array of items)
// This is the game's response to the "inv"/"i" (inventory) command.
// Prints a message telling the player what he/she is holding.
//----------------------------------------------------------------------------
void doInventory(item item[], int numItems) {
	if (numItems == 0)
		cout << "You don't have anything.\n";
	else {
		cout << "You are holding: ";
		for (int i = 0; i < (numItems - 1); i++)		// for all but last item
			cout << item[i].shortDesc << ", ";			// print short desc with comma
		cout << item[numItems - 1].shortDesc << ".\n";  // for last item, use period and newline
	}
} // doInventory()

//----------------------------------------------------------------------------
//							goNorth
//----------------------------------------------------------------------------
// Given:    partial array of rooms
// Modifies: index of current room
// This is the game's response to the player entering the "north"/"n" command.
// When the north exit exists, changes the current room index and performs
// a "look" command with a short room description
void goNorth(int& currentRoom, room rooms[]) {
	if (rooms[currentRoom].north == NO_EXIT)
		cout << "You can't go north.\n";
	else {
		currentRoom = rooms[currentRoom].north;
		doLook(rooms[currentRoom], true); // true = show short room desc.
	}
} // goNorth()

//----------------------------------------------------------------------------
//                       goSouth
//----------------------------------------------------------------------------
void goSouth(int& currentRoom, room rooms[]) {
	if (rooms[currentRoom].south == NO_EXIT) {
		cout << "You can't go south.\n";
	}
	else {
		currentRoom = rooms[currentRoom].south;
		doLook(rooms[currentRoom], true);
	}
}//goSouth

//----------------------------------------------------------------------------
//                     goEast
//----------------------------------------------------------------------------
void goEast(int& currentRoom, room rooms[]) {
	if (rooms[currentRoom].east == NO_EXIT) {
		cout << "You can't go east.\n";
	}
	else {
		currentRoom = rooms[currentRoom].east;
		doLook(rooms[currentRoom], true);
	}
}//goEast

//----------------------------------------------------------------------------
//                      goWest
//----------------------------------------------------------------------------
void goWest(int& currentRoom, room rooms[]) {
	if (rooms[currentRoom].west == NO_EXIT) {
		cout << "You can't go west.\n";
	}
	else {
		currentRoom = rooms[currentRoom].west;
		doLook(rooms[currentRoom], true);
	}
}//goWest

//----------------------------------------------------------------------------
//                           gameWon
//----------------------------------------------------------------------------
bool gameWon(winDataStruct data, room rooms[]) {
	int idx;
	int item = 0;
	bool win = false;
	for (int i = 0; i < data.numWinItems; i++) {
		idx = findItem(data.winItem[i].shortDesc, rooms[data.winRoom].items, rooms[data.winRoom].numItems);
		if (idx != -1) {
			item++;
		}
	}
	if (item == data.numWinItems) {
		win = true;
	}
	return win;
}//gameWon

//----------------------------------------------------------------------------
//							main
//----------------------------------------------------------------------------
int main() {
	// declare data structures
	room rooms[MAX_ROOMS];
	int numRooms = 0, numItems, roomidx = 0;
	item items[MAX_ITEMS];
	winDataStruct winData;
	int currentRoom = START_ROOM;
	// declare other variables
	string cmd;					// command entered by user (first word of input)
	string remainder;			// remainder of command entered by user
	bool   gameWasWon = gameWon(winData,rooms);	// check with gameWon() as needed
	// start up: load game data from file and initialize player inventory
	startUp(rooms,numRooms,items, numItems, winData);
	// get first command input
	cmd = getCmd(remainder);

	// repeat until "exit" entered or the game is won
	while (cmd != "exit" && !gameWasWon) {
		if (cmd == "help")
			printHelp();
		// add cases for other commands and invocations
		else if (cmd == "look" || cmd == "l") {
			doLook(rooms[roomidx], false);
		}
		else if (cmd == "inv" || cmd == "i") {
			doInventory(items, numItems);
		}
		else if (cmd == "exa" || cmd == "x") {
			doExamine(remainder, rooms[roomidx].items, rooms[roomidx].numItems, items, numItems);
		}
		else if (cmd == "take" || cmd == "t") {
			doTake(remainder, rooms[roomidx].items, rooms[roomidx].numItems, items, numItems);
		}
		else if (cmd == "drop" || cmd == "d") {
			doDrop(remainder, rooms[roomidx].items, rooms[roomidx].numItems, items, numItems);
		}
		else if (cmd == "north" || cmd == "n") {
			goNorth(roomidx, rooms);
			cout << makeExitList(rooms[roomidx]) << endl;
		}
		else if (cmd == "south" || cmd == "s") {
			goSouth(roomidx, rooms);
			cout << makeExitList(rooms[roomidx]) << endl;
		}
		else if (cmd == "east" || cmd == "e") {
			goEast(roomidx, rooms);
			cout << makeExitList(rooms[roomidx]) << endl;
		}
		else if (cmd == "west" || cmd == "w") {
			goWest(roomidx, rooms);
			cout << makeExitList(rooms[roomidx]) << endl;
		}
		else // the user entered an unrecognized command
			cout << "What??\n";

		// check if game was won and print win message or get next user input
		gameWasWon = gameWon(winData, rooms);
		if (gameWasWon)
			cout << winData.winMessage << endl;
		else
			cmd = getCmd(remainder);
	}

	system("pause");
	return 0;
}