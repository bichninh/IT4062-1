from tkinter import *
from _pickle import loads, dumps
from message import message
from model.mainPlay import mainPlay
from tkinter import messagebox

####  Gui join team, start!
class startFrame():

    def __init__(self, client, master, list_team):
        self.client = client
        self.master = master
        self.list_team = list_team
        self.master.title("Small-Game")
        ## in center and size
        self.master.geometry("430x170")
        self.master.update_idletasks()
        w = self.master.winfo_screenwidth()
        h = self.master.winfo_screenheight()
        size = tuple(int(_) for _ in self.master.geometry().split('+')[0].split('x'))
        x = w/2 - size[0]/2
        y = h/2 - size[1]/2
        self.master.geometry("%dx%d+%d+%d" % (size + (x, y)))
        
        # table , info team
        self.table_team = Frame(self.master)
        self.lable1 = Label(self.master, text = "Team Id:", relief= RIDGE, width = 10)
        self.teamId = Entry(self.master, width = 15)
        self.btjoin = Button(self.master, text = "Play", command = lambda: self.playGame(self.teamId.get()), width = 5)
        self.btexit = Button(self.master, text = "Exit", command = lambda: self.Exit(), width = 5)

        #Layout
        self.table_team.place(x= 0, y = 0)
        self.lable1.place(x = 5, y = 100)
        self.teamId.place(x = 80,y = 100)
        self.btjoin.place(x = 350, y = 130)
        self.btexit.place(x = 5, y = 130)
        self.updateTbteam()

    # display
    def mainLoop(self):
        self.master.mainloop()

    #update table when data of Team change
    def updateTbteam(self):
        l = Label(self.table_team, text="%10s" % ("Team Id"), relief=RIDGE, width = 10)
        l.grid(row=0, column=0, sticky=NSEW)
        l = Label(self.table_team, text="%10s" % ("Gold"), relief=RIDGE, width = 10)
        l.grid(row=0, column=1, sticky=NSEW)
        l = Label(self.table_team, text="%10s" % ("Iron"), relief=RIDGE, width = 10)
        l.grid(row=0, column=2, sticky=NSEW)
        l = Label(self.table_team, text="%10s" % ("Wood"), relief=RIDGE, width = 10)
        l.grid(row=0, column=3, sticky=NSEW)
        l = Label(self.table_team, text="%10s" % ("Stone"), relief=RIDGE, width = 10)
        l.grid(row=0, column=4, sticky=NSEW)
        for i,team in enumerate(self.list_team):
            l = Label(self.table_team, text="%10s" % (team[0]), relief=RIDGE, width = 10)
            l.grid(row=i+1, column=0, sticky=NSEW)
            l = Label(self.table_team, text="%10s" % (team[1]), relief=RIDGE, width = 10)
            l.grid(row=i+1, column=1, sticky=NSEW)
            l = Label(self.table_team, text="%10s" % (team[2]), relief=RIDGE, width = 10)
            l.grid(row=i+1, column=2, sticky=NSEW)
            l = Label(self.table_team, text="%10s" % (team[3]), relief=RIDGE, width = 10)
            l.grid(row=i+1, column=3, sticky=NSEW)
            l = Label(self.table_team, text="%10s" % (team[4]), relief=RIDGE, width = 10)
            l.grid(row=i+1, column=4, sticky=NSEW)

    # join team and start Play
    def playGame(self, teamId):
        try:
            teamId = int(teamId)
        except:
            teamId = 0
        mes = message(0x0101, teamId, None)
        try:
            self.client.send(dumps(mes))
            mesrcv = loads(self.client.recv(2048))
            if mesrcv.opCode == 0x0102:
                if mesrcv.teamId == True:
                    newFrame = Toplevel()
                    self.hide()
                    # call mainPlay and start play...
                    mainGame = mainPlay(self.client, teamId, newFrame, self.master)
                else:
                    messagebox.showwarning("Warning", mesrcv.payLoad)
            else:
                messagebox.showwarning("Warning", "Server send Error! Please try again!")
        except:
            messagebox.showwarning("Warning", "Cannot connect to server!")

    # close this Gui
    def quit(self):
        self.master.destroy()

    # hide this Gui
    def hide(self):
        self.master.withdraw()
    # close this Gui
    def Exit(self):
        self.quit()
