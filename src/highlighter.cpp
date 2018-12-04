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
    rule.pattern = QRegExp("\\bQ[A-Za-z]+\\b");
    rule.format = classFormat;
    highlightingRules.append(rule);
//! [2]


    multiLineCommentFormat.setForeground(Qt::red);
//! [3]

    //! [4]
        quotationFormat.setForeground(Qt::darkGreen);
        rule.pattern = QRegExp("\".*\"");
    //    rule.pattern = QRegExp("\\(.*\\)");
        rule.format = quotationFormat;
        highlightingRules.append(rule);
    //! [4]


//! [5]
//    functionFormat.setFontItalic(true);
    functionFormat.setForeground(Qt::darkMagenta);
    rule.pattern = QRegExp("\\b[A-Za-z0-9_]+(?=\\()");
    rule.format = functionFormat;
    highlightingRules.append(rule);
//! [5]


    foreach (const QString &pattern, preambleKeywordPatterns) {
        rule.pattern = QRegExp(pattern,Qt::CaseInsensitive);
        rule.format = preambleKeywordFormat;
        highlightingRules.append(rule);
    }


    foreach (const QString &pattern, keywordPatterns) {
        rule.pattern = QRegExp(pattern,Qt::CaseInsensitive);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }
//! [1]


    //! [3]
        singleLineCommentFormat.setForeground(Qt::lightGray);
        singleLineCommentFormat.setFontItalic(true);
    //    rule.pattern = QRegExp("//[^\n]*");
        rule.pattern = QRegExp("\\{[^\n]*");
        rule.format = singleLineCommentFormat;
        highlightingRules.append(rule);

        rule.pattern = QRegExp("//[^\n]*");
        highlightingRules.append(rule);


//! [6]
    commentStartExpression = QRegExp("/\\*");
    commentEndExpression = QRegExp("\\*/");
}
//! [6]

//! [7]
void Highlighter::highlightBlock(const QString &text)
{
    foreach (const HighlightingRule &rule, highlightingRules) {
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(text);
        while (index >= 0) {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = expression.indexIn(text, index + length);
        }
    }
//! [7] //! [8]
    setCurrentBlockState(0);
//! [8]

//! [9]
    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = commentStartExpression.indexIn(text);

//! [9] //! [10]
    while (startIndex >= 0) {
//! [10] //! [11]
        int endIndex = commentEndExpression.indexIn(text, startIndex);
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
    }
}
//! [11]

