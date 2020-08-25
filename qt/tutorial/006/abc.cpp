#include <QTextStream>

#include <QVector>
#include <QList>
#include <QSet>
#include <QMap>
#include <algorithm>

#include <QFileInfo>
#include <QFile>
#include <QDir>


int main(int argc, char *argv[])
{
    QTextStream out(stdout);

    if (argc < 2) {
        qWarning("usage:xx xx");
        return -1;
    }

    QString filename = argv[1];
    QFile file(filename);
    if (!file.exists()) {    //判断file是否存在
        qWarning("file not exists.");
        return -1;
    }
    QFileInfo fileinfo(filename);
    qint64 size = fileinfo.size();
    QString owner = fileinfo.owner();
    QString group = fileinfo.group();
    QString absPath = fileinfo.absoluteFilePath();
    QString baseName = fileinfo.baseName();
    QString cbaseName = fileinfo.completeBaseName();
    QString fileName = fileinfo.fileName();
    QString suffix = fileinfo.suffix();
    QString cuffix= fileinfo.completeSuffix();
    out << "file size is " << size <<" bytes."<< endl;
    out << "file owner is " << owner << endl;
    out << "absPath is " << absPath << endl;
    out << "baseName is " << baseName << endl;
    out << "cbaseName is " << cbaseName << endl;
    out << "fileName is " << fileName << endl;
    out << "suffix is " << suffix << endl;
    out << "cuffix is " << cuffix << endl;

    //读取文件
    QList<QString> lines;
    if(file.open(QIODevice::ReadOnly)) {   //只读方式打开文件
        QTextStream in(&file);  //使用 QTextStream 操作打开的文件

        //QString all = in.readAll(); //看起来内容最后带了\n,虽然文件中并没有,
        //all.chop(1);                //去掉最后一个字符\n 这样这种方式就不会多一个空行
        //lines = all.split("\n");    //

        while (!in.atEnd()) {
            QString line = in.readLine();
            lines.append(line);
        }
        file.close();           //
    } else {
        qWarning("file open fail.");
        return -1;
    }

    for (int i = 0; i < lines.size(); i++) {
        out << "line " << i << ":" << lines[i] << endl;
    }

    //写入文件
    QString ofilename = "./out.txt";
    QFile ofile(ofilename);
    if (ofile.open(QIODevice::WriteOnly)) {
        QTextStream out(&ofile);  //使用 QTextStream 操作打开的文件,看起来out不会导致冲突

        for (QString li : lines) {
            out << li << endl;
        }

        ofile.close();
    } else {
        qWarning("file open fail.");
        return -1;
    }

    //拷贝文件
    QFile::copy(filename, "./copyed.txt");

    //目录相关
    //QDir dir;
    //if (dir.mkdir("test-dir")) {
    //    out << "mkdir successfully." << endl;
    //}
    //if (dir.mkdir("test-dir2")) {
    //    out << "mkdir2 successfully." << endl;
    //}
    //if (dir.exists("test-dir2")) {
    //    dir.rename("test-dir2", "renamed-dir");
    //}
    //if (dir.mkpath("test-dir2/test-sub-dir")) {
    //    out << "mkpath successfully." << endl;
    //}
    out << "current path: " << QDir::currentPath() << endl;
    out << "home path: " << QDir::homePath() << endl;
    out << "temp path: " << QDir::tempPath() << endl;
    out << "root path: " << QDir::rootPath() << endl;

    QDir dir(QDir::currentPath());
    dir.setFilter(QDir::Files | QDir::AllDirs);
    dir.setSorting(QDir::Size | QDir::Reversed);
    QFileInfoList fl = dir.entryInfoList();
    for (QFileInfo fi : fl) {
        out << "name: " << fi.fileName() << (fi.isDir() ? " d":" f")<< endl;    //可以利用递归列出某目录下所有文件
    }

    return 0;
}
