#ifndef WORK1_H
#define WORK1_H

#include <QString>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QTextStream>
#include <QHash>

struct Work1Params{
public:
    QString inFile;
    QString outFile;
    bool isBackup;
};

struct Literal{
    enum Type {normal, raw, interpolated};

    QString fileName;
    int lineNumber;
    int index=-1;
    int indexEnd=-1;
    int length;
    QString value;
    QString wordCode;
    QString pref;
    QString postf;    
    bool relevant;
    QStringList embeddings;
    Type type;
    bool concatenate = false;

    bool isValid(){return !value.isEmpty();}
};

struct Exclusion{
    QString fileName;
    int index;
    int length;
    QString value;
};

class Work1
{
public:
    Work1();
    static int doWork();
    static Work1Params params;
private:

    static const QString SLN;
    static const QString CSPROJ1;
    static const QString CSPROJ2;
    static const QString CSPROJ3;
    //static const QString STRLIT;
    //static const QString COMMENT;
    static const QString COMORSTR;
    static const QString CLASSSTR;
    static const QString FILTER1;
    static const QString FILTER2;
    static const QString FILTER3;
    static const QString FILTER4;
    static const QString FILTER5;
    static const QString FILTER6;
    static const QString FILTER7;
    static const QString FILTER8;
    static const QString FILTER9;
    static const QString ENUM;
    static const QString ANNOTATION;
    static const QString DEBUGS;
    static const QString IACS1;
    static const QString IACS2;
    static const QString DISP1;
    static const QString DISP2;
    static const QString SQLSTR;
    static const QString DEBUGW;
    static const QString SQLPARAM;
    static const QString FIELDNAME;
    static const QString REGION;
    static const QString DPROP;
    static const QString RCOM;
    static const QString LSER;
    static QStringList getFileList(QString in, const QString regex);
    static QStringList getFileList(QStringList in, const QString regex);
    static QString append9(const QString &a, const QString &b);
    static QString getParentDirName(const QString &file);
    static QStringList appendString(const QStringList &in, const QString &ezt);
    static void copyDir(QStringList source, QStringList dest);
    static QString getProjFolderName(QString proja);
    //static void killComments(QStringList files);
    static void filterStrings(QList<Literal> *list, const QString regexp);
    static QList<Exclusion> getSpecial(QStringList source, const QString regexp);
    static QList<Exclusion> getSpecial2(QStringList source, QString filename, const QString regexp);
    static void assertLiterals(QList<Literal> *list, QList<Exclusion> set, const QString regexp);
    static void assertLiterals2(QList<Literal> *list, QList<Exclusion> set);

    enum AssertMode{ ByValue, ByPrefix, ByPostfix, ByLetter, BySize, ByLetterCount };
    static void assertLiterals3(QList<Literal> *list, QStringList exc, AssertMode mode);
    static QStringList removeFiles(QStringList list, QStringList exc);

    static int generateCsv(const QList<Literal>&, const QString& filename);
    static int WcodesToFile(const QList<Literal> &list, const QString &filename,  const QStringList& langCodes);

    static void addWCodes(QList<Literal> *list);

    static void LiteralsToFile(const QList<Literal>& literals, const QString& fileName);
    static QList<Literal> getLiterals2(const QStringList& inFileNames);
    static QList<Literal> getLiterals2(const QString& inFileName);
    static QList<Literal> getLiterals3(const QString& txt);
    static Literal getLiteral2(const QString& txt, int ix);

    static Literal getNormalString(const QString& txt, int ix);
    static Literal getRawString(const QString& txt, int ix);
    static Literal getInterpolatedString(const QString& txt, int ix);

    static QString getPrefix(const QString& txt, const Literal& l, int plen);
    static QString getPostfix(const QString& txt, const Literal& l, int plen);
    static QList<Literal> GetWcodes(const QList<Literal> &literals);
};


#endif // WORK1_H
