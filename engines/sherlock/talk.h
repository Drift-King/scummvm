/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#ifndef SHERLOCK_TALK_H
#define SHERLOCK_TALK_H

#include "common/scummsys.h"
#include "common/array.h"
#include "common/rect.h"
#include "common/serializer.h"
#include "common/stream.h"
#include "common/stack.h"

namespace Sherlock {

#define MAX_TALK_SEQUENCES 11
#define MAX_TALK_FILES 500

enum {
	OP_SWITCH_SPEAKER			= 0,
	OP_RUN_CANIMATION			= 1,
	OP_ASSIGN_PORTRAIT_LOCATION = 2,
	OP_PAUSE					= 3,
	OP_REMOVE_PORTRAIT			= 4,
	OP_CLEAR_WINDOW				= 5,
	OP_ADJUST_OBJ_SEQUENCE		= 6,
	OP_WALK_TO_COORDS			= 7,
	OP_PAUSE_WITHOUT_CONTROL	= 8,
	OP_BANISH_WINDOW			= 9,
	OP_SUMMON_WINDOW			= 10,
	OP_SET_FLAG					= 11,
	OP_SFX_COMMAND				= 12,
	OP_TOGGLE_OBJECT			= 13,
	OP_STEALTH_MODE_ACTIVE		= 14,
	OP_IF_STATEMENT				= 15,
	OP_ELSE_STATEMENT			= 16,
	OP_END_IF_STATEMENT			= 17,
	OP_STEALTH_MODE_DEACTIVATE	= 18,
	OP_TURN_HOLMES_OFF			= 19,
	OP_TURN_HOLMES_ON			= 20,
	OP_GOTO_SCENE				= 21,
	OP_PLAY_PROLOGUE			= 22,
	OP_ADD_ITEM_TO_INVENTORY	= 23,
	OP_SET_OBJECT				= 24,
	OP_CALL_TALK_FILE			= 25,
	OP_MOVE_MOUSE				= 26,
	OP_DISPLAY_INFO_LINE		= 27,
	OP_CLEAR_INFO_LINE			= 28,
	OP_WALK_TO_CANIMATION		= 29,
	OP_REMOVE_ITEM_FROM_INVENTORY = 30,
	OP_ENABLE_END_KEY			= 31,
	OP_DISABLE_END_KEY			= 32,
	OP_CARRIAGE_RETURN			= 33,
	
	OP_MOUSE_OFF_ON				= 34,
	OP_SET_WALK_CONTROL			= 35,
	OP_SET_TALK_SEQUENCE		= 36,
	OP_PLAY_SONG				= 37,
	OP_WALK_HOLMES_AND_NPC_TO_CANIM = 38,
	OP_SET_NPC_PATH_DEST		= 39,
	OP_NEXT_SONG				= 40,
	OP_SET_NPC_PATH_PAUSE		= 41,
	OP_NEED_PASSWORD			= 42,
	OP_SET_SCENE_ENTRY_FLAG		= 43,
	OP_WALK_NPC_TO_CANIM		= 44,
	OP_WALK_HOLMES_AND_NPC_TO_COORDS = 45,
	OP_SET_NPC_TALK_FILE		= 46,
	OP_TURN_NPC_OFF				= 47,
	OP_TURN_NPC_ON				= 48,
	OP_NPC_DESC_ON_OFF			= 49,
	OP_NPC_PATH_PAUSE_TAKING_NOTES	= 50,
	OP_NPC_PATH_PAUSE_LOOKING_HOLMES = 51,
	OP_ENABLE_TALK_INTERRUPTS	= 52,
	OP_DISABLE_TALK_INTERRUPTS	= 53,
	OP_SET_NPC_INFO_LINE		= 54,
	OP_SET_NPC_POSITION			= 54,
	OP_NPC_PATH_LABEL			= 55,
	OP_PATH_GOTO_LABEL			= 56,
	OP_PATH_IF_FLAG_GOTO_LABEL	= 57,
	OP_NPC_WALK_GRAPHICS		= 58,
	OP_NPC_VERB					= 59,
	OP_NPC_VERB_CANIM			= 60,
	OP_NPC_VERB_SCRIPT			= 61,
	OP_RESTORE_PEOPLE_SEQUENCE	= 62,
	OP_NPC_VERB_TARGET			= 63,
	OP_TURN_SOUNDS_OFF			= 64
};

enum OpcodeReturn { RET_EXIT = -1, RET_SUCCESS = 0, RET_CONTINUE = 1 };

class SherlockEngine;
class ScalpelUserInterface;
class Talk;

typedef OpcodeReturn(Talk::*OpcodeMethod)(const byte *&str);

struct SequenceEntry {
	int _objNum;
	Common::Array<byte> _sequences;
	int _frameNumber;
	int _seqTo;

	SequenceEntry();
};

struct ScriptStackEntry {
	Common::String _name;
	int _currentIndex;
	int _select;
};

struct Statement {
	Common::String _statement;
	Common::String _reply;
	Common::String _linkFile;
	Common::String _voiceFile;
	Common::Array<int> _required;
	Common::Array<int> _modified;
	int _portraitSide;
	int _quotient;
	int _talkMap;
	Common::Rect _talkPos;

	/**
	 * Load the data for a single statement within a talk file
	 */
	void synchronize(Common::SeekableReadStream &s);
};

struct TalkHistoryEntry {
	bool _data[16];

	TalkHistoryEntry();
	bool &operator[](int index) { return _data[index]; }
};

struct TalkSequences {
	byte _data[MAX_TALK_SEQUENCES];

	TalkSequences() { clear(); }
	TalkSequences(const byte *data);

	byte &operator[](int idx) { return _data[idx]; }
	void clear();
};

class Talk {
	friend class ScalpelUserInterface;
private:
	/**
	 * Remove any voice commands from a loaded statement list
	 */
	void stripVoiceCommands();
	
	/**
	 * Form a table of the display indexes for statements
	 */
	void setTalkMap();

	/**
	 * Display a list of statements in a window at the bottom of the screen that the
	 * player can select from.
	 */
	bool displayTalk(bool slamIt);

	/**
	 * Prints a single conversation option in the interface window
	 */
	int talkLine(int lineNum, int stateNum, byte color, int lineY, bool slamIt);

	/**
	 * Parses a reply for control codes and display text. The found text is printed within
	 * the text window, handles delays, animations, and animating portraits.
	 */
	void doScript(const Common::String &script);

	/**
	 * When the talk window has been displayed, waits a period of time proportional to
	 * the amount of text that's been displayed
	 */
	int waitForMore(int delay);
protected:
	SherlockEngine *_vm;
	OpcodeMethod *_opcodeTable;
	Common::Stack<SequenceEntry> _savedSequences;
	Common::Stack<SequenceEntry> _sequenceStack;
	Common::Stack<ScriptStackEntry> _scriptStack;
	Common::Array<Statement> _statements;
	TalkHistoryEntry _talkHistory[MAX_TALK_FILES];
	int _speaker;
	int _talkIndex;
	int _scriptSelect;
	int _talkStealth;
	int _talkToFlag;
	int _scriptSaveIndex;

	// These fields are used solely by doScript, but are fields because all the script opcodes are
	// separate methods now, and need access to these fields
	int _yp;
	int _charCount;
	int _line;
	int _wait;
	bool _pauseFlag;
	bool _endStr, _noTextYet;
	int _seqCount;
	const byte *_scriptStart, *_scriptEnd;
protected:
	Talk(SherlockEngine *vm);

	OpcodeReturn cmdAddItemToInventory(const byte *&str);
	OpcodeReturn cmdAdjustObjectSequence(const byte *&str);
	OpcodeReturn cmdBanishWindow(const byte *&str);
	OpcodeReturn cmdDisableEndKey(const byte *&str);
	OpcodeReturn cmdEnableEndKey(const byte *&str);
	OpcodeReturn cmdGotoScene(const byte *&str);
	OpcodeReturn cmdHolmesOff(const byte *&str);
	OpcodeReturn cmdHolmesOn(const byte *&str);
	OpcodeReturn cmdPause(const byte *&str);
	OpcodeReturn cmdPauseWithoutControl(const byte *&str);
	OpcodeReturn cmdRemoveItemFromInventory(const byte *&str);
	OpcodeReturn cmdRunCAnimation(const byte *&str);
	OpcodeReturn cmdSetFlag(const byte *&str);
	OpcodeReturn cmdSetObject(const byte *&str);
	OpcodeReturn cmdStealthModeActivate(const byte *&str);
	OpcodeReturn cmdStealthModeDeactivate(const byte *&str);
	OpcodeReturn cmdSwitchSpeaker(const byte *&str);
	OpcodeReturn cmdToggleObject(const byte *&str);
	OpcodeReturn cmdWalkToCAnimation(const byte *&str);
	OpcodeReturn cmdWalkToCoords(const byte *&str);
public:
	bool _talkToAbort;
	int _talkCounter;
	int _talkTo;
	int _scriptMoreFlag;
	Common::String _scriptName;
	bool _moreTalkUp, _moreTalkDown;
	int _converseNum;
	const byte *_opcodes;

public:
	static Talk *init(SherlockEngine *vm);
	virtual ~Talk() {}

	/**
	 * Return a given talk statement
	 */
	Statement &operator[](int idx) { return _statements[idx]; }

	/**
	 * Called whenever a conversation or item script needs to be run. For standard conversations,
	 * it opens up a description window similar to how 'talk' does, but shows a 'reply' directly
	 * instead of waiting for a statement option.
	 * @remarks		It seems that at some point, all item scripts were set up to use this as well.
	 *	In their case, the conversation display is simply suppressed, and control is passed on to
	 *	doScript to implement whatever action is required.
	 */
	void talkTo(const Common::String &filename);

	/**
	 * Main method for handling conversations when a character to talk to has been
	 * selected. It will make Holmes walk to the person to talk to, draws the
	 * interface window for the conversation and passes on control to give the
	 * player a list of options to make a selection from
	 */
	void talk(int objNum);

	/**
	 * Clear loaded talk data
	 */
	void freeTalkVars();

	/**
	 * Draws the interface for conversation display
	 */
	void drawInterface();

	/**
	 * Opens the talk file 'talk.tlk' and searches the index for the specified
	 * conversation. If found, the data for that conversation is loaded
	 */
	void loadTalkFile(const Common::String &filename);

	/**
	 * Change the sequence of a background object corresponding to a given speaker.
	 * The new sequence will display the character as "listening"
	 */
	void setStillSeq(int speaker);

	/**
	 * Clears the stack of pending object sequences associated with speakers in the scene
	 */
	void clearSequences();
	
	/**
	 * Pulls a background object sequence from the sequence stack and restore's the
	 * object's sequence
	 */
	void pullSequence();

	/**
	 * Push the sequence of a background object that's an NPC that needs to be
	 * saved onto the sequence stack.
	 */
	void pushSequence(int speaker);
	
	/**
	 * Change the sequence of the scene background object associated with the current speaker.
	 */
	void setSequence(int speaker);

	/**
	 * Returns true if the script stack is empty
	 */
	bool isSequencesEmpty() const { return _scriptStack.empty(); }

	/**
	 * Pops an entry off of the script stack
	 */
	void popStack();

	/**
	 * Synchronize the data for a savegame
	 */
	void synchronize(Common::Serializer &s);
};

class ScalpelTalk : public Talk {
protected:
	OpcodeReturn cmdAssignPortraitLocation(const byte *&str);
	OpcodeReturn cmdCallTalkFile(const byte *&str);
	OpcodeReturn cmdClearInfoLine(const byte *&str);
	OpcodeReturn cmdClearWindow(const byte *&str);
	OpcodeReturn cmdDisplayInfoLine(const byte *&str);
	OpcodeReturn cmdElse(const byte *&str);
	OpcodeReturn cmdIf(const byte *&str);
	OpcodeReturn cmdMoveMouse(const byte *&str);
	OpcodeReturn cmdPlayPrologue(const byte *&str);
	OpcodeReturn cmdRemovePortrait(const byte *&str);
	OpcodeReturn cmdSfxCommand(const byte *&str);
	OpcodeReturn cmdSummonWindow(const byte *&str);
	OpcodeReturn cmdCarriageReturn(const byte *&str);
public:
	ScalpelTalk(SherlockEngine *vm);
	virtual ~ScalpelTalk() {}
};

class TattooTalk : public Talk {
public:
	TattooTalk(SherlockEngine *vm);
	virtual ~TattooTalk() {}
};

} // End of namespace Sherlock

#endif
