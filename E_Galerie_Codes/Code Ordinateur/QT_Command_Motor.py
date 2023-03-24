import cv2
import numpy as np
import os
import time
from PyQt6.QtCore import pyqtSignal, pyqtSlot, Qt, QThread
from PyQt5.QtMultimedia import QMediaContent, QMediaPlayer
from PyQt5.QtMultimediaWidgets import QVideoWidget
from PyQt6.QtCore import QDateTime, Qt, QTimer
from PyQt6.QtWidgets import (QApplication, QCheckBox, QComboBox, QDateTimeEdit,
        QDial, QDialog, QGridLayout, QGroupBox, QHBoxLayout, QLabel, QLineEdit,
        QProgressBar, QPushButton, QRadioButton, QScrollBar, QSizePolicy,QStyle,
        QSlider, QSpinBox, QStyleFactory, QTableWidget, QTabWidget, QTextEdit,
        QVBoxLayout, QWidget)
from PyQt6.QtGui import QImage, QPixmap 
from PyQt6 import QtGui
import asyncio
import nest_asyncio
import websockets
import keyboard

nest_asyncio.apply()

global direction
direction = "stop"

async def recv(websocket):
    async for message in websocket:
        print(message)
        await websocket.send(direction)
        
async def run_websocket_server():
    async with websockets.serve(recv, "172.20.10.2", 8084): #adresse ip 
        await asyncio.Future()  # run forever

class WebSocketThread(QThread):
    def __init__(self, parent=None):
        super(WebSocketThread, self).__init__(parent)

    def run(self):
        asyncio.run(run_websocket_server())

class EGallery(QWidget):
    def __init__(self, parent=None):
        super(EGallery, self).__init__(parent)
        
        self.disply_width = 640
        self.display_height = 480
        
        styleComboBox = QComboBox()
        styleComboBox.addItems(QStyleFactory.keys())
        
        statutLabel = QLabel("Statut : ")
        ActualStatutLabel = QLabel("not connected")
        IPLabel = QLabel("IP : ")
        ActualIPLabel = QLabel("0.0.0.0")

        self.createLeftGroupBox()
        self.createMiddleBox2()
        
        topLayout = QHBoxLayout()
        topLayout.addWidget(statutLabel)
        topLayout.addWidget(ActualStatutLabel)
        topLayout.addWidget(IPLabel)
        topLayout.addWidget(ActualIPLabel)
        topLayout.addStretch(1)

        mainLayout = QGridLayout()
        mainLayout.addLayout(topLayout, 0, 0, 1, 2)
        mainLayout.addWidget(self.LeftGroupBox, 1, 0)
        mainLayout.addWidget(self.MiddleBox2, 1, 2)

        mainLayout.setRowStretch(1, 1)
        mainLayout.setRowStretch(2, 1)
        mainLayout.setColumnStretch(0, 1)
        mainLayout.setColumnStretch(1, 1)
        self.setLayout(mainLayout)
        
        self.setWindowTitle("EGalerie")
    
    def createMiddleBox2(self):
        self.MiddleBox2 = QGroupBox("Visualization :")
        
        self.image_label = QLabel(self)
        self.image_label.resize(self.disply_width, self.display_height)
        
        layout = QHBoxLayout()
        layout.addWidget(self.image_label)
        
        self.MiddleBox2.setLayout(layout)
        
    def createLeftGroupBox(self):
        self.LeftGroupBox = QGroupBox("Manual Mode :")

        ForwardPushButton = QPushButton("Forward (Z)")
        ForwardPushButton.setShortcut("Z")
        ForwardPushButton.setDefault(False)
        ForwardPushButton.clicked.connect(self.ForwardPushButton_clicked)
        
        LeftPushButton = QPushButton("Left (Q)")
        LeftPushButton.setShortcut("Q")
        LeftPushButton.setDefault(False)
        LeftPushButton.clicked.connect(self.LeftPushButton_clicked)
        
        BackwardPushButton = QPushButton("Backward (S)")
        BackwardPushButton.setShortcut("S")
        BackwardPushButton.setDefault(False)
        BackwardPushButton.clicked.connect(self.BackwardPushButton_clicked)
        
        RightPushButton = QPushButton("Right (D)")
        RightPushButton.setShortcut("D")
        RightPushButton.setDefault(False)
        RightPushButton.clicked.connect(self.RightPushButton_clicked)
        
        LaserPushButton = QPushButton("Laser (L)")
        LaserPushButton.setShortcut("L")
        LaserPushButton.setDefault(False)
        LaserPushButton.clicked.connect(self.LaserPushButton_clicked)
        
        StopPushButton = QPushButton("Stop (Spacebar)")
        StopPushButton.setShortcut("space")
        StopPushButton.setDefault(False)
        StopPushButton.clicked.connect(self.StopPushButton_clicked)

        layout = QVBoxLayout()
        layout.addWidget(ForwardPushButton)
        layout.addWidget(LeftPushButton)
        layout.addWidget(BackwardPushButton)
        layout.addWidget(RightPushButton)
        layout.addWidget(LaserPushButton)
        layout.addWidget(StopPushButton)        
        layout.addStretch(1)
        self.LeftGroupBox.setLayout(layout)
        
    def ForwardPushButton_clicked(self):
        print("Button avanc clicked")
        global direction
        direction = "forward"
        
    def LeftPushButton_clicked(self):
        print("Button gauche clicked")
        global direction
        direction = "left"
        
    def RightPushButton_clicked(self):
        print("Button droite clicked")
        global direction
        direction = "right"
        
    def BackwardPushButton_clicked(self):
        print("Button arrier clicked")
        global direction
        direction = "backward"
        
    def LaserPushButton_clicked(self):
        print("Button laser clicked")
    
    def StopPushButton_clicked(self):
        print("Button stop clicked")
        global direction
        direction = "stop"
        
    def variableTest(self):
        var = 0
        print(var)
    
    def createRightGroupBox(self):
        self.RightGroupBox = QGroupBox("Actual Datas :")
        
        speedLabel = QLabel("Actual speed : ")
        ActualSpeedLabel = QLabel("")
        
        xLabel = QLabel("X : ")
        
        yLabel = QLabel("Y : ")

        layout = QVBoxLayout()
        layout.addWidget(speedLabel)
        layout.addWidget(ActualSpeedLabel)
        layout.addWidget(xLabel)
        layout.addWidget(yLabel)
        layout.addStretch(1)
        self.RightGroupBox.setLayout(layout)
        
class MyApplication(QApplication):
    def __init__(self, args):
        super().__init__(args)

        self.gallery = EGallery()
        self.gallery.show()

if __name__ == '__main__':
    import sys
    app = MyApplication(sys.argv)
    key = cv2.waitKey(5)
    if key == (ord('q')):
        cv2.destroyAllWindows()
    
    websocket_thread = WebSocketThread()
    websocket_thread.start()
    
    sys.exit(app.exec())

