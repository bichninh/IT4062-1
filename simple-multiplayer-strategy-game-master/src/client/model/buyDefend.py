from tkinter import *
from _pickle import loads, dumps
from message import message
from tkinter import messagebox

class buyDefend(object):
    def __init__ (self, client, teamId, castleId, master):
        ITEM = {
            'defence': {
                'fence': {
                    'name': 'Fence',
                    'value': 200,
                    'resources': {
                        'wood': 200,
                        'stone': 50,
                    }
                },
                'wood': {
                    'name': 'Wood wall',
                    'value': 1000,
                    'resources': {
                        'wood': 1000,
                        'stone': 100,
                        'iron': 100,
                    },
                },
                'stone': {
                    'name': 'Stone wall',
                    'value': 3000,
                    'resources': {
                        'wood': 200,
                        'stone': 1000,
                        'iron': 200,
                    },
                },
                'legend': {
                    'name': 'Legend wall',
                    'value': 8000,
                    'resources': {
                        'wood': 1000,
                        'stone': 2000,
                        'iron': 1000,
                    },
                },
            },
        }

        # setup center and size 
        self.master = master
        self.master.geometry("670x175")
        self.master.update_idletasks()
        w = self.master.winfo_screenwidth()
        h = self.master.winfo_screenheight()
        size = tuple(int(_) for _ in self.master.geometry().split('+')[0].split('x'))
        x = w/2 - size[0]/2
        y = h/2 - size[1]/2
        self.master.geometry("%dx%d+%d+%d" % (size + (x, y)))

        # id, choices
        self.client = client
        self.teamId = teamId
        self.castleId = castleId
        self.checked = StringVar()
        self.choices = [
            (ITEM['defence']['fence'],"fence"),
            (ITEM['defence']['wood'],"wood"),
            (ITEM['defence']['stone'],"stone"),
            (ITEM['defence']['legend'],"legend"),
        ]

        # gui choices
        Label(self.master, text = "Sellect Item: ", justify = LEFT,padx=20).pack()
        for txt, val in self.choices:
            Radiobutton(self.master, text=txt,padx = 20, variable=self.checked, command=None,value=val).pack(anchor=W)
        button_ok = Button(self.master, text='Ok', command=lambda: self.buyDefend(), width = 10).pack()

    # buy defend
    def buyDefend(self):
        check = self.checked.get()
        if check == "":
            messagebox.showwarning("Warning", "You must choice one of this!")
        else:
            payload = self.castleId, check
            #call message setup struct of mes
            mes = message(0x0601, self.teamId, payload)
            try:
                self.client.send(dumps(mes))
                mesrcv = loads(self.client.recv(2048))
                # check data...
                if mesrcv.opCode == 0x0602:
                    if mesrcv.teamId == True:
                        messagebox.showinfo("Notify", "Buy success!")
                    else:
                        messagebox.showwarning("Warning", mesrcv.payLoad)
                else:
                    messagebox.showwarning("Warning", "Server send Error! Please try again!")
            except:
                messagebox.showwarning("Warning", "Cannot connect to server!")
        self.master.destroy()