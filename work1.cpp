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
//("([^"]|"").*")
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
    auto destpathname = slnparent+"_ford";

    QStringList sourceFinal;
    QStringList destFinal;

    // teszt 1
    QStringList exc;
    QStringList excByText;
    QStringList excByPrefix;
    QStringList excByPostfix;

    QDir slnParentDir(slnparent);

    auto excludefilepath = slnParentDir.filePath("exclude_file.csv"); // filenév szintű kizárás

    auto a  = com::helper::TextFileHelper::loadLines(excludefilepath);
    for(auto&i:a){
        if(i.isEmpty()) continue;
        if(i.startsWith('#')) continue;
        exc.append(i);
    }

    zInfo("excludefilepath_files: "+excludefilepath + " ("+QString::number(exc.count())+")");


    auto excludefilepath2 = slnParentDir.filePath("exclude_txt.csv"); // szöveg szintű kizárás

    a = com::helper::TextFileHelper::loadLines(excludefilepath2);

    for(auto&i:a){        
        if(i.isEmpty()) continue;
        if(i.startsWith('#')) continue;        
        if(i.startsWith("pref")){
            int l = i.length()-4;
            QString a = i.right(l).toLower();
            excByPrefix.append(a);
        }
        else if(i.startsWith("post")){
            int l = i.length()-4;
            QString a = i.right(l).toLower();
            excByPostfix.append(a);
        }
        else if(i.startsWith("\\#")) excByText.append(i.mid(1).toLower());
        else excByText.append(i.toLower());
    }

    zInfo("excludefilepath_txt: "+excludefilepath2+ " ("+QString::number(excByText.count())+")");

    if(params.inFile.endsWith(".cs"))
    {
        QFileInfo fi(params.inFile);
        auto csprojparent = getParentDirName(params.inFile);
        auto pfn = getProjFolderName(params.inFile);

        auto cslr2 = fi.fileName();
        auto source = append9(csprojparent,cslr2);
        auto dest_ = append9(pfn, cslr2);
        auto dest = append9(destpathname, dest_);

        sourceFinal<<source;
        destFinal<<dest;
    }
    else
    {
        auto projFilePath = appendString(projFileNames, slnparent);
        foreach(auto proj, projFilePath){
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
    }

    copyDir(sourceFinal, destFinal);

    QDir destDir(slnparent);//destpathname

    //auto literalStrings = getLiterals(destFinal, COMORSTR, data1path);
    auto literalStrings = getLiterals2(destFinal);

    auto data1path = destDir.filePath("literals1.csv"); // az eredeti
    zInfo("data1path: "+data1path+ " ("+QString::number(data1path.length())+")");
    LiteralsToFile(literalStrings, data1path);

    assertLiterals3(&literalStrings, excByText, AssertMode::ByLetter);
    assertLiterals3(&literalStrings, excByText, AssertMode::ByValue);
    assertLiterals3(&literalStrings, excByPrefix, AssertMode::ByPrefix);
    assertLiterals3(&literalStrings, excByPostfix, AssertMode::ByPostfix);

    addWCodes(&literalStrings);

    auto data2path = destDir.filePath("literals2.csv"); // a szűrt
    zInfo("data2path: "+data2path+ " ("+QString::number(data2path.length())+")");
    LiteralsToFile(literalStrings, data2path);

    auto csvFileName = destDir.filePath(params.outFile); // a végeredmény
    int csvLineCount = generateCsv(literalStrings, csvFileName);
    zInfo("csvFileName: "+csvFileName+ " ("+QString::number(csvLineCount)+")");

    zInfo(QStringLiteral("Work1 done"))
    return 1;
}


void Work1::LiteralsToFile(const QList<Literal> &literals, const QString &fileName)
{
    QFile data2(fileName);
    data2.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream output(&data2);

    QString prev("");
    int n = 0;

    foreach(auto s, literals){
        if(s.relevant){
            if(s.fileName!=prev){
                output<<"##############################"<<s.fileName<<"##############################"<<Qt::endl;
                prev = s.fileName;
            }
            output<<s.value<<Qt::endl;
            n++;
        }
    }

    output<<Qt::endl<<Qt::endl<<"Talalt literalis sztringek szama: "<<n<<Qt::endl;

    data2.close();
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
        out<<"#filename"<<s<<"index"<<s<<"length"<<s<<"value"<<s<<"wordcode"<<Qt::endl;
        foreach(auto l, list){
            if(l.relevant){
                out<<l.fileName<<s<<l.index<<s<<l.length<<s<<l.value<<s<<l.wordCode<<Qt::endl;
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
                {
                    e.value = match.captured(2);
                    e.index = match.capturedStart(2);
                    e.length = match.capturedLength(2);
                    e.fileName = d;
                    out<<e;
                }
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

void Work1::assertLiterals3(QList<Literal> *list, QStringList exc, AssertMode mode)
{
    zforeach(l, *list){
        QString txt;
        switch(mode){
        case AssertMode::ByLetter:
        case AssertMode::ByValue: txt = l->value.toLower(); break;
        case AssertMode::ByPrefix: txt = l->pref.toLower(); break;
        case AssertMode::ByPostfix: txt = l->postf.toLower(); break;
        default: txt = "";
        }
        if(mode == AssertMode::ByLetter){
            int i;
            for (i=0;i<txt.size();i++){
                if(txt[i].isLetter()) break;
            }
            if(i==txt.size()) l->relevant=false;
        } else{

            foreach(auto s, exc){

//            if(mode == AssertMode::ByPrefix && l->value.startsWith("kelencntr@gmail."))
//            {
//                zInfo("exc");
//            }
            if(s.endsWith('*'))
            {
                if(s.startsWith('*'))
                {
                    if(txt.contains(s.mid(1,s.length()-2))) l->relevant = false;
                }
                else
                {
                    if(txt.startsWith(s.left(s.length()-1))) l->relevant = false;
                }
            } else if(s.startsWith('*'))
            {
                QString token = s.mid(1 ,s.length()-1);
                if(txt.endsWith(token))
                    l->relevant=false;
            }
            else if(txt == s) l->relevant = false;
            }
        }
    }
}

//A getFileList, a killComments és ez kb ugyan azt csinálja, lehetne közös függvény
QList<Literal> Work1::getLiterals(QStringList inFileNames, const QString regexp, const QString& outFileName){
    QFile data(outFileName);
    data.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream output(&data);
    QList<Literal> out;
    QRegularExpression reg(regexp);
    foreach(auto d, inFileNames){
        auto l = getLiterals(d, reg, &output);
        out<<l;
    }
    data.close();
    return out;
}

QList<Literal> Work1::getLiterals(const QString inFileName, const QRegularExpression reg, QTextStream* output){
    QList<Literal> out;

    QString fileText(com::helper::FileHelper::load(inFileName));
    int maxIx = fileText.length()-1;
    *output<<inFileName<<" tartalma:"<<Qt::endl;
    QRegularExpressionMatchIterator i = reg.globalMatch(fileText);
    //int count = 1;
    while(i.hasNext()){
        Literal l;
        QRegularExpressionMatch match = i.next();
        if(match.hasMatch()){
            if(match.captured(2).length() <= 1) continue;
            l.value = match.captured(2);
            l.fileName = inFileName;
            l.index = match.capturedStart(2);
            l.length = match.capturedLength(2);
            l.relevant = true;

            int lix1 = l.index; //end

            int lix2 = l.index-20;
            if(lix2<=0) lix2=0;
            int len = lix1-lix2;


            l.pref = fileText.mid(lix2, len);

            lix2 = l.pref.lastIndexOf('\n');
            if(lix2!=-1) l.pref = l.pref.mid(lix2,l.pref.length()-lix2);

            lix1 = l.index+l.length;//start
            lix2 = fileText.indexOf('\n');//lix1+20;//end
            if(lix2==-1) lix2=lix1+20;
            if(lix2>maxIx) lix2=maxIx;
            len = lix2-lix1;

            l.postf = fileText.mid(lix1,len);

            lix2 = l.postf.indexOf('\n');
            if(lix2!=-1) l.postf = l.postf.mid(0,lix2);

            *output<<l.value<<Qt::endl;
            out<<l;
        }
    }
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

/**/

QList<Literal> Work1::getLiterals2(const QStringList& inFileNames){
    QList<Literal> out;
    foreach(auto d, inFileNames){
        auto l = getLiterals2(d);
        out<<l;
    }
    return out;
}

QList<Literal> Work1::getLiterals2(const QString& inFileName){
    QList<Literal> out;

    QString txt(com::helper::FileHelper::load(inFileName));
    int ix1=0;
    int maxIx = txt.length()-1;

    int comments = 0;
    bool end = false;
    do
    {        
        ix1 = txt.indexOf('"', ix1);
        if(ix1==-1) break;


        Literal l = getLiteral2(txt, ix1);
        // ha kommentben van?

        if(l.isValid())
        {
            if(l.value=="7")
            {
                zInfo("upsz");
            }
            int ix5 = txt.lastIndexOf("//", l.index);
            if(ix5!=-1) {
                ix1++; continue;
            }
            int ix6 = txt.lastIndexOf("/*", l.index);
            if(ix6!=-1){
                int ix7 = txt.lastIndexOf("*/", l.index);
                if((ix7==-1)||(ix7<ix6))
                {
                    ix1++;
                    continue;
                }
            }
//            else
//            {
                l.fileName = inFileName;
                QString pref= getPrefix(txt, l, 20);
                l.pref = pref;
                QString postf= getPostfix(txt, l, 20);
                l.postf = postf;

                out<<l;
                if(l.indexEnd>0)
                    ix1=l.indexEnd+1;
                else
                    ix1++;
  //          }
        }
        else
        {
            ix1++;
        }
        end = ix1>=maxIx;
    }while(!end);

    return out;
}

Literal Work1::getLiteral2(const QString &txt, int ix1)
{
    int preIx = ix1-1;
    if(preIx>=0){
        if(txt[preIx]=='@') return getRawString(txt,ix1);
        if(txt[preIx]=='$') return getInterpolatedString(txt, ix1);
    }
    return getNormalString(txt,ix1);
}

Literal Work1::getNormalString(const QString &txt, int ix1){
    ix1++;
    int ix2=ix1;
    bool end=false;
    do{
        ix2 = txt.indexOf('"', ix2);
        if(ix2==-1) break;
        int preIx=ix2-1;
        auto pre = txt[preIx];
        if(pre=='\\') ix2++;
        else end = true;
    }while(!end);
    if(ix2==-1) return {};

    Literal l;
    l.value = txt.mid(ix1, ix2-ix1);
    l.index = ix1;
    l.indexEnd = ix2;
    l.length = ix2-ix1;
    l.relevant = true;
    l.type = Literal::Type::normal;

    return l;
}

Literal Work1::getRawString(const QString &txt, int ix1){
    return {};
}

Literal Work1::getInterpolatedString(const QString &txt, int ix1){    
    ix1++;
    int ix2=-1;

    int embeddings = 0;
    for(int i=ix1;i<txt.length();i++){
        if(txt[i]=='\\') {i++; continue;}
        if(txt[i]=='{') embeddings++;
        if(txt[i]=='}') embeddings--;
        if(embeddings==0){
            if(txt[i]=='"') {ix2 = i; break;}
        }
    }

    if(ix2==-1) return {};

    Literal l;
    l.value = txt.mid(ix1, ix2-ix1);
    l.index = ix1;
    l.indexEnd = ix2;
    l.length = ix2-ix1;
    l.relevant = true;
    l.type = Literal::Type::normal;

    return l;
}

QString Work1::getPrefix(const QString &txt, const Literal &l, int plen)
{
    int lix2 = l.index-plen;
    if(lix2<=0) lix2=0;
    int len = l.index-lix2;

    auto pref =  txt.mid(lix2, len);
    lix2 = pref.lastIndexOf('\n');
    if(lix2!=-1) pref = pref.mid(lix2,pref.length()-lix2);

    return pref;
}
QString Work1::getPostfix(const QString &txt, const Literal &l, int plen)
{
    int maxIx = txt.length()-1;

    int lix1 = l.indexEnd;//index+l.length;//start
    int lix2 = txt.indexOf('\n');//lix1+20;//end
    if(lix2==-1) lix2=lix1+plen;
    if(lix2>maxIx) lix2=maxIx;
    int len = lix2-lix1;

    auto postf = txt.mid(lix1,len);
    lix2 = postf.indexOf('\n');
    if(lix2!=-1) postf = postf.mid(0,lix2);

    return postf;
}
