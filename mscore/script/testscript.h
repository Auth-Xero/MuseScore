//=============================================================================
//  MuseScore
//  Music Composition & Notation
//
//  Copyright (C) 2018 Werner Schweer
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License version 2
//  as published by the Free Software Foundation and appearing in
//  the file LICENCE.GPL
//=============================================================================

#ifndef __TESTSCRIPT_H__
#define __TESTSCRIPT_H__

#include "scriptentry.h"

namespace Ms {

class ScriptContext;

//---------------------------------------------------------
//   TestScriptEntry
//---------------------------------------------------------

class TestScriptEntry : public ScriptEntry {
   protected:
      static QString entryTemplate(const char* testType) { return ScriptEntry::entryTemplate(SCRIPT_TEST).arg(testType) + " %1"; }
   public:
      static constexpr const char* TEST_SCORE = "score";
      static constexpr const char* TEST_IMAGE = "image";

      static std::unique_ptr<ScriptEntry> deserialize(const QStringList& tokens);
      };

//---------------------------------------------------------
//   ScoreTestScriptEntry
//---------------------------------------------------------

class ScoreTestScriptEntry : public TestScriptEntry {
      QString _refPath;
   public:
      ScoreTestScriptEntry(QString refPath) : _refPath(refPath) {}
      bool execute(ScriptContext& ctx) const override;
      QString serialize() const override { return entryTemplate(TEST_SCORE).arg(_refPath); }
      static std::unique_ptr<ScriptEntry> fromContext(const ScriptContext& ctx, QString fileName = QString());
      };

//---------------------------------------------------------
//   ImageTestScriptEntry
//---------------------------------------------------------

class ImageTestScriptEntry : public TestScriptEntry {
      QString _refPath;
      int _pageIndex = 0;

      // parameters for ScoreView::getRectImage()
      static constexpr int dpi = 130;
      static constexpr bool transparent = true;
      static constexpr bool printMode = false;

   public:
      ImageTestScriptEntry(QString refPath, int pageIndex) : _refPath(refPath), _pageIndex(pageIndex) {}
      bool execute(ScriptContext& ctx) const override;
      QString serialize() const override { return entryTemplate(TEST_IMAGE).arg(QString("page %1 %2").arg(_pageIndex).arg(_refPath)); }
      static std::unique_ptr<ScriptEntry> deserialize(const QStringList& tokens);
      static std::unique_ptr<ScriptEntry> fromContext(const ScriptContext& ctx, QString fileName = QString());
      };

}     // namespace Ms
#endif
