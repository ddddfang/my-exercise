# http://zetcode.com/gui/pyqt5/

#!/usr/bin/python3

from PyQt5.QtCore import QDate, QTime, QDateTime, Qt


now = QDate.currentDate()
print(now.toString(Qt.ISODate))
print(now.toString(Qt.DefaultLocaleLongDate))

datetime = QDateTime.currentDateTime()
#print(datetime.toString())
print("Local datetime: ", datetime.toString(Qt.ISODate))
print("Universal datetime: ", datetime.toUTC().toString(Qt.ISODate))


time = QTime.currentTime()
print(time.toString(Qt.DefaultLocaleLongDate))


xmas1 = QDate(2016, 12, 24)
xmas2 = QDate(2017, 12, 24)
now = QDate.currentDate()
dayspassed = xmas1.daysTo(now)
print("{0} days have passed since last XMas".format(dayspassed))
nofdays = now.daysTo(xmas2)
print("There are {0} days until next XMas".format(nofdays))


now = QDateTime.currentDateTime()

print("Today:", now.toString(Qt.ISODate))
print("Adding 12 days: {0}".format(now.addDays(12).toString(Qt.ISODate)))
print("Subtracting 22 days: {0}".format(now.addDays(-22).toString(Qt.ISODate)))

print("Adding 50 seconds: {0}".format(now.addSecs(50).toString(Qt.ISODate)))
print("Adding 3 months: {0}".format(now.addMonths(3).toString(Qt.ISODate)))
print("Adding 12 years: {0}".format(now.addYears(12).toString(Qt.ISODate)))



now = QDateTime.currentDateTime()
unix_time = now.toSecsSinceEpoch() 
print(unix_time)
d = QDateTime.fromSecsSinceEpoch(unix_time)
print(d.toString(Qt.ISODate))



