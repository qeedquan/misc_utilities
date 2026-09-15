#!/usr/bin/env python3

import tkinter as tk
from tkinter import ttk

class ScrollableFrame(ttk.Frame):
    def __init__(self, container, *args, **kwargs):
        super().__init__(container, *args, **kwargs)
        
        # 1. Create a canvas
        self.canvas = tk.Canvas(self, borderwidth=0, highlightthickness=0)
        
        # 2. Create the scrollbar and link it to the canvas y-view
        self.scrollbar = ttk.Scrollbar(self, orient="vertical", command=self.canvas.yview)
        self.canvas.configure(yscrollcommand=self.scrollbar.set)
        
        # 3. Create the inner frame that will hold the content
        self.scrollable_frame = ttk.Frame(self.canvas)
        
        # 4. Bind the frame resizing event to update the canvas scrolling region
        self.scrollable_frame.bind(
            "<Configure>",
            lambda e: self.canvas.configure(scrollregion=self.canvas.bbox("all"))
        )
        
        # 5. Pack the inner frame into a canvas window
        self.canvas_window = self.canvas.create_window((0, 0), window=self.scrollable_frame, anchor="nw")
        
        # 6. Bind canvas resize event to make the inner frame expand to fill full width
        self.canvas.bind('<Configure>', self._on_canvas_configure)
        
        # 7. Layout components inside the wrapper frame
        self.canvas.pack(side="left", fill="both", expand=True)
        self.scrollbar.pack(side="right", fill="y")
        
    def _on_canvas_configure(self, event):
        # Match the inner frame width to the canvas width
        self.canvas.itemconfig(self.canvas_window, width=event.width)

# --- Application Example Usage ---
root = tk.Tk()
root.title("Scrollable Frame Example")
root.geometry("800x400")

# Instantiate the scrollable frame wrapper
frame_container = ScrollableFrame(root)
frame_container.pack(fill="both", expand=True, padx=10, pady=10)

def on_enter(event):
    print(str(event))

# Add elements directly into 'frame_container.scrollable_frame'
for i in range(50):
    lbl = ttk.Label(frame_container.scrollable_frame, text=f"Item Label Row {i+1}")
    lbl.bind("<Enter>", on_enter)
    lbl.pack(fill="x", pady=5)

root.mainloop()
