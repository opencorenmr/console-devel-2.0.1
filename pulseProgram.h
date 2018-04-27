#ifndef PULSEPROGRAM_H
#define PULSEPROGRAM_H

#include <QtWidgets>
#include <QObject>
#include <QSet>
#include <QList>
#include <QString>
#include <QStringList>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QDebug>

#include "gates.h"
#include "variables.h"
#include "intToHex.h"
#include "rcvrcom.h"
#include "digitalfilter.h"



#define CURRENT_VERSION 2.0

#define MAX_PHASELIST_ITEMS 15   // phase list per channel
                                 //( 7 for build<=2004 , 15 for build >= 2005)
#define MAX_PHASELIST_LENGTH 128
#define MAX_LOOPS 2

#define CLK_COUNT_AD9858_COMMAND 6



/*
 *
when X"00" => -- END_PPG
when X"10" => -- LOOP0
when X"11" => -- LOOP1
when X"12" => -- LOOP2
when X"18" => -- END_LOOP0
when X"19" => -- END_LOOP1
when X"1A" => -- END_LOOP2
when X"B0" => -- JUMP
when X"B1" => -- JUMP_IF
when X"A1" => --OUT
when X"BA" => -- BURST
when X"A2" => -- WAIT_HIGH
when X"A3" => -- WAIT_LOW
when X"AA" => -- ASYNC
when X"FF" => -- ALL_SYNC
 *
 */

class TXAxisOptionWidget;

enum TbinCommand {com_END_PPG,com_OUT,com_OUT_PD,com_WAIT_HIGH,com_WAIT_LOW,com_LOOP0,com_LOOP1,
                 com_LOOP2,com_END_LOOP0,com_END_LOOP1,com_END_LOOP2,com_BURST,com_ASYNC,
                  com_JUMP,com_JUMP_IF,com_ALL_SYNC};
//enum Tfunction {
//  fSin, fCos, fTan, fSinc, fExp
//
//};

class TcompiledPPG{
public:
    TcompiledPPG(TbinCommand bc) {binCommand=bc; argument="0000000000"; lineSet.clear();}
    TcompiledPPG(TbinCommand bc, QString arg) {binCommand=bc; argument=arg; lineSet.clear();}
    TcompiledPPG(TbinCommand bc, QString arg, QSet<int> s) {binCommand=bc; argument=arg; lineSet=s;}

    virtual ~TcompiledPPG(){;}
    TbinCommand binCommand;

    QString argument;
    QSet<int> lineSet;
    void setLineSet(QSet<int> qs){lineSet.clear(); lineSet=qs;}

    static QString lineWord(TcompiledPPG *tc);
    static QString command(TcompiledPPG *tc);
    static QString arg(TcompiledPPG *tc);
    static QString comArgLineWord(TcompiledPPG *tc);

protected:

};


class TppgLines{
  public:
    TppgLines()
    {
       source.clear(); fileIndex.clear(); lineIndex.clear();
       // qDebug()<<"constructor of TppgLines";
    }
    virtual ~TppgLines()
    {
        // qDebug()<<"destructor of TppgLines";
    }
    QStringList source;
//    QStringList sourceFileNames;
    QList<int> fileIndex;
    QList<int> lineIndex;

    int currentPosition;
    QString currentLine;
    bool getStr();
    bool getStr(QChar qc);
    bool getStr(QChar qc, int*p);
    bool getStr(QChar qc1, QChar qc2);
    bool getStr(QChar qc1, QChar qc2, int *p1, int *p2);
    bool getStr(QChar qc1, QChar qc2, int *p);
    bool getStr(QString qs);
    bool getStr(QString qs1, QString qs2);

  protected:

};

enum TasyncMode {asyncContinue,asyncStandby};


class TasyncPPG: public TppgLines {
  public:
    TasyncPPG()
    { source.clear(); fileIndex.clear(); lineIndex.clear(); called=false; whereIs.clear();
                 // qDebug()<<"constructor of TasyncPPG";
    }
    virtual ~TasyncPPG() {
        // qDebug()<<"destructor of TasyncPPG";
    }
    int channel() {return Fchannel;}
    void setChannel(int ch) {Fchannel=ch;}
    QString name() {return Fname;}
    void setName(QString s) {Fname=s;}
    bool called;
    TasyncMode asyncMode;
    QList<int> whereIs;
    int startAddress;

    TppgLines toPPGLines()
    {
        TppgLines pl;
        pl.source=source;
        pl.fileIndex=fileIndex;
        pl.lineIndex=lineIndex;
        return pl;
    }

  protected:
    int Fchannel;
    QString Fname;

};

class TpulseProgram
{
  public:
    TpulseProgram(int channels);   // constructor
    ~TpulseProgram();  // destructor

    TpulseProgram& operator=(const TpulseProgram&); // copy

    bool compiled;  // !!!
    bool error; // !!!
    QString errorMessage; // !!!

    bool warning;
    QString warningMessage;

    double version() {return FVersion;}
    void setVersion(double v) {FVersion=v;}

//    TrcvrCom rcvrCom;
    TdigitalFilter digitalFilter; // !!!

    int channels() {return FNOfChannels;}
    int origNA() {return ForigNA;}
    void setOrigNA(int n) {ForigNA=n;}


    QString sourceFileName() {return FppgFileName;}
    void setppgFileName(QString fn)
    { //qDebug() << "ppg file name " + fn + " has been set.";
       FppgFileName=fn;
       FppgFilePath=QFileInfo(fn).path();
       gateFilePaths.clear();
       gateFilePaths.append(QDir::homePath()+"/.opencorenmr"+"/gates");
       gateFilePaths.append(FppgFilePath);

      // qDebug()<<gateFilePaths;
    }
    void setppgFilePath(QString fp) {FppgFilePath=fp;}
    QString ppgFilePath() {return FppgFilePath;}  // Path of "this" pulse program

    QStringList loadedFiles; // !!! // files to be read

//    int readPPGFromFile(QString filename);
    bool loadPPG();
   // bool loadPPG(QStringList sl) {sourcePPG=sl;}   // TODO: copy source from TextEdit Widget etc.
    bool savePPG();  //
    // bool savePPG(QString filename);   // TODO: overloaded function for "save as" operation
    bool analyzePPG();
    bool compilePPG();

    QStringList sourcePPG;  // !!!
    void updateSourcePPG();
    TppgLines preamble; // !!!
    TppgLines mainPPG;// !!!

    QList<TasyncPPG*> asyncPPG;
    QList<QStringList *> compiledPPG_str;
    QList<QStringList *> compiledPhaseCycle;
//    QList<QList<TcompiledPPG> *> compiledPPG;
    QList<QList<TcompiledPPG*> > compiledPPG;
//    QList<TcompiledPPG> *insertionCompiledPPG;
    QList<TcompiledPPG*> insertionCompiledPPG;
    QList<QList<int> *> NOfLinesPerCommand;
    QList<TVariable*> variables;

    QStringList userDefinedFunctions;

    double CLK; // !!!
    TGates gates;// !!!

    QStringList gateFilePaths; // !!!// should be set by the mainwindow.cpp
    QString gateFileName() {return FGateFileName;}
    void setGateFileName(QString gfn) {FGateFileName=gfn;}

    bool processPreamble();
    QStringList preambleInfo;// !!!

    QStringList toFPGA;// !!!
    QStringList updatedPPG;// !!!

    int variableIndex(QString s)
    {
        int k=-1;
        for(int i=0; i<variables.size(); i++)
        {
            QString s1=variables.at(i)->name();
            if(0==QString::compare(s,variables.at(i)->name(),Qt::CaseInsensitive)) k=i;
        }

        return k;
    }

    bool errorCheckOnly; // !!!

    bool updateVariable(QString vName, QVariant qv, TVariable::Unit uni);
    bool updateVariable(); // In advance, you need to update the variables that you want to change

    bool processMainPPG();
    bool processAsyncPPG();

    TreceiverInfo receiverInfo; // !!!

    void updateReceiverInfo()
    {
//        receiverInfo.setNA(variables.at(variableIndex("NA"))->value().toInt());
        receiverInfo.setNA(origNA());
        receiverInfo.setND(variables.at(variableIndex("ND"))->value().toInt());
        receiverInfo.setAL(variables.at(variableIndex("AL"))->value().toInt());
        receiverInfo.setDW(variables.at(variableIndex("DW"))->value().toDouble());
       // receiverInfo.setDCOffset(0);
    }

    bool evalArg(QString &str);
    double evalArgExpression(const QString &str, int &pos, bool &ok);
//    double evalArgExpression(QString &str, int &pos, bool &ok);
    double evalArgTerm(const QString &str, int &pos, bool &ok);
    double evalArgFactor(const QString &str, int &pos, bool &ok);
    double evalArgResult;


  private:
 //   bool updateVariable_private(QString vName, QVariant qv, TUnit uni);

    QVariant evalExpression(const QString &str, int &pos) const;
    QVariant evalTerm(const QString &str, int &pos) const;
    QVariant evalFactor(const QString &str, int &pos) const;


    bool evalTime(QString &str);
    double evalTimeExpression(const QString &str, int &pos, bool &ok);
    double evalTimeTerm(const QString &str, int &pos, bool &ok);
    double evalTimeFactor(const QString &str, int &pos, bool &ok);

    bool isShapedPulseUsed; // set by m_pulse and read by processGate
    int sharpK;  // # -> int
    bool isShapedPulseActive;  // initialized by m_pulse, modified by processGate, and read by m_pulse

    bool isInsertActive;
    int ForigNA;

protected:

    double FVersion;
    QString FppgFilePath;
    int FNOfChannels;
    QString FppgFileName;
      // The first item (FppgFileNames[0]) must specify the main source file of the pulse program
      // The names of the imported files will be added to the following items.
    QStringList reservedWords;


    QStringList preambleCommands;
    QStringList ppgCommands;
    QStringList asyncPPGCommands;
    QStringList specialFunctionNames;
    QString varName;
    QString argument;
    QString caption;

    bool updateACQTIME();

    bool checkVariableNameExpression(QString vn);
    bool checkMultipleDeclaration(QString vn);
    bool checkReservedWordDeclaration(QString vn);

    bool processPreambleCommand();
    bool processPreambleCommand2();

    QSet<int> lineSet;

    bool p_uses();
    bool p_clk();
    bool p_time();
    bool p_amp();
    bool p_phase();
    bool p_freq();
    bool p_int();
    bool p_loop();
    bool p_aux();
    bool p_phaseList();
    bool p_acqPhase();
    bool p_function();
    bool p_version();
    bool p_transform();
    bool p_double();

    bool processPhaseCycle();

    bool m_pulse(TppgLines &ppgLines);
    bool m_pulse(TppgLines &ppgLines, int insersionAddress);
    TppgLines freqSweepPPGLines;
    int freqSweepInsertionPosition;
    bool m_delay(TppgLines &ppgLines);
      bool getTime(QString q);
      double getTimeResult;
    bool m_burst(TppgLines &ppgLines);
    bool m_ready();
    bool m_loop(TppgLines &ppgLines);
    bool m_endlp(TppgLines &ppgLines);
    bool m_freqSweep(TppgLines &ppgLines);
    bool m_endFreqSweep(TppgLines &ppgLines);
      bool m_freqSweepLateAssign(TppgLines &ppgLines);
    bool m_async(TppgLines &ppgLines);
    bool m_sync(TppgLines &ppgLines);
    bool m_wait_hl(TppgLines &ppgLines, int hl); // wait_h -> hl=1, wait_l -> hl=0
    bool m_exttrig();
    bool m_import(TppgLines &ppgLines);
    bool m_init();
    bool m_relax();

    bool analyzeGate(TppgLines &ppgLines);
    bool processGate(TppgLines &ppgLines);  // analyzeGate -> eventually change over to "processGate"

    QString FGateFileName;
    QSet<int> usedGateIndex;

    int currentCH;
    int currentAsyncPPGIndex;

    QList<int> acqPhaseList;
    bool genAcqPhaseCycle(QStringList *sl);

    bool constVar;
    QList<int> loopManager; QList<int> loopCounts;
    QSet<int> asyncManager;
    QSet<int> sweepManager;
    QList<double> elapsedTime;



    bool containsAD9858;
    QList<QSet<int> > AD9858Buffer;

    bool NADefined;
    bool NDDefined;
    bool DWDefined;
    bool PDDefined;
    bool ALDefined;
    bool acqPhaseDefined;

    bool loopZero;
    int zeroLoopCounter;
    int zeroLoopAddress;
};

#endif // PULSEPROGRAM_H
