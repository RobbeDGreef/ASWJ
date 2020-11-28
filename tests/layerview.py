from tkinter import Tk, Canvas, Frame, BOTH, Scale, HORIZONTAL, X, TOP, ALL, LEFT, Label
import os

TESTDIR = os.path.join(os.path.dirname(os.path.realpath(__file__)), "debug")

class LineView(Frame):
    def __init__(self):
        super().__init__()

        self.zoom = 1.0
        self.ui()

    def setzoom(self, e):
        self.zoom = float(e)

    def draw_canvas(self, layer):
        self.canvas.delete(ALL)
        with open(os.path.join(TESTDIR, str(layer) + ".txt"), 'r') as f:
            for line in f.readlines():
                try:
                    locations = [int(float(x) * self.zoom) for x in line.split(' ')]
                    self.canvas.create_line(locations[0], locations[1], locations[3], locations[4])
                    self.canvas.create_oval(locations[0] - 2, locations[1] -2, locations[0] +2, locations[1] +2)
                    self.canvas.create_oval(locations[3] - 2, locations[4] -2, locations[3] +2, locations[4] +2)
                except ValueError as e:
                    print("error ", e)
    def ui(self):
        self.master.title("Layer view")
        self.pack(fill=BOTH, expand=1)

        Label(self, text="Zoom: ").pack(side=TOP)
        slider = Scale(self, from_=-10, to=10, resolution=0.1, orient=HORIZONTAL, command=lambda e: self.setzoom(e))
        slider.pack(fill=X, expand=1, side=TOP)

        Label(self, text="Layer: ").pack(side=TOP)
        slider = Scale(self, from_=0, to=len(os.listdir(TESTDIR)), resolution=1, orient=HORIZONTAL, command=lambda e: self.draw_canvas(e))
        slider.pack(fill=X, expand=1, side=TOP)

        self.canvas = Canvas(self)
        self.draw_canvas(0)
        self.canvas.pack(fill=BOTH, expand=1)

root = Tk()
root.geometry("280x300")
frame = LineView()
root.mainloop()