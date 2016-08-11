/*
 * Copyright (C) 2003, 2006, 2008, 2011 Apple Inc. All rights reserved.
 * Copyright (C) 2008 Holger Hans Peter Freyther
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#ifndef WidthIterator_h
#define WidthIterator_h

#include "FontCascade.h"
#include "SVGGlyph.h"
#include "TextRun.h"
#include <wtf/HashSet.h>
#include <wtf/Vector.h>

namespace WebCore {

class FontCascade;
class GlyphBuffer;
class Font;
class TextRun;
struct GlyphData;
struct OriginalAdvancesForCharacterTreatedAsSpace;

typedef Vector<std::pair<int, OriginalAdvancesForCharacterTreatedAsSpace>, 64> CharactersTreatedAsSpace;

struct WidthIterator {
    WTF_MAKE_FAST_ALLOCATED;
public:
    WidthIterator(const FontCascade*, const TextRun&, HashSet<const Font*>* fallbackFonts = 0, bool accountForGlyphBounds = false, bool forTextEmphasis = false);

    unsigned advance(int to, GlyphBuffer*);
    bool advanceOneCharacter(float& width, GlyphBuffer&);

    float maxGlyphBoundingBoxY() const { ASSERT(m_accountForGlyphBounds); return m_maxGlyphBoundingBoxY; }
    float minGlyphBoundingBoxY() const { ASSERT(m_accountForGlyphBounds); return m_minGlyphBoundingBoxY; }
    float firstGlyphOverflow() const { ASSERT(m_accountForGlyphBounds); return m_firstGlyphOverflow; }
    float lastGlyphOverflow() const { ASSERT(m_accountForGlyphBounds); return m_lastGlyphOverflow; }

    const TextRun& run() const { return m_run; }
    float runWidthSoFar() const { return m_runWidthSoFar; }

#if ENABLE(SVG_FONTS)
    String lastGlyphName() const { return m_lastGlyphName; }
    void setLastGlyphName(const String& name) { m_lastGlyphName = name; }
    Vector<SVGGlyph::ArabicForm>& arabicForms() { return m_arabicForms; }
#endif

    static bool supportsTypesettingFeatures(const FontCascade& font)
    {
#if PLATFORM(COCOA)
        return !(font.typesettingFeatures() & ~(Kerning | Ligatures));
#else
        return !font.typesettingFeatures();
#endif
    }

    const FontCascade* m_font;

    const TextRun& m_run;

    unsigned m_currentCharacter;
    float m_runWidthSoFar;
    float m_expansion;
    float m_expansionPerOpportunity;
    bool m_isAfterExpansion;
    float m_finalRoundingWidth;

#if ENABLE(SVG_FONTS)
    String m_lastGlyphName;
    Vector<SVGGlyph::ArabicForm> m_arabicForms;
#endif

private:
    GlyphData glyphDataForCharacter(UChar32, bool mirror, int currentCharacter, unsigned& advanceLength, String& normalizedSpacesStringCache);
    template <typename TextIterator>
    inline unsigned advanceInternal(TextIterator&, GlyphBuffer*);

    enum class TransformsType { None, Forced, NotForced };
    TransformsType shouldApplyFontTransforms(const GlyphBuffer*, int lastGlyphCount, UChar32 previousCharacter) const;
    float applyFontTransforms(GlyphBuffer*, bool ltr, int& lastGlyphCount, const Font*, TypesettingFeatures, UChar32 previousCharacter, bool force, CharactersTreatedAsSpace&);

    TypesettingFeatures m_typesettingFeatures;
    HashSet<const Font*>* m_fallbackFonts;
    bool m_accountForGlyphBounds;
    float m_maxGlyphBoundingBoxY;
    float m_minGlyphBoundingBoxY;
    float m_firstGlyphOverflow;
    float m_lastGlyphOverflow;
    bool m_forTextEmphasis;
};

}

#endif