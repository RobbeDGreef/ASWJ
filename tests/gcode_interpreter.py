import tkinter as tk
import sys, os

class Interpreter(tk.Frame):
    def __init__(self, master=None):
        super().__init__(master)

        self.master = master
        self.pack(fill=tk.BOTH, expand=1)
        self.ui()
    
    def calc_coords(self, line):
        loc = line.find('X')
        if loc == -1:
            return None
        x = float(line[loc+1: line.find(' ', loc)])
        loc = line.find('Y')
        if loc == -1:
            return None
        y = float(line[loc+1: line.find(' ', loc)])
        
        return (x, y)


    def draw(self):
        prevpoint = (0,0)
        with open(sys.argv[1], 'r') as f:
            for line in f.readlines():
                if line.startswith('G1'):
                    calced = self.calc_coords(line)
                    
                    if calced != None and line.find('E') != -1:
                        self.canvas.create_line(prevpoint, calced)
                    
                    if calced != None:
                        prevpoint = calced

    def zoom(self, e, factor):
        
        self.canvas.scale(tk.ALL, e.x, e.y, factor, factor)

    def ui(self):
        self.canvas = tk.Canvas(self)
        self.canvas.pack(fill=tk.BOTH, expand=1)
        self.canvas.focus_set()

        self.canvas.bind('<ButtonPress-1>', lambda e: self.canvas.scan_mark(e.x, e.y))
        self.canvas.bind('<B1-Motion>', lambda e: self.canvas.scan_dragto(e.x, e.y, gain=1))
        self.canvas.bind('<4>', lambda e: self.zoom(e, 1.1))
        self.canvas.bind('<5>', lambda e: self.zoom(e, 0.9))

        self.draw()
        self.canvas.scale(tk.ALL, 0, 0, 3, 3)

root = tk.Tk()
app = Interpreter(master=root)
app.mainloop()