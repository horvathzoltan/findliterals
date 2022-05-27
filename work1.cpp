#include "work1.h"
#include "common/logger/log.h"
#include "common/helper/file/filenamehelper.h"
#include "common/helper/file/filehelper.h"
#include "common/helper/textfilehelper/textfilehelper.h"

#include <QSet>
#include <QString>

#define zforeach(var, container) for(auto (var) = (container).begin(); (var) != (container).end(); ++(var))

Work1Params Work1::params;

Work1::Work1()
{

}

QTextStream& qStdOut()
{
    static QTextStream ts( stdout );
    return ts;
}

const QString Work1::SLN = R"(Project.*\"(.*.csproj)\")";
const QString Work1::CSPROJ = R"(<Compile\s+Include\s*=\s*\"([^.]*\.cs)\")";
const QString Work1::COMORSTR = R"((\"([^"\\]*(?:\\.[^"\\]*)*)\"|\@\"((?:[^\"]|(?:\"\"))*)\")|([\s]*(?:\/\*([\s\S]*?)\*\/|\/\/(.*))))";
const QString Work1::FILTER1 = R"(^[^\pL]+$)"; //a betűt nem tartalmazóak
const QString Work1::FILTER2 = R"(^\s*select\s+\*)"; //a select * -gal kezdődőek
const QString Work1::FILTER3 = R"(^[^\"\|\?\*\<\>\:\n]*\.(?:[a-z]{3,5})$)"; //fájlnevek
const QString Work1::FILTER4 = R"(^.*[*|]\.(?:[a-z]{3,5})$)"; //fájlszűrők
const QString Work1::FILTER5 = R"(^[0-9a-fA-F]{3,}-)"; //guid-ok
const QString Work1::FILTER6 = R"(^dbo\..*$)";
const QString Work1::FILTER7 = R"([yY]{2,4})"; //dátumok
const QString Work1::FILTER8 = R"(^(?:[\p{L}_$][\p{L}\p{N}_$]*\.)*[\p{L}_$][\p{L}\p{N}_$\[\]]*$)"; //osztálynevek
const QString Work1::FILTER9 = R"(^http:\/\/.*)";
const QString Work1::CLASSSTR = R"(\b\s*(?:public|private|protected|internal|protected\s+internal|private\s+protected)\s+class\s+(\pL*)\b)";
const QString Work1::ENUM = R"(\benum(?:[^"])*?\{\s*([\s\S]*?)\s*\})";

const QString Work1::ANNOTATION = R"(\[\pL+\(\"([\pL\s.!:,;0-9\n-]+)\"\)\])";
const QString Work1::DEBUGS = R"(^-{2,}.*$)";
const QString Work1::IACS1 = R"(\.(?:Include|Add|ConnectionStrings|Browse)(?:[\(\[]\s*\")([^\"]+)\"[^\"]*[\)\]])";
const QString Work1::IACS2 = R"(\.(?:Include|Add|ConnectionStrings|Browse)(?:[\(\[]\s*\")(.*)\"\s*\,\s*\"(.*)\"\s*[\]\)])";
const QString Work1::DISP1 = R"(\[Display\s*\(\s*Name\s*=\s*\"([^\"]*)\"\)\])";
const QString Work1::DISP2 = R"(\[Display\s*\(\s*Name\s*=\s*\"(.*)\"\s*,\s*Description\s*=\s*\"(.*)\"\s*\)\])";
const QString Work1::SQLSTR = R"(\"(\s*Delete\s*From.*)\")";
const QString Work1::DEBUGW = R"(Debug\.(?:WriteLine|Write)\s*\(\s*\"(.*)\")";
const QString Work1::SQLPARAM = R"(SqlParameter\s*\(\s*\"(.*)\"\s*)";
const QString Work1::FIELDNAME = R"(field_name\s*=\s*\"(.*)\")";
const QString Work1::REGION = R"(\#\s*region\s*\"(.*)\")";
const QString Work1::DPROP = R"(DependencyProperty\.(?:Register|RegisterAttached)\s*\(\s*\"(\pL*)\")";
const QString Work1::RCOM = R"((?:RaisePropertyChanged|RoutedCommand)\s*\(\s*\"(\pL*)\")";
const QString Work1::LSER = R"(LogService\.(?:Trace|Info|Message)\s*\(\$*\s*\"(.*)\")";

int Work1::doWork()
{
    zInfo(QStringLiteral("params: %1").arg(params.inFile))

    QStringList projFileNames;
    if(params.inFile.endsWith(".sln"))
    {
        projFileNames = getFileList(params.inFile, SLN);
    }
    else if(params.inFile.endsWith(".csproj"))
    {
        QFileInfo fi(params.inFile);
        auto fn = fi.fileName();
        projFileNames.append(fn);
    }

    auto slnparent = getParentDirName(params.inFile);
    auto projla = appendString(projFileNames, slnparent);
    auto destpathname = slnparent+"_ford";

    QStringList sourceFinal;
    QStringList destFinal;

    QStringList exc;/*{
        "InsoleNames.cs",
        "InsoleWCodes.cs",
        "DrawerService.cs",
        "FriendlyColors2.cs",
        "LogService.cs",
        "CSVImportHelper.cs",
        "HelpService.cs",
        "TranslationService.cs",
        "HeatColorizer.cs",
        "RegistryManager.cs",
        "RPIZeroConnectionConfiguration.cs",
        "LiteralFinder.cs",
        "SQLHelper.cs",
        "ChangeFromIntToDecimalInBodyPointDetectionProperties.cs",
        "MeasurementProjectInsoleMeasurementNeededPropertyAdded.cs",
        "InsoleIndicatorValuesChanged.cs",
        "MotherNameGoesToCommentInPatientModel.cs",
        "ConcreteUsedMeasurableBodyPointModelGetPropertiesFromMeasurableBodyPointModel.cs",
        "MeasurableBodyPointsObjectDetectionAreaPropertiesAdded.cs",
        "MeasurementProjectsReportDateTimeNullable.cs",
        "MeasurementProjectsReportAdded.cs",
        "MeasurementResultBodyAngleVectorsCalculatedAngleNullable.cs",
        "MeasurementResultFrameFKAdded.cs",
        "MeasurementResultStreamBytesRemoved.cs",
        "MeasurementResultStreamBytesAdded.cs",
        "MeasurementResultBodyAngleVectorshaveTwoAdditionalPropertyForTimeCode.cs",
        "InitialMigration.cs",
        "PreferredFpsPropertyAddedToMeasurementProjectModel.cs",
        "footprint.cs",
        "Configuration.cs",
        "_TEST",
        "AssemblyInfo.cs",
        "CamelCaseString.cs",
        "ExceptionExtensions.cs",
        "SystemSpecHelper.cs",
        "BodyPointGridItem.cs", "AppConfigurationService.cs", "AssemblyInfoHelper.cs","Migrations",
        "MeasurementWizardForm", "Entities", "EntityFrameworkDataReader.cs", "SensorValueListHelper.cs",
        "AnchorHelper.cs", "Benchmark"
    };*/

    QStringList exc2;/*{
            "System",
            "MasterMoveAnalyse",
            "oIq97mc_mEi68wzXxDxFLg",
            "q4kYxXN2iUSSEjGDWNxNLQ",
            "Arial",
            "Tahoma",
            "ISO-8859-2",
            "en-US",
            "Admin",
            "System.String[]",
            "System.UInt16[]",
            " (*.csv)|*.csv|",
            "{wdata.Patient.MedicalNumber}_{wdata.Footer.Timestamp.ToString(",
            "{m.Name} ({m.UserName})", "RIGHT_", "\\\\Report", "Win32_ComputerSystemProduct",
            "UUID", "UNEXPECTED", "eu-ES", "045e_0810", "046d_086b", "{0} {1}\\n{1}", "MasterMoveAnalyseTestDBConnectionString",
            "{body_point_.ObjectDetectionSize_MinWidth} px - {body_point_.ObjectDetectionSize_MaxWidth} px",
            "0 px - 0 px",
            "{CalibratedDetectionSize_MinWidth} px - {CalibratedDetectionSize_MaxWidth} px",
            "image/jpeg",
            "- Frame added to Queue {0}. FinalFramesForVideo count: {1} -", "//{displayDescription}", "{displayName}:{prop.GetValue(this, null).ToString()}",
            "{opname}:{opid}",
            "ThreadLogger.cs"

        };*/

    QDir slnParentDir(slnparent);

    auto excludefilepath = slnParentDir.filePath("exclude_file.csv"); // filenév szintű kizárás

    auto a  = com::helper::TextFileHelper::loadLines(excludefilepath);
    for(auto&i:a){
        if(i.isEmpty()) continue;
        exc.append(i);
    }

    zInfo("excludefilepath_files: "+excludefilepath + " ("+QString::number(exc.count())+")");


    auto excludefilepath2 = slnParentDir.filePath("exclude_txt.csv"); // szöveg szintű kizárás

    a = com::helper::TextFileHelper::loadLines(excludefilepath2);

    for(auto&i:a){
        if(i.isEmpty()) continue;
        exc2.append(i);
    }

    zInfo("excludefilepath_txt: "+excludefilepath2+ " ("+QString::number(exc2.count())+")");


    foreach(auto proj, projla){
        auto cslr = getFileList(proj, CSPROJ);

        auto cslr2 = removeFiles(cslr, exc);

        auto csprojparent = getParentDirName(proj);

        auto pfn = getProjFolderName(proj);

        auto source = appendString(cslr2, csprojparent);
        auto dest_ = appendString(cslr2, pfn);
        auto dest = appendString(dest_, destpathname);

        sourceFinal<<source;
        destFinal<<dest;
    }

    copyDir(sourceFinal, destFinal);

    QList<Exclusion> classes = getSpecial(destFinal, CLASSSTR);
    QList<Exclusion> annotations = getSpecial(destFinal, ANNOTATION);
    QList<Exclusion> iacs1 = getSpecial(destFinal, IACS2);
    QList<Exclusion> iacs2 = getSpecial(destFinal, IACS1);
    QList<Exclusion> disp1 = getSpecial(destFinal, DISP1);
    QList<Exclusion> disp2 = getSpecial(destFinal, DISP2);
    QList<Exclusion> debugws = getSpecial(destFinal, DEBUGW);
    QList<Exclusion> sqlps = getSpecial(destFinal, SQLPARAM);
    QList<Exclusion> fieldnames = getSpecial(destFinal, FIELDNAME);
    QList<Exclusion> regions = getSpecial(destFinal, REGION);
    QList<Exclusion> dprop = getSpecial(destFinal, DPROP);
    QList<Exclusion> rcom = getSpecial(destFinal, RCOM);
    QList<Exclusion> lser = getSpecial(destFinal, LSER);


    QList<Exclusion> wcodes = getSpecial2(destFinal, "TranslationService.cs", ENUM);

    QDir destDir(slnparent);//destpathname
    auto data1path = destDir.filePath("literals.csv"); // literalis stringek - összes
    auto data2path = destDir.filePath("literals2.csv"); // szűrt

    zInfo("data1path: "+data1path);
    zInfo("data2path: "+data2path);

    auto literalStrings = getLiterals(destFinal, COMORSTR, data1path);

    filterStrings(&literalStrings, FILTER1);
    filterStrings(&literalStrings, FILTER2);
    filterStrings(&literalStrings, FILTER3);
    filterStrings(&literalStrings, FILTER4);
    filterStrings(&literalStrings, FILTER5);
    filterStrings(&literalStrings, FILTER6);
    filterStrings(&literalStrings, FILTER7);
    filterStrings(&literalStrings, FILTER9);
    filterStrings(&literalStrings, DEBUGS);
    filterStrings(&literalStrings, SQLSTR);

    assertLiterals(&literalStrings, classes, FILTER8);

    assertLiterals2(&literalStrings, wcodes);
    assertLiterals2(&literalStrings, iacs1);
    assertLiterals2(&literalStrings, iacs2);
    assertLiterals2(&literalStrings, disp1);
    assertLiterals2(&literalStrings, disp2);
    assertLiterals2(&literalStrings, annotations);
    assertLiterals2(&literalStrings, debugws);
    assertLiterals2(&literalStrings, sqlps);
    assertLiterals2(&literalStrings, fieldnames);
    assertLiterals2(&literalStrings, regions);
    assertLiterals2(&literalStrings, dprop);
    assertLiterals2(&literalStrings, rcom);
    assertLiterals2(&literalStrings, lser);

    assertLiterals3(&literalStrings, exc2);

    addWCodes(&literalStrings);


    QFile data2(data2path);
    data2.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream output(&data2);

    QString prev("");
    int n = 0;

    foreach(auto s, literalStrings){
        if(s.relevant){
            if(s.fileName!=prev){
                output<<"##############################"<<s.fileName<<"##############################"<<endl;
                prev = s.fileName;
            }
            output<<s.value<<endl;
            n++;
        }
    }

    output<<endl<<endl<<"Talalt literalis sztringek szama: "<<n<<endl;

    data2.close();

    auto csvFileName = destDir.filePath(params.outFile); // szűrt

    int csvLineCount = generateCsv(literalStrings, csvFileName);

    zInfo("csvFileName: "+csvFileName+ " ("+QString::number(csvLineCount)+")");


    zInfo(QStringLiteral("Work1 done"))
    return 1;
}

QStringList Work1::removeFiles(QStringList list, QStringList exc){
    QStringList out;
    foreach(auto s, list){
        bool a = false;
        foreach(auto e, exc){
            if(s.contains(e)){
                a = true;
                break;
            }
        }
        if(!a)
            out<<s;
    }

    return out;
}

int Work1::generateCsv(const QList<Literal>& list, const QString& filename)
{
    QFile f(filename);
    int i = 0;
    if(!f.open(QIODevice::WriteOnly | QIODevice::Text)){
        zInfo(QStringLiteral("Hiba az output fájl megnyitásakor!"))
    }
    else{
        QChar s('\t');
        QTextStream out(&f);
        out<<"#filename"<<s<<"index"<<s<<"length"<<s<<"value"<<s<<"wordcode"<<endl;
        foreach(auto l, list){
            if(l.relevant){
                out<<l.fileName<<s<<l.index<<s<<l.length<<s<<l.value<<s<<l.wordCode<<endl;
                i++;
            }
        }
        f.close();
    }
    return i;
}

void Work1::addWCodes(QList<Literal> *list)
{
    QSet<QString> WCodes;
    QSet<QString> WCFNs;
    QString prev("");
    QString a;
    zforeach(l, *list){
        if(l->relevant){
            if(l->fileName != prev){
                prev = l->fileName;
                int i = 5;
                QStringList token(l->fileName.split(QDir::separator()));
                QString fn(token.at(token.length()-1));
                QString wcfn(fn.left(i).toUpper());
                while(WCFNs.contains(wcfn)){
                    i++;
                    wcfn = fn.left(i).toUpper();
                }
                WCFNs.insert(wcfn);
                a = wcfn;
            }
            int i = 1;
            while(WCodes.contains(QString(a+QString::number(i)))){
                i++;
            }
            QString x(a + QString::number(i));
            l->wordCode = QString(x);
            WCodes.insert(x);
        }
    }
}

QList<Exclusion> Work1::getSpecial(QStringList dest, const QString regex){
    QList<Exclusion> out;
    QRegularExpression reg(regex);
    foreach(auto d, dest){
        QString fileText(com::helper::FileHelper::load(d));
        QRegularExpressionMatchIterator i = reg.globalMatch(fileText);
        //int count = 1;
        while(i.hasNext()){
            QRegularExpressionMatch match = i.next();
            if(match.hasMatch()){
                Exclusion e;
                e.value = match.captured(1);
                e.index = match.capturedStart(1);
                e.length = match.capturedLength(1);
                e.fileName = d;
                out<<e;
                if(match.captured(2)!=nullptr)
                    e.value = match.captured(2);
                    e.index = match.capturedStart(2);
                    e.length = match.capturedLength(2);
                    e.fileName = d;
                    out<<e;
            }
        }
    }
    return out;
}

QList<Exclusion> Work1::getSpecial2(QStringList source, QString filename, const QString regexp)
{
    QList<Exclusion> out;
    QRegularExpression reg(regexp);
    foreach(auto s, source){
        if(s.endsWith(filename)){
        QString fileText(com::helper::FileHelper::load(s));
            QRegularExpressionMatch match = reg.match(fileText);
            QStringList token = match.captured(0).split(",");
            int l = 0;
            foreach(auto t, token){
                Exclusion e;
                e.value = t;
                e.index = match.capturedStart(0) + l;
                e.length = t.length();
                e.fileName = s;
                out<<e;
                l += t.length();
            }
        }
    }
    return out;
}

void Work1::assertLiterals(QList<Literal> *list, QList<Exclusion> set, const QString regexp)
{
    QRegularExpression reg(regexp);
    zforeach(l, *list){
        QRegularExpressionMatch match = reg.match(l->value);
        if(match.hasMatch()){
            auto u = match.captured(0);
            foreach(auto s, set){
                if(l->value == s.value && l->fileName == s.fileName && l->index == s.index)
                    l->relevant = false;
                else{
                    int k = 0;
                    foreach(auto t, u.split(".")){
                        if(l->value == t && l->fileName == s.fileName && l->index == s.index + k){
                            l->relevant = false;
                            break;
                        }
                        k += t.length();
                    }
                }
            }
        }
    }
}

void Work1::assertLiterals2(QList<Literal> *list, QList<Exclusion> set)
{
    zforeach(l, *list){
        foreach(auto s, set){
            if(l->value == s.value && l->fileName == s.fileName && l->index == s.index)
                l->relevant = false;
        }
    }
}

void Work1::assertLiterals3(QList<Literal> *list, QStringList exc)
{
    zforeach(l, *list){
        foreach(auto s, exc){
            if(s.endsWith('*'))
            {
                if(s.startsWith('*'))
                {
                    if(l->value.contains(s.mid(1,s.length()-2))) l->relevant = false;
                }
                else
                {
                    if(l->value.startsWith(s.left(s.length()-1))) l->relevant = false;
                }
            }
            else if(l->value == s) l->relevant = false;
        }
    }
}

//A getFileList, a killComments és ez kb ugyan azt csinálja, lehetne közös függvény
QList<Literal> Work1::getLiterals(QStringList dest, const QString regexp, const QString& filename){
    QFile data(filename);
    data.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream output(&data);
    QList<Literal> out;
    QRegularExpression reg(regexp);
    foreach(auto d, dest){
        QString fileText(com::helper::FileHelper::load(d));
        output<<d<<" tartalma:"<<Qt::endl;
        QRegularExpressionMatchIterator i = reg.globalMatch(fileText);
        //int count = 1;
        while(i.hasNext()){
            Literal l;
            QRegularExpressionMatch match = i.next();
            if(match.hasMatch() && match.captured(2).length() > 3){
                l.value = match.captured(2);
                l.fileName = d;
                l.index = match.capturedStart(2);
                l.length = match.capturedLength(2);
                l.relevant = true;
                output<<l.value<<Qt::endl;
                out<<l;
            }
        }
    }
    data.close();
    return out;
}

void Work1::filterStrings(QList<Literal> *list, const QString regexp)
{
    QRegularExpression reg(regexp);
    zforeach(l, *list){
        QRegularExpressionMatch match = reg.match(l->value);
        if(match.hasMatch()){
            l->relevant = false;
        }
    }
}

QStringList Work1::getFileList(QString in, const QString regexp)
{
    QRegularExpression reg(regexp);
    QString fileText(com::helper::FileHelper::load(in));
    QStringList out;
    QRegularExpressionMatchIterator i = reg.globalMatch(fileText);
    while(i.hasNext()){
        QRegularExpressionMatch match = i.next();
        if(match.hasMatch())    out<<match.captured(1);
    }

    return out;
}

QString Work1::getProjFolderName(QString proja){
    QStringList token = proja.split(QDir::separator());
    return token[token.size()-2];
}

QString Work1::getParentDirName(const QString &file){
    QFileInfo fi(file);
    QDir absDir = fi.absoluteDir();
    return absDir.path();
}



QStringList Work1::appendString(const QStringList &in, const QString &ezt){
    QStringList out;
    foreach(auto s, in){
        out<<Work1::append9(ezt, s);
    }
    return out;
}

void Work1::copyDir(QStringList source, QStringList dest)
{
    for(int i = 0; i < source.size(); i++){
        QFile s(source.at(i));
        QFile d(dest.at(i));
        QDir dir(getParentDirName(d.fileName()));
        if(!dir.exists())
            dir.mkpath(dir.absolutePath());
        if(d.exists())
            d.remove();
        if(QFile::copy(s.fileName(), d.fileName()))
            zInfo(QStringLiteral("Fájl sikeresen átmásolva: %1 -> %2").arg(s.fileName()).arg(d.fileName()))
    }
}



QStringList Work1::getFileList(QStringList in, const QString regex)
{
    QStringList out;
    if(in.size() == 0)
        zInfo(QStringLiteral("A .csproj fájlok listája üres!"))
    foreach (const QString &str, in) {
        out<<getFileList(str, regex);
    }
    return out;
}

QString Work1::append9(const QString &a, const QString &b)
{
    QChar s = QDir::separator();
    QString x, y;
    if(s == QChar('/')){
        x = a;
        y = b;
        x.replace("\\", "/");
        y.replace("\\", "/");
    }
    else if(s == QChar('\\')){
        x = a;
        y = b;
        x.replace("/", "\\");
        y.replace("/", "\\");
    }

    QString retVal = x + s + y;

    return retVal;
}
