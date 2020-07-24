g++ -o version 001.cpp \
-std=c++11 \
-I/home/fang/Qt5.12.0/5.12.0/gcc_64/include/ \
-I/home/fang/Qt5.12.0/5.12.0/gcc_64/include/QtCore/ \
-L/home/fang/Qt5.12.0/5.12.0/gcc_64/lib -lQt5Core -fPIC


#编译的时候:

#----------------------------------------------------------------

#/usr/bin/ld: warning: libicui18n.so.56, needed by /home/fang/Qt5.12.0/5.12.0/gcc_64/lib/libQt5Core.so, not found (try using -rpath or -rpath-link)
#
#提示缺少链接文件，libicui18n.so.52，使用命令 ldd /home/fang/Qt5.12.0/5.12.0/gcc_64/lib/libQt5Core.so 查找到所有的链接文件，其中就有libicui18n.so.52的链接路径：
#libicui18n.so.56 => /home/fang/Qt5.12.0/5.12.0/gcc_64/lib/libicui18n.so.56 (0x00007ff1166b2000)
#
#将此路径添加到环境变量LD_LIBRARY_PATH中；
#export LD_LIBRARY_PATH=/home/fang/Qt5.12.0/5.12.0/gcc_64/lib:$LD_LIBRARY_PATH
#再次编译就不会报错了。


#----------------------------------------------------------------

#-fPIC http://blog.sina.com.cn/s/blog_54f82cc201011op1.html 
#产生的代码中，没有绝对地址，全部使用相对地址，故而代码可以被加载器加载到内存的任意
#位置，都可以正确的执行。这正是共享库所要求的，共享库被加载时，在内存的位置不是固定的。
#gcc -shared -fPIC -o 1.so 1.c
