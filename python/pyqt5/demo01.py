import sys
from PyQt5.QtWidgets import QApplication, QWidget

if __name__ == '__main__':

    app = QApplication(sys.argv)

    w = QWidget()
    w.resize(250, 150)  # 250px wide and 150px high. 
    w.move(300, 300)
    w.setWindowTitle('Simple')
    w.show()

    # enter the mainloop of the application. 
    # The event handling starts from this point.
    # The mainloop receives events from the window 
    # system and dispatches them to the application widgets.
    # The mainloop ends if we call the exit() method
    # or the main widget is destroyed. The sys.exit()
    # method ensures a clean exit. The environment
    # will be informed how the application ended.
    sys.exit(app.exec_())   # exec is a Python keyword. And thus, exec_() was used instead
