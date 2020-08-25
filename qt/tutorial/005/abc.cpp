#include <QTextStream>
#include <QVector>
#include <QList>
#include <QSet>
#include <QMap>
#include <algorithm>


class book {
public:
    book(QString author, QString title);
    QString getAuthor() const;
    QString getTitle() const;
private:
    QString author;
    QString title;
};

book::book(QString author, QString title) {
    this->author = author;
    this->title = title;
}

QString book::getAuthor() const {
    return this->author;
}

QString book::getTitle() const {
    return this->title;
}

bool comapreTitle(const book &b1, const book &b2) {
    return b1.getTitle() < b2.getTitle();
}




int main(void)
{
    QTextStream out(stdout);

    //---------------------QVector 动态数组
    {
        QVector<int> vals = {1, 2, 3, 4, 5, 6, 7, 8, 9};
        vals.append(10);    //在尾部添加元素
        vals.prepend(0);    //在头部添加元素(显然这个要低效)
        out << "vals size is " << vals.size() <<",first is "<< vals.first() << ",last is "<< vals.last()<< endl;
        for (int val : vals) {  //c++ 11
            out << val <<" ";
        }
        out << endl;
    }
    //-------------------- QList
    {
        QList<QString> authors = {"jensen", "yann", "fabio", "anddy", "linken"};
        for (int i = 0; i < authors.size(); i++) {
            out << authors.at(i) << endl;
        }
        out << endl;
        authors << "lodon" << "balck";  //添加两个元素到 QList
        authors.append("hanson");   //append, prepend 都可用

        std::sort(authors.begin(), authors.end());  //调用c++标准算法

        out << "index of hanson is " << authors.indexOf("hanson") << endl;

        for (QString si : authors) {    //QList也提供基于index的随机访问
            out << si << endl;
        }
        out << endl;

        //从c++标准list
        std::list<double> stdlist;
        stdlist.push_back(1.2);
        stdlist.push_back(0.5);
        stdlist.push_back(3.14);
        QList<double> list = QList<double>::fromStdList(stdlist);
        for (double di : stdlist) {
            out << di << endl;
        }
        out << endl;

        QList<book> books = {
            book("Jason", "The Wild"),
            book("Leo", "War and peace"),
            book("Wiliam", "Hamlet"),
            book("Gue", "Yne vie")
        };
        std::sort(books.begin(), books.end(), comapreTitle);
        for (book bi : books) {
            out << bi.getAuthor() << ":" << bi.getTitle() << endl;
        }
    }
    {
        QString s = "jensen,yann,friday,fabio,linke,hanson,diro";
        //QStringList sl = s.split(",");
        QList<QString> sl = s.split(",");   //看起来 QStringList 和 QList<QString> 等效啊
        for (QString si : sl) {
            out << si << endl;
        }
        out << endl;
    }
    //------------------------QSet
    {
        out << endl;
        QList<QString> sl;
        QSet<QString> ss0 = {"four", "hanson"};
        QSet<QString> ss1;

        sl << "stdlist" << "orange" << "hanson" << "yellow" << "bule" << "pink" << "sky" << "orange" << "red";
        for (int i = 0; i < sl.size(); i++) {   //QSet 有一个 fromList() 方法
            out << "insert " << sl[i] << " into set." << endl;
            ss1.insert(sl[i]);
        }

        out << "now elements in ss1 are:[";
        for (QString si : ss1) {
            out << si << " ";
        }
        out << "]" << endl;

        out << "now elements in ss0 are:[";
        for (QString si : ss0) {
            out << si << " ";
        }
        out << "]" << endl;

        ss0.unite(ss1);

        out << "now elements in ss0 are(after unite):[";
        for (QString si : ss0) {
            out << si << " ";
        }
        out << "]" << endl;

        if (ss0.find("hanson") != ss0.end()) {
            out << "seems ss0 has element hanson !" << endl;
        } else {
            out << "seems ss0 do not has element hanson !" << endl;
        }

        QList<QString> result = ss0.values();   //QSet.values()返回集合中所有元素组成的list
        std::sort(result.begin(), result.end());  //排序

        out << "result:[";
        for (QString si : result) {
            out << si << " ";
        }
        out << "]" << endl;
        out << endl;
    }
    //--------------------------QMap
    {
        QMap<QString,int> items = {
            {"hanson", 18},
            {"jensen", 38},
            {"orange", 46},
            {"fabio", 52},
            {"pinkman", 74}
        };

        items.insert("black", 65);  //插入键值对
        items["white"] = 43;        //插入键值对

        out << "jensen's val is " << items["jensen"] << endl;           //查询键为 jensen 的值,若不存在则会插入(val为int默认值0),推荐使用value()
        out << "fabio's val is " << items.value("fabio", 100) << endl;      //查询键为 fabio 的值,若没有则返回默认值100
        if (items.contains("orange")) {                                     //查找键值对集合中是否含有键为 orange 的元素
            out << "we have items[orange] = " << items["orange"] << endl;
        } else {
            out << "we don't have this element" << endl;
        }

        QList<QString> keys = items.keys();
        QList<int> vals = items.values();
        for (auto ki : keys) {
            out << QString("items[%1]=%2.").arg(ki).arg(items.value(ki,-1)) << endl;
        }
        out << endl;

        items.remove("pinkman");

        for (auto ki : items.keys()) {
            out << QString("items[%1]=%2.").arg(ki).arg(items.value(ki,-1)) << endl;
        }
        items.clear();
    }

    return 0;
}
