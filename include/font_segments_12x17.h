// Font "Segments" optimised for U8g2lib.
//
// Copyright (c) 2023 Jan Lindblom (janlindblom@fastmail.fm)
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#pragma once

#include <Arduino.h>
#include <U8g2lib.h>

/*
  The following #defines are sections copied from:

  Universal 8bit Graphics Library (https://github.com/olikraus/u8g2/)

  Copyright (c) 2016, olikraus@gmail.
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

  * Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
  CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
  NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef U8X8_SECTION
#  ifdef __GNUC__
#    define U8X8_SECTION(name) __attribute__((section(name)))
#  else
#    define U8X8_SECTION(name)
#  endif
#endif

#if !defined(U8X8_FONT_SECTION) && defined(__GNUC__) && defined(__AVR__)
#  define U8X8_FONT_SECTION(name) U8X8_SECTION(".progmem." name)
#elif defined(ESP8266)
#  define U8X8_FONT_SECTION(name) __attribute__((section(".text." name)))
#endif

#ifndef U8X8_FONT_SECTION
#  define U8X8_FONT_SECTION(name)
#endif

#define U8G2_FONT_SECTION(name) U8X8_FONT_SECTION(name)

/*
  Fontname: Segments_12x17
  Copyright: Jan Lindblom
  Glyphs: 20/145
  BBX Build Mode: 0
*/

// clang-format off
const uint8_t font_segments_12x17[345] U8G2_FONT_SECTION("font_segments_12x17") =
  "\24\0\4\3\4\5\4\5\5\11\21\0\377\17\0\0\0\0\0\0\0\1< \5\0\20n%\42\11\21"
  "\352\230\232\10\222\20\222\220\231\20\231\230\20\231\20\323bDb\211\304\214\204\220\204P\304\324\4\0+\17"
  "\210\220\346\21S\63q\20\61#\246\6\0,\10\63\360\321\220\240\0-\7(\360fx .\6\42\20"
  "N \60\24\11\21\352\270\210\250\230\342\325\234\334\24\257&*\42.\0\61\13\342>\352\70\240\210\70\240"
  "\10\62\23\11\21\352\270\251\210\323\13\13\11\13\71\335E\324\134\0\63\24\10\23j\70\251\10\323\331AD"
  "DE\230^T\34D\0\64\20\351\60j\210\233\342\325DE\204E\234\36\6\65\24\11\21\352\270\210("
  "\221\323]\204\211E\234\236TD\134\0\66\26\11\21\352\270\210(\221\323]\204ED\305\24\257&*\42"
  ".\0\67\15\370\62j\70\251\10\323\273dz\27\70\26\11\21\352\270\210\250\230\342\325D\205D\305\24\257"
  "&*\42.\0\71\25\11\21\352\270\210\250\230\342\325DE\204E\234\236TD\134\0:\7b\260N "
  "\42C\22\10\21\346\70\210\250\20\323Y\134\234\230\316\42J\16F\22\370\60\346\70\210\250\20\323Y\304E"
  "\205\230\316\342\0\260\14fP\337`\61B\62\301\2\0\0\0\0\4\377\377\0";
//clang-format on
