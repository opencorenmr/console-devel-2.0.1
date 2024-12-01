#include <QtGui>

#include "highlighter.h"

//! [0]
Highlighter::Highlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    preambleKeywordFormat.setForeground(Qt::darkRed);

    keywordFormat.setForeground(Qt::darkBlue);
    keywordFormat.setFontWeight(QFont::Bold);

    QStringList preambleKeywordPatterns;
    preambleKeywordPatterns << "\\bclk\\b" << "\\bfunction\\b" << "\\btransform\\b"
                            << "\\buses\\b" << "\\bimport\\b" << "\\bamp\\b"
                            << "\\bphase\\b" << "\\bfreq\\b" << "\\bint\\b"
                            << "\\bphaselist\\b"
                            << "\\btime\\b" << "\\bacqphase\\b" << "\\baux\\b" << "\\bconst\\b"
                            << "\\bal\\b" << "\\bdw\\b" << "\\bna\\b"<< "\\bnd\\b"<< "\\bpd\\b";

    QStringList keywordPatterns;

    keywordPatterns
                    << "\\bstart\\b"
                    << "\\binit\\b" << "\\bpulse\\b" << "\\brelax\\b"
                    << "\\bdelay\\b" << "\\bloop\\b" << "\\bendlp\\b"
                    << "\\bsync\\b" << "\\basync\\b" << "\\bcontinue\\b" << "\\bstandby\\b";

/*    keywordPatterns << "\\bchar\\b" << "\\bclass\\b" << "\\bconst\\b"
                    << "\\bdouble\\b" << "\\benum\\b" << "\\bexplicit\\b"
                    << "\\bfriend\\b" << "\\binline\\b" << "\\bint\\b"
                    << "\\blong\\b" << "\\bnamespace\\b" << "\\boperator\\b"
                    << "\\bprivate\\b" << "\\bprotected\\b" << "\\bpublic\\b"
                    << "\\bshort\\b" << "\\bsignals\\b" << "\\bsigned\\b"
                    << "\\bslots\\b" << "\\bstatic\\b" << "\\bstruct\\b"
                    << "\\btemplate\\b" << "\\btypedef\\b" << "\\btypename\\b"
                    << "\\bunion\\b" << "\\bunsigned\\b" << "\\bvirtual\\b"
                    << "\\bvoid\\b" << "\\bvolatile\\b";
*/


//! [2]
    classFormat.setFontWeight(QFont::Bold);
    classFormat.setForeground(Qt::darkMagenta);
    rule.pattern = QRegularExpression("\\bQ[A-Za-z]+\\b");
    rule.format = classFormat;
    highlightingRules.append(rule);
//! [2]


    multiLineCommentFormat.setForeground(Qt::red);
//! [3]

    //! [4]
        quotationFormat.setForeground(Qt::darkGreen);
        rule.pattern = QRegularExpression("\".*\"");
    //    rule.pattern = QRegularExpression("\\(.*\\)");
        rule.format = quotationFormat;
        highlightingRules.append(rule);
    //! [4]


//! [5]
//    functionFormat.setFontItalic(true);
    functionFormat.setForeground(Qt::darkMagenta);
    rule.pattern = QRegularExpression("\\b[A-Za-z0-9_]+(?=\\()");
    rule.format = functionFormat;
    highlightingRules.append(rule);
//! [5]


    foreach (const QString &pattern, preambleKeywordPatterns) {
        rule.pattern = QRegularExpression(pattern,QRegularExpression::CaseInsensitiveOption);
        rule.format = preambleKeywordFormat;
        highlightingRules.append(rule);
    }


    foreach (const QString &pattern, keywordPatterns) {
        rule.pattern = QRegularExpression(pattern,QRegularExpression::CaseInsensitiveOption);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }
//! [1]


    //! [3]
        singleLineCommentFormat.setForeground(Qt::lightGray);
        singleLineCommentFormat.setFontItalic(true);
    //    rule.pattern = QRegularExpression("//[^\n]*");
        rule.pattern = QRegularExpression("\\{[^\n]*");
        rule.format = singleLineCommentFormat;
        highlightingRules.append(rule);

        rule.pattern = QRegularExpression("//[^\n]*");
        highlightingRules.append(rule);


//! [6]
    commentStartExpression = QRegularExpression("/\\*");
    commentEndExpression = QRegularExpression("\\*/");
}
//! [6]

//! [7]
void Highlighter::highlightBlock(const QString &text)
{
    /*
      foreach (const HighlightingRule &rule, highlightingRules) {
        QRegularExpression expression(rule.pattern);
        int index = expression.indexIn(text);
        while (index >= 0) {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = expression.indexIn(text, index + length);
        }
    }
    */
    for (const HighlightingRule &rule : std::as_const(highlightingRules)) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
//! [7] //! [8]
    setCurrentBlockState(0);
//! [8]

//! [9]
    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = text.indexOf(commentStartExpression);
//        startIndex = commentStartExpression.indexIn(text);

//! [9] //! [10]
    while (startIndex >= 0) {
//! [10] //! [11]
/*        int endIndex = commentEndExpression.indexIn(text, startIndex);
        int commentLength;
        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex
                            + commentEndExpression.matchedLength();
        }
        setFormat(startIndex, commentLength, multiLineCommentFormat);
        startIndex = commentStartExpression.indexIn(text, startIndex + commentLength);
*/
        QRegularExpressionMatch match = commentEndExpression.match(text, startIndex);
        int endIndex = match.capturedStart();
        int commentLength = 0;
        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex
                            + match.capturedLength();
        }
        setFormat(startIndex, commentLength, multiLineCommentFormat);
        startIndex = text.indexOf(commentStartExpression, startIndex + commentLength);

    }
}
//! [11]

