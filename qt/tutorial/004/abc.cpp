#include <QTextStream>
#include <QDate>
#include <QTime>
#include <QDateTime>

int main(void)
{
    QTextStream out(stdout);

    //---------------------初始化
    {
        QDate d1(2015, 7, 16);
        if (d1.isValid()) {
            out << "d1 is " << d1.toString() << endl;
        }

        QDate d2;
        d2.setDate(2015, 2, 14);
        out << "d2 is " << d2.toString() << endl;

        QDate d3(2015, 2, 30);
        if (d3.isValid()) { //检查合法性
            out << "d3 is " << d3.toString() << endl;
        } else {
            out << "d3 is not valid." << endl;
        }

        if (d1 < d2) {  //流逝的时间越久 数值越大
            out << "d1 < d2" << endl;
        } else {
            out << "d1 > d2" << endl;
        }

        QTime t1(17, 30, 12, 55);   //下午5点30分12秒 55ms
        out << "t1 is " << t1.toString() << endl;

        QTime t2;
        t2.setHMS(13, 52, 45, 155);//
        out << "t2 is " << t2.toString() << endl;

        if (t1 < t2) {  //流逝的时间越久 数值越大
            out << "t1 < t2" << endl;
        } else {
            out << "t1 > t2" << endl;
        }
    }
    //--------------------- 获取当前日期和时间
    {
        QDate cd = QDate::currentDate();
        QTime ct = QTime::currentTime();
        out << QString("current date (TextDate) is ").leftJustified(50, ' ') << cd.toString(Qt::TextDate) << endl;
        out << QString("current date (ISODate) is ").leftJustified(50, ' ') << cd.toString(Qt::ISODate) << endl;
        out << QString("current date (SystemLocaleShortDate) is ").leftJustified(50, ' ') << cd.toString(Qt::SystemLocaleShortDate) << endl;
        out << QString("current date (SystemLocaleLongDate) is ").leftJustified(50, ' ') << cd.toString(Qt::SystemLocaleLongDate) << endl;
        out << QString("current date (SystemLocaleDate) is ").leftJustified(50, ' ') << cd.toString(Qt::SystemLocaleDate) << endl;
        out << QString("current date (LocaleDate) is ").leftJustified(50, ' ') << cd.toString(Qt::LocaleDate) << endl;
        out << QString("current date (DefaultLocaleShortDate) is ").leftJustified(50, ' ') << cd.toString(Qt::DefaultLocaleShortDate) << endl;
        out << QString("current date (DefaultLocaleLongDate) is ").leftJustified(50, ' ') << cd.toString(Qt::DefaultLocaleLongDate) << endl;
        out << QString("current date is ").leftJustified(50, ' ') << cd.toString("yyyy*MM*dd") << endl; //使用自定义格式
        int wd = cd.dayOfWeek();
        out << wd << endl;

        out << QString("current time (TextDate) is ").leftJustified(50, ' ') << ct.toString(Qt::TextDate) << endl;
        out << QString("current time (ISODate) is ").leftJustified(50, ' ') << ct.toString(Qt::ISODate) << endl;
        out << QString("current time (SystemLocaleShortDate) is ").leftJustified(50, ' ') << ct.toString(Qt::SystemLocaleShortDate) << endl;
        out << QString("current time (SystemLocaleLongDate) is ").leftJustified(50, ' ') << ct.toString(Qt::SystemLocaleLongDate) << endl;
        out << QString("current time (SystemLocaleDate) is ").leftJustified(50, ' ') << ct.toString(Qt::SystemLocaleDate) << endl;
        out << QString("current time (LocaleDate) is ").leftJustified(50, ' ') << ct.toString(Qt::LocaleDate) << endl;
        out << QString("current time (DefaultLocaleShortDate) is ").leftJustified(50, ' ') << ct.toString(Qt::DefaultLocaleShortDate) << endl;
        out << QString("current time (DefaultLocaleLongDate) is ").leftJustified(50, ' ') << ct.toString(Qt::DefaultLocaleLongDate) << endl;
        out << QString("current time is ").leftJustified(50, ' ') << ct.toString("hh*mm*ss.") << endl; //使用自定义格式
    }
    //----------------------- 判断是否是闰年
    {
        QList<int> years ({2010, 2011, 2015, 2020});
        foreach (int year, years) {
            if (QDate::isLeapYear(year)) {  //判断闰年
                out << QString("%1 is a leap year").arg(year) <<endl;
            } else {
                out << QString("%1 is not a leap year").arg(year) <<endl;
            }
        }
    }
    {
        QList<QString> months;
        QDate cd = QDate::currentDate();

        months.append("January");
        months.append("February");
        months.append("March");
        months.append("April");
        months.append("May");
        months.append("June");
        months.append("July");
        months.append("August");
        months.append("September");
        months.append("Qctober");
        months.append("November");
        months.append("December");

        out << "There are " << cd.daysInMonth() << " days in " << months.at(cd.month() - 1) << endl;
        out << "There are " << cd.daysInYear() << " days in " << QString::number(cd.year()) << endl;
    }
    {
        QDate target(2020, 12, 24);
        QDate cd = QDate::currentDate();

        QDate date_55later = cd.addDays(50);
        out << date_55later.toString() << endl;

        out << "there are " << cd.daysTo(target) << " days  till 2020.12.24" <<endl;
    }
    //----------------------使用 QDateTime
    {
        QDateTime cdt = QDateTime::currentDateTime();
        out << cdt.toString() << endl;
        out << cdt.toUTC().toString() << endl;
        out << cdt.toLocalTime().toString() << endl;
    }

    return 0;
}
