from tkinter import *

class table(object):

    def __init__(self, master, row, colum):
        self.master = master
        self.row = row
        self.colum = colum
        self.createTable()

    def createTable(self):
        self.table = []
        Row1 = []
        l = Label(self.master, text="%10s" % ("Team Id"), relief=RIDGE, width = 10)
        l.grid(row=0, column=0, sticky=NSEW)
        Row1.append(l)
        l = Label(self.master, text="%10s" % ("Gold"), relief=RIDGE, width = 10)
        l.grid(row=0, column=1, sticky=NSEW)
        Row1.append(l)
        l = Label(self.master, text="%10s" % ("Iron"), relief=RIDGE, width = 10)
        l.grid(row=0, column=2, sticky=NSEW)
        Row1.append(l)
        l = Label(self.master, text="%10s" % ("Wood"), relief=RIDGE, width = 10)
        l.grid(row=0, column=3, sticky=NSEW)
        Row1.append(l)
        l = Label(self.master, text="%10s" % ("Stone"), relief=RIDGE, width = 10)
        l.grid(row=0, column=4, sticky=NSEW)
        Row1.append(l)
        self.table.append(Row1)
        for i in range(1, self.row):
            listRow = []
            for j in range(self.colum):
                lab = Label(self.master, text="", relief=RIDGE, width = 15)
                lab.grid(row=i, column=j, sticky=NSEW)
                listRow.append(lab)
            self.table.append(listRow)

    def updateTable(self, array):
        for i in range(1, self.row):
            for j in range(self.colum):
                self.table[i][j]['text'] = str(array[i-1][j])