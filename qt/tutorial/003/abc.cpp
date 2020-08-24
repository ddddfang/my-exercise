#include <QTextStream>
#include <QFile>

int main(void)
{
    QTextStream out(stdout);

    //---------------------初始化 QString 的几种方式
    {
        //QString a = "love";
        //QString a("love");
        //std::string b  ="love"; //qtcore支持标准c++
        //QString a = b.c_str();

        //char v[] = "love";    //使用 c 字符数组的方式
        //QString a(v);

        std::string b = "love";
        QString a = QString::fromLatin1(b.data(), b.size());// pointer + size
        out << a << endl;
        std::string c = a.toStdString();    //std string 和 QString 的互转
        out << c.c_str() << endl;
    }

    //------------------- 修改字符串
    {
        QString a = "love ";
        a.append(" chess for 10 years.");    //modified in-place
        a.prepend("I ");        //与其他语言稍有差异的地方在于,这里a是真正被修改了

        out << a << endl;
        out << "the stream has " << a.count() << " chars" <<endl;

        out << a.toUpper() << endl; //return a copy of string
        out << a.toLower() << endl; //返回a的一份拷贝,a本身没有被修改

        a.remove(10, 3);            //remove index=10处的3个字符
        out << "after remove(10, 3):" << a << endl;
        a.replace(10, 3, "hello");  //将 index=10处的3个字符换成 "hello"
        out << "after replace:" << a << endl;
        a.clear();
        if (a.isEmpty()) {
            out << "a is empty.." << endl;
        } else {
            out << "a is not empty.." << endl;
        }
    }

    //------------------- 遍历字符串
    {
        QString a = "I love chess for about 13 years.";
        for(int i = 0; i < a.size(); i++) { //a.size() a.count() a.length() 等效
            //out << "the no." << i << " char is " << a[i] <<endl;
            out << "the no." << i << " char is " << a.at(i) <<endl;
        }
        out << endl;
        foreach (QChar c, a) {  //c++11 的 foreach 用法
            if (c.isDigit()) {          //0-9
                out << c << "(digit) ";
            } else if (c.isLetter()) {  //a-zA-Z 字符
                out << c << "(letter) ";
            } else if (c.isSpace()) {   //空格
                out << c << "(space) ";
            } else if (c.isPunct()) {   //特殊字符比如 .
                out << c << "(punct) ";
            } else {
                out << c << " ";
            }
        }
        out << endl;
        for (QChar *it = a.begin(); it != a.end(); it++) {  //迭代器方式
            out << *it << " ";
        }
        out << endl;

        //out << "the no.13" << " char is " << a[13] <<endl;    //当访问越界的时候不会crash而是返回 null,
        out << "the no.13" << " char is " << a.at(13) <<endl;   //可以用 a.at(i).isNull() 或 a[i].isNull() 来判断
    }

    //------------------ 格式化输出
    {
        QString f = "hello, my name is %1, and I am %2 years old.";
        QString name = "fang";
        int age = 30;
        out << f.arg(name).arg(age) << endl;

        //------------------- 子字符串
        QString fmt = QString("hello, my name is %1, and I am %2 years old.").arg(name).arg(age);
        out << fmt <<endl;
        out << fmt.right(4) <<endl; //打印最右边的4个字符组成的子字符串
        out << fmt.left(5) <<endl;  //打印最左边的5个字符组成的子字符串
        out << fmt.mid(7, 5) <<endl;    //打印从 index=7开始长度为5字符的子字符串

        QStringRef sub(&fmt, 7, 10);    //QStringRef 是 QString 的 read-only 版本, index=7开始长度为10字符的子字符串
        out << sub.toString() <<endl;   //sub.toString() 将 QStringRef 转化为 QString
    }

    //------------------- 字符串比较
    {
        QString a = "hello";
        QString b = "Hello";
        if (QString::compare(a, b) == 0) {
            out << "a == b." << endl;
        } else if (QString::compare(a, b) < 0) {
            out << "a < b." << endl;
        } else {
            out << "a > b." << endl;
        }

        if (QString::compare(a, b, Qt::CaseInsensitive) == 0) { //大小写不敏感
            out << "a == b." << endl;
        } else {
            out << "a != b." << endl;
        }
    }
    //------------------ 字符串与 number 类型的互转
    {
        QString a0 = "12.34";
        QString a1 = "76";
        float b = a0.toFloat(); //QString 转 数字, QString::toFloat(), toInt(), toLong()
        //int c = a0.toInt();  //转换不了的会得到0
        int c = a1.toInt();

        QString a2;
        a2.setNum(b);   //数字 转 QString,直接 setNum()
        out << "b is " << b <<endl;
        out << "c is " << c <<endl;
        out << "a2 is " << a2 <<endl;
    }
    //------------------ 字符串对齐
    {
        QString a = "name:";
        QString b = "telephone:";
        QString c = "address:";
        out << a.rightJustified(20, ' ') << "Robert" << endl;   //a.rightJustified() 将a使用空格填充到20个字符宽,原有内容右对齐
        out << b.rightJustified(20, ' ') << "15001547846" << endl;
        out << c.rightJustified(20, ' ') << "Hiajin Road. Number 63 Street.ShangHai.China." << endl;

        out << a.leftJustified(20, ' ') << "Robert" << endl;   //a.rightJustified() 将a使用空格填充到20个字符宽,原有内容右对齐
        out << b.leftJustified(20, ' ') << "15001547846" << endl;
        out << c.leftJustified(20, ' ') << "Hiajin Road. Number 63 Street.ShangHai.China." << endl;
    }
    //---------------------------
    QFile file("./abc.cpp");
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning("cannot open file");
        return -1;
    }
    QTextStream in(&file);  //使用 QTextStream 操作打开的文件
    QString all = in.readAll();
    out << all << endl;
    out << all.toHtmlEscaped() << endl;

    file.close();
    return 0;
}
