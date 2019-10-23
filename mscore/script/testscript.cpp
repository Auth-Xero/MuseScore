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

#include "testscript.h"

#include "musescore.h"
#include "scoreview.h"
#include "script.h"

#include "libmscore/page.h"
#include "libmscore/scorediff.h"

namespace Ms {

//---------------------------------------------------------
//   TestScriptEntry::deserialize
//---------------------------------------------------------

std::unique_ptr<ScriptEntry> TestScriptEntry::deserialize(const QStringList& tokens)
      {
      // assume that 0th token is just a "test" statement
      if (tokens.size() < 2) {
            qDebug("test: unexpected tokens size: %d", tokens.size());
            return nullptr;
            }

      const QString& type = tokens[1];
      if (type == TEST_SCORE) {
            if (tokens.size() != 3) {
                  qDebug("test: unexpected tokens size: %d", tokens.size());
                  return nullptr;
                  }
            return std::unique_ptr<ScriptEntry>(new ScoreTestScriptEntry(tokens[2]));
            }
      else if (type == TEST_IMAGE)
            return ImageTestScriptEntry::deserialize(tokens);

      qDebug() << "test: unsupported type:" << tokens[1];
      return nullptr;
      }

//---------------------------------------------------------
//   ScoreTestScriptEntry::fromContext
//---------------------------------------------------------

std::unique_ptr<ScriptEntry> ScoreTestScriptEntry::fromContext(const ScriptContext& ctx, QString fileName)
      {
      MasterScore* score = ctx.mscore()->currentScore()->masterScore();
      // TODO: handle excerpts

      if (fileName.isEmpty()) {
            int scoreNum = 1;
            const QString templ("%1.mscx");
            fileName = templ.arg(QString::number(scoreNum));
            while (QFileInfo(fileName).exists())
                  fileName = templ.arg(QString::number(++scoreNum));
            }

      QString filePath = ctx.absoluteFilePath(fileName);
      QFileInfo fi(filePath);
      score->Score::saveFile(fi);

      if (ctx.relativePaths())
            filePath = fileName;

      return std::unique_ptr<ScriptEntry>(new ScoreTestScriptEntry(filePath));
      }

//---------------------------------------------------------
//   ScoreTestScriptEntry::execute
//---------------------------------------------------------

bool ScoreTestScriptEntry::execute(ScriptContext& ctx) const
      {
      MasterScore* curScore = ctx.mscore()->currentScore()->masterScore();
      if (!curScore) {
            ctx.execLog() << "ScoreTestScriptEntry: no current score" << endl;
            return false;
            }

      QString refFilePath = ctx.absoluteFilePath(_refPath);
      std::unique_ptr<MasterScore> refScore(ctx.mscore()->readScore(refFilePath));
      if (!refScore) {
            ctx.execLog() << "reference score loaded with errors: " << refFilePath << endl;
            return false;
            }

      ScoreDiff diff(curScore, refScore.get(), /* textDiffOnly */ true);
      if (!diff.equal()) {
            ctx.execLog() << "ScoreTestScriptEntry: fail\n" << diff.rawDiff() << endl;
            return false;
            }
      return true;
      }

//---------------------------------------------------------
//   ImageTestScriptEntry::deserialize
//---------------------------------------------------------

std::unique_ptr<ScriptEntry> ImageTestScriptEntry::deserialize(const QStringList& tokens)
      {
      if (tokens.size() < 5) {
            qWarning("test image: unexpected number of tokens: %d", tokens.size());
            return nullptr;
            }

      if (tokens[2] == "page") {
            bool ok;
            const int pageIndex = tokens[3].toInt(&ok);
            const QString& refPath = tokens[4];

            if (!ok)
                  return nullptr;
            return std::unique_ptr<ScriptEntry>(new ImageTestScriptEntry(refPath, pageIndex));
            }
      else
            qWarning("unknown image test type: %s", qPrintable(tokens[2]));

      return nullptr;
      }

//---------------------------------------------------------
//   ImageTestScriptEntry::fromContext
//---------------------------------------------------------

std::unique_ptr<ScriptEntry> ImageTestScriptEntry::fromContext(const ScriptContext& ctx, QString fileName)
      {
      if (fileName.isEmpty())
            return nullptr;

      ScoreView* v = ctx.mscore()->currentScoreView();
      if (!v)
            return nullptr;

      const QRectF r = v->canvasViewport();
      const QPointF p((r.left() + r.right()) / 2, (r.top() + r.bottom()) / 2);

      int pageIdx = v->score()->pageIdx(v->point2page(p));
      if (pageIdx < 0)
            pageIdx = 0;

      const QImage img = v->getRectImage(v->score()->pages()[pageIdx]->canvasBoundingRect(), dpi, transparent, printMode);

      QString filePath = ctx.absoluteFilePath(fileName);
      img.save(filePath);

      if (ctx.relativePaths())
            filePath = fileName;

      return std::unique_ptr<ScriptEntry>(new ImageTestScriptEntry(filePath, pageIdx));
      }

//---------------------------------------------------------
//   ImageTestScriptEntry::execute
//---------------------------------------------------------

bool ImageTestScriptEntry::execute(ScriptContext& ctx) const
      {
      ScoreView* v = ctx.mscore()->currentScoreView();
      if (!v) {
            ctx.execLog() << "ImageTestScriptEntry: no score view" << endl;
            return false;
            }

      QImage refImg;
      const QString refImgPath = ctx.absoluteFilePath(_refPath);
      if (!refImg.load(refImgPath)) {
            ctx.execLog() << "ImageTestScriptEntry: couldn't load reference image: " << refImgPath << endl;
            return false;
            }

      QImage img = v->getRectImage(v->score()->pages()[_pageIndex]->canvasBoundingRect(), dpi, transparent, printMode);
      img = img.convertToFormat(refImg.format());

      return img == refImg;
      // TODO: save "wrong" image in case of failure?
      }
}
