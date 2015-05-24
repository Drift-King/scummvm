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

#ifndef SHERLOCK_OBJECTS_H
#define SHERLOCK_OBJECTS_H

#include "common/scummsys.h"
#include "common/rect.h"
#include "common/str-array.h"
#include "common/str.h"
#include "sherlock/resources.h"

namespace Sherlock {

class SherlockEngine;

enum ObjectAllow {
	ALLOW_MOVE = 1, ALLOW_OPEN = 2, ALLOW_CLOSE = 4
};

enum SpriteType {
	INVALID = 0,
	CHARACTER = 1,
	CURSOR = 2,
	STATIC_BG_SHAPE = 3,		// Background shape that doesn't animate
	ACTIVE_BG_SHAPE = 4,		// Background shape that animates
	REMOVE = 5,					// Object should be removed next frame
	NO_SHAPE = 6,				// Background object with no shape
	HIDDEN = 7,					// Hidden backgruond object
	HIDE_SHAPE = 8,				// Object needs to be hidden

	// Rose Tattoo
	HIDDEN_CHARACTER = 128
};

enum AType {
	OBJECT		= 0,
	PERSON		= 1,
	SOLID		= 2,
	TALK		= 3,  // Standard talk zone
	FLAG_SET	= 4,
	DELTA		= 5,
	WALK_AROUND	= 6,
	TALK_EVERY	= 7,  // Talk zone that turns on every room visit
	TALK_MOVE	= 8,  // Talk zone that only activates when Holmes moves
	PAL_CHANGE	= 9,  // Changes the palette down so that it gets darker
	PAL_CHANGE2	= 10, // Same as PAL_CHANGE, except that it goes up
	SCRIPT_ZONE	= 11, // If this is clicked in, it is activated
	BLANK_ZONE	= 12, // This masks out other objects when entered
	NOWALK_ZONE = 13  // Player cannot walk here
};

// Different levels for sprites to be at
enum {
	BEHIND = 0, NORMAL_BEHIND = 1, NORMAL_FORWARD = 2, FORWARD = 3
};

#define MAX_HOLMES_SEQUENCE 16
#define MAX_FRAME 30

// code put into sequences to defines 1-10 type seqs
#define SEQ_TO_CODE 67
#define FLIP_CODE (64 + 128)
#define SOUND_CODE (34 + 128)

class Point32 {
public:
	int x;
	int y;

	Point32() : x(0), y(0) {}
	Point32(int x1, int y1) : x(x1), y(y1) {}
	Point32(const Common::Point &pt) : x(pt.x), y(pt.y) {}

	bool operator==(const Point32 &p) const { return x == p.x && y == p.y; }
	bool operator!=(const Point32 &p) const { return x != p.x || y != p.y; }
	Point32 operator+(const Point32 &delta) const { return Point32(x + delta.x, y + delta.y); }
	Point32 operator-(const Point32 &delta) const { return Point32(x - delta.x, y - delta.y); }
	operator Common::Point() { return Common::Point(x, y); }

	void operator+=(const Point32 &delta) { x += delta.x; y += delta.y; }
	void operator-=(const Point32 &delta) { x -= delta.x; y -= delta.y; }
};


struct WalkSequence {
	Common::String _vgsName;
	bool _horizFlip;
	Common::Array<byte> _sequences;

	WalkSequence() : _horizFlip(false) {}
	const byte &operator[](int idx) { return _sequences[idx]; }

	/**
	 * Load data for the sequence from a stream
	 */
	void load(Common::SeekableReadStream &s);
};

class Sprite {
private:
	static SherlockEngine *_vm;
public:
	Common::String _name;				
	Common::String _description;		
	Common::String _examine;			// Examine in-depth description
	Common::String _pickUp;				// Message for if you can't pick up object

	Common::Array<WalkSequence> _walkSequences;	// Holds animation sequences
	ImageFile *_images;					// Sprite images
	ImageFrame *_imageFrame;			// Pointer to shape in the images
	int _walkCount;						// Character walk counter
	int _allow;							// Allowed menu commands - ObjectAllow
	int _frameNumber;					// Frame number in rame sequence to draw
	int _sequenceNumber;				// Sequence being used
	Point32 _position;					// Current position
	Point32 _delta;						// Momvement delta
	Common::Point _oldPosition;			// Old position
	Common::Point _oldSize;				// Image's old size
	Common::Point _goto;				// Walk destination
	SpriteType _type;					// Type of object
	Common::Point _noShapeSize;			// Size of a NO_SHAPE
	int _status;						// Status: open/closed, moved/not moved
	int8 _misc;							// Miscellaneous use
	int _numFrames;						// How many frames the object has
	ImageFile *_altImages;
	bool _altSequences;
	ImageFrame *_stopFrames[8];			// Stop/rest frame for each direction
public:
	Sprite() { clear(); }

	static void setVm(SherlockEngine *vm) { _vm = vm; }

	/**
	 * Reset the data for the sprite
	 */
	void clear();

	/**
	 * Updates the image frame poiner for the sprite
	 */
	void setImageFrame();

	/**
	 * This adjusts the sprites position, as well as it's animation sequence:
	 */
	void adjustSprite();

	/**
	 * Checks the sprite's position to see if it's collided with any special objects
	 */
	void checkSprite();

	/**
	 * Return frame width
	 */
	int frameWidth() const { return _imageFrame ? _imageFrame->_frame.w : 0; }
	
	/**
	 * Return frame height
	 */
	int frameHeight() const { return _imageFrame ? _imageFrame->_frame.h : 0; }
};

enum { REVERSE_DIRECTION = 0x80 };
#define NAMES_COUNT 4

struct ActionType {
	int _cAnimNum;
	int _cAnimSpeed;
	Common::String _names[NAMES_COUNT];

	/**
	 * Load the data for the action
	 */
	void load(Common::SeekableReadStream &s);
};

struct UseType {
	int _cAnimNum;
	int _cAnimSpeed;
	Common::String _names[NAMES_COUNT];
	int _useFlag;					// Which flag USE will set (if any)
	Common::String _target;
	Common::String _verb;

	UseType();

	/**
	 * Load the data for the UseType
	 */
	void load(Common::SeekableReadStream &s, bool isRoseTattoo);
};

enum { OBJ_BEHIND = 1, OBJ_FLIPPED = 2, OBJ_FORWARD = 4, TURNON_OBJ = 0x20, TURNOFF_OBJ = 0x40 };
#define USE_COUNT 4

class Object {
private:
	static SherlockEngine *_vm;

	/**
	 * This will check to see if the object has reached the end of a sequence.
	 * If it has, it switch to whichever next sequence should be started.
	 * @returns		true if the end of a sequence was reached
	 */
	bool checkEndOfSequence();

	/**
	 * Scans through the sequences array and finds the designated sequence.
	 * It then sets the frame number of the start of that sequence
	 */
	void setObjSequence(int seq, bool wait);
public:
	static bool _countCAnimFrames;

	static void setVm(SherlockEngine *vm);
public:
	Common::String _name;			// Name
	Common::String _description;	// Description lines
	Common::String _examine;		// Examine in-depth description
	int _sequenceOffset;
	uint8 *_sequences;				// Holds animation sequences
	ImageFile *_images;				// Sprite images
	ImageFrame *_imageFrame;		// Pointer to shape in the images
	int _walkCount;					// Character walk counter
	int _allow;						// Allowed menu commands - ObjectAllow
	int _frameNumber;				// Frame number in rame sequence to draw
	int _sequenceNumber;			// Sequence being used
	SpriteType _type;				// Object type
	Common::Point _position;		// Current position
	Common::Point _delta;			// Momvement amount
	Common::Point _oldPosition;		// Old position
	Common::Point _oldSize;			// Image's old size
	Common::Point _goto;			// Walk destination

	int _pickup;
	int _defaultCommand;			// Default right-click command
	int _lookFlag;					// Which flag LOOK   will set (if any)
	int _requiredFlag;				// Object will be hidden if not set
	Common::Point _noShapeSize;		// Size of a NO_SHAPE
	int _status;					// Status (open/closed, moved/not)
	int8 _misc;						// Misc field -- use varies with type
	int _maxFrames;					// Number of frames
	int _flags;						// Tells if object can be walked behind
	AType _aType;					// Tells if this is an object, person, talk, etc.
	int _lookFrames;				// How many frames to play of the look anim before pausing
	int _seqCounter;				// How many times this sequence has been executed
	Common::Point _lookPosition;	// Where to walk when examining object
	int _lookFacing;				// Direction to face when examining object
	int _lookcAnim;
	int _seqStack;					// Allows gosubs to return to calling frame
	int _seqTo;						// Allows 1-5, 8-3 type sequences encoded in 2 bytes
	uint _descOffset;					// Tells where description starts in DescText
	int _seqCounter2;				// Counter of calling frame sequence
	uint _seqSize;					// Tells where description starts
	UseType _use[6];				// Serrated Scalpel uses 4, Rose Tattoo 6

	// Serrated Scalpel fields
	int _pickupFlag;				// Which flag PICKUP will set (if any)
	ActionType _aOpen;				// Holds data for moving object
	ActionType _aClose;
	ActionType _aMove;

	// Rose Tattoo fields
	int _quickDraw;
	int _scaleVal;
	int _requiredFlag1;
	int _gotoSeq;
	int _talkSeq;
	int _restoreSlot;

	Object();

	/**
	 * Load the data for the object
	 */
	void load(Common::SeekableReadStream &s, bool isRoseTattoo);

	/**
	 * Toggle the type of an object between hidden and active
	 */
	void toggleHidden();

	/**
	 * Check the state of the object
	 */
	void checkObject();

	/**
	 * Checks for codes
	 * @param name		The name to check for codes
	 * @param messages	Provides a lookup list of messages that can be printed
	 * @returns		0 if no codes are found, 1 if codes were found
	 */
	int checkNameForCodes(const Common::String &name, const char *const messages[]);

	/**
	 * Handle setting any flags associated with the object
	 */
	void setFlagsAndToggles();

	/**
	 * Adjusts the sprite's position and animation sequence, advancing by 1 frame.
	 * If the end of the sequence is reached, the appropriate action is taken.
	 */
	void adjustObject();

	/**
	 * Handles trying to pick up an object. If allowed, plays an y necessary animation for picking
	 * up the item, and then adds it to the player's inventory
	 */
	int pickUpObject(const char *const messages[]);

	/**
	 * Return the frame width
	 */
	int frameWidth() const { return _imageFrame ? _imageFrame->_frame.w : 0; }
	
	/**
	 * Return the frame height
	 */
	int frameHeight() const { return _imageFrame ? _imageFrame->_frame.h : 0; }

	/**
	 * Returns the current bounds for the sprite
	 */
	const Common::Rect getNewBounds() const;

	/**
	 * Returns the bounds for a sprite without a shape
	 */
	const Common::Rect getNoShapeBounds() const;

	/**
	 * Returns the old bounsd for the sprite from the previous frame
	 */
	const Common::Rect getOldBounds() const;
};

struct CAnim {
	Common::String _name;			// Name
	Common::Point _position;		// Position
	int _size;						// Size of uncompressed animation
	int _flags;						// Tells if can be walked behind
	Common::Point _goto;			// coords holmes should walk to before starting canim
	int _gotoDir;
	Common::Point _teleportPos;		// Location Holmes shoul teleport to after
	int _teleportDir;					// playing canim

	// Scalpel specific
	byte _sequences[MAX_FRAME];		// Animation sequences
	SpriteType _type;

	// Rose Tattoo specific
	int _scaleVal;					// How much the canim is scaled

	/**
	 * Load the data for the animation
	 */
	void load(Common::SeekableReadStream &s, bool isRoseTattoo);
};

struct SceneImage {
	ImageFile *_images;				// Object images
	int _maxFrames;					// How many frames in object
	int _filesize;					// File size

	SceneImage();
} ;

} // End of namespace Sherlock

#endif
