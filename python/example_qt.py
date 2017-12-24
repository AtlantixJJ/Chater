# coding: utf-8
import PyQt5.QtGui as G  
import PyQt5.QtCore as C
import PyQt5.QtWidgets as W
import sys
  
class MyWindow(W.QWidget):  
    def __init__(self):  
        super(MyWindow,self).__init__()  

app = W.QApplication(sys.argv) 
windows = MyWindow()  
label = W.QLabel(windows)     #在窗口中绑定label  
label.setText("hello world")
windows.show()
sys.exit(app.exec_()) 