#include <QTextStream>

int main(void)
{
    QTextStream out(stdout);

    //---------------------初始化 QString 的几种方式
    //QString a = "love";
    //QString a("love");
    //std::string b  ="love"; //qtcore支持标准c++
    //QString a = b.c_str();

    //char v[] = "love";    //使用c 字符数组的方式
    //QString a(v);

    std::string b = "love";
    QString a = QString::fromLatin1(b.data(), b.size());// pointer + size


    //-------------------
    a.append(" chess.");    //modified in-place
    a.prepend("I ");        //与其他语言稍有差异的地方在于,这里a是真正被修改了

    out << a << endl;
    out << "the stream has " << a.count() << " chars" <<endl;

    out << a.toUpper() << endl; //return a copy of string
    out << a.toLower() << endl; //返回a的一份拷贝,a本身没有被修改


    //------------------- access chars in string
    for(int i = 0; i < a.size(); i++) { //a.size() a.count() a.length() 等效
        //out << "the no." << i << " char is " << a[i] <<endl;
        out << "the no." << i << " char is " << a.at(i) <<endl;
    }

    //out << "the no.13" << " char is " << a[13] <<endl;    //当访问越界的时候不会crash而是返回 null,
    out << "the no.13" << " char is " << a.at(13) <<endl;   //可以用 a.at(i).isNull() 或 a[i].isNull() 来判断

    //------------------ 格式化
    QString f = "hello, my name is %1, and I am %2 years old.";
    QString name = "fang";
    int age = 30;
    out << f.arg(name).arg(age) << endl;

    //------------------- 子字符串
    QString fmt = QString("hello, my name is %1, and I am %2 years old.").arg(name).arg(age);
    out << fmt <<endl;
}
