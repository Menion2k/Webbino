/***************************************************************************
 *   This file is part of Webbino                                          *
 *                                                                         *
 *   Copyright (C) 2012-2018 by SukkoPera                                  *
 *                                                                         *
 *   Webbino is free software: you can redistribute it and/or modify       *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation, either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   Webbino is distributed in the hope that it will be useful,            *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with Webbino. If not, see <http://www.gnu.org/licenses/>.       *
 ***************************************************************************/

#ifndef _WEBSERVER_H_
#define _WEBSERVER_H_

#include <webbino_config.h>
#include "NetworkInterface.h"
#include "HTTPRequestParser.h"
#include "Storage.h"

// http://arduiniana.org/libraries/pstring/
#include <PString.h>

class WebClient;
class Content;

#ifdef ENABLE_TAGS

typedef PString& (*TagEvaluateFn) (void* data);

struct ReplacementTag {
	PGM_P name;				// Max length: MAX_TAG_LEN
	TagEvaluateFn function;
	void *data;

	// Methods that (try to) hide the complexity of accessing PROGMEM data
	PGM_P getName () const {
		return reinterpret_cast<PGM_P> (pgm_read_ptr (&(this -> name)));
	}

	TagEvaluateFn getFunction () const {
		return reinterpret_cast<TagEvaluateFn> (pgm_read_ptr (&(this -> function)));
	}

	void *getData () const {
		return reinterpret_cast<void *> (const_cast<void *> (pgm_read_ptr (&(this -> data))));
	}
};

/* Macros that simplify things
 * Note that max length of tag name is MAX_TAG_LEN (24)
 */
#define REPTAG_STR(tag) #tag
#define REPTAG_STR_VAR(tag) _rtStr_ ## tag
//#define REPTAG_VAR(tag) _rtTag_ ## tag
//#define REPTAG_PTR(tag) &REPTAG_VAR(tag)

#define RepTagWithArg(var, tag, fn, arg) \
                const char REPTAG_STR_VAR(tag)[] PROGMEM = REPTAG_STR(tag); \
                const ReplacementTag var PROGMEM = {REPTAG_STR_VAR(tag), fn, reinterpret_cast<void *> (arg)};

#define RepTagNoArg(var, tag, fn) RepTagWithArg(var, tag, fn, NULL)

// http://stackoverflow.com/questions/11761703/overloading-macro-on-number-of-arguments
#define GET_MACRO(_1, _2, _3, _4, NAME, ...) NAME
#define EasyReplacementTag(...) GET_MACRO (__VA_ARGS__, RepTagWithArg, RepTagNoArg) (__VA_ARGS__)

typedef const ReplacementTag* const EasyReplacementTagArray;

#endif

/******************************************************************************/

const byte MAX_STORAGES = 3;

class WebServer {
private:
	NetworkInterface* netint;

	Storage* storages[MAX_STORAGES];
	byte nStorage;

#ifdef ENABLE_TAGS
	const ReplacementTag* const * substitutions = nullptr;
#endif

	void handleClient (WebClient& client);

	void sendContent (WebClient& client, Content& content);

	PGM_P getContentType (const char* filename);

#ifdef ENABLE_TAGS
	boolean shallReplace (PGM_P contType);

	PString* findSubstitutionTag (const char* tag) const;

	char *findSubstitutionTagGetParameter (HTTPRequestParser& request, const char* tag);
#endif

public:
	boolean begin (NetworkInterface& _netint);

	boolean addStorage (Storage& storage);

#ifdef ENABLE_TAGS
	void enableReplacementTags (const ReplacementTag* const _substitutions[]);
#endif

	boolean loop ();
};

#endif
