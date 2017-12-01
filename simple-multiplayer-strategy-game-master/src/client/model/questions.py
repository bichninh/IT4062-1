from tkinter import *
from _pickle import loads, dumps
from message import message

#Gui question
class questions(object):
    def __init__ (self, client, teamId, opCode, mineId, resource, master, content, choices):
        self.client =  client
        self.teamId = teamId
        self.opCode = opCode
        self.mineId = mineId
        self.resource = resource
        self.master = master

        # setup center and size

        self.master.geometry("670x180")
        self.master.update_idletasks()
        w = self.master.winfo_screenwidth()
        h = self.master.winfo_screenheight()
        size = tuple(int(_) for _ in self.master.geometry().split('+')[0].split('x'))
        x = w/2 - size[0]/2
        y = h/2 - size[1]/2
        self.master.geometry("%dx%d+%d+%d" % (size + (x, y)))
        
        # question :
        self.content = content
        self.choices = choices
        self.answer = StringVar()
        
        # Gui question
        Label(self.master, text = self.content, justify = LEFT,padx=20).pack()
        i = 1
        for val in self.choices:
            Radiobutton(self.master, text=val,padx = 20, variable = self.answer, command=None ,value=i).pack(anchor=W)
            i += 1
        button_ok = Button(self.master, text='Ok', command=lambda: self.sendAnswer(), width = 10).pack()

    # send answer to server
    def sendAnswer(self):
        answer = self.answer.get()
        if(answer == ""):
            messagebox.showwarning("Warning", "You must choice one of this!")
        else:
            payload = self.mineId, self.resource, answer
            #call message setup struct mes
            mes = message(self.opCode, self.teamId, payload)
            try:
                self.client.send(dumps(mes))
                mesrcv = loads(self.client.recv(2048))
                if mesrcv.opCode == (self.opCode + 1):
                    if mesrcv.teamId == True:
                    	# correct
                        messagebox.showinfo("Notify", "Answer correct!")
                    else:
                    	# incorrect id team
                        messagebox.showwarning("Notify", mesrcv.payLoad)
                else:
                	#incorrect opcode or error server
                    messagebox.showwarning("Warning", "Server send Error! Please try again!")
            except:
            	# can not connect server
                messagebox.showwarning("Warning", "Cannot connect to server!")
        self.master.destroy()