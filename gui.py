import customtkinter as ctk
import tkinter as tk
import sys
import json
import subprocess

from pathlib import Path

c1 = Path(__file__).parent / "gui_config.json"
c2 = Path(__file__).parent / "mandelbrot"

if c1.exists():
  if not c1.is_file():
    print("gui_config.json does not exist")
    sys.exit()
else:
  print("gui_config.json does not exist")
  sys.exit()

if c2.exists():
  if not c2.is_file():
    print("main program does not exist or is not compiled")
    sys.exit()
else:
  print("main program does not exist or is not compiled")
  sys.exit()


with open("gui_config.json", "r", encoding="utf-8") as file:
  config = json.load(file)


presets = []
for item in config:
  presets.append(item["name"])
  presets.append(str(item["size"]))
  presets.append(str(item["iter"]))
presets_count = int(len(presets)/3)

bg_color = "#2b2b2b"
size_x = 800
size_y = 575


ctk.set_appearance_mode("dark")
ctk.set_default_color_theme("blue")

app = ctk.CTk(fg_color=bg_color)

app.title("Mandelbrot_Benchmark")
app.geometry(str(size_x) + "x" + str(size_y))
app.resizable(False, False)

canvas = tk.Canvas(
  app,
  width=size_x,
  height=size_y,
  bg=bg_color,
  highlightthickness=0
)

canvas.place(x=0, y=0)



def pokaz_tekst():
    tekst = entry.get()
    label.configure(text=tekst)


def ustaw_wszystko():
    label.configure(text="Ustawiono wszystko!")

    entry.delete(0, "end")
    entry.insert(0, "Tekst ustawiony przez kod")

    switch.select()

    slider.set(80)


def zmiana_suwaka(wartosc):
    label_slider.configure(
        text=f"Suwak: {int(wartosc)}"
    )

def click(button_id):
  print("button " + str(button_id) + " pressed")



def change_slider(value, entry):
  value = int(round(float(value)))
  entry.delete(0, "end")
  entry.insert(0, str(value))


def change_entry(entry, slider):
  try:
    value = int(entry.get())
  except ValueError:
    return

  min_value = slider.cget("from_")
  max_value = slider.cget("to")

  if value < min_value:
    slider.set(min_value)
  elif value > max_value:
    slider.set(max_value)
  else:
    slider.set(value)





output_lines = []
scores = []


def start_test():
  status.configure(text="Working...")
  
  output_lines.clear()
  scores.clear()

  cores = 16
  size = int(size_entry.get())
  max_iter = int(iter_entry.get())
  repeat = int(num_entry.get())
  save_preview = 1
  preview_size = 100

  for i in range(repeat):
    command = [
      str(c2),
      str(cores),
      str(size),
      str(max_iter),
      str(save_preview),
      str(preview_size)
    ]

    result = subprocess.run(
      command,
      capture_output=True,
      text=True
    )

    test_output = result.stdout.splitlines()

    output_lines.extend(test_output)

    for line in test_output:
      if line.startswith("Score:"):
        score = float(line.split(":")[1].strip())
        scores.append(score)
        break

  average_score = sum(scores) / len(scores)

  print("Scores:", scores)
  print("Average:", average_score)
  print("Output:", output_lines)

  status.configure(
    text=f"Done!\nYour CPU scored {average_score:.3f} points"
  )












label = ctk.CTkLabel(
  app,
  text="Mandelbrot Benchmark",
  font=("Arial", 30)
)

label.place(
  x=10,
  y=10
)

buttons = []

canvas.create_rectangle(
  20, 60,
  310, 70+(50*presets_count),
  outline="white",
  width=3
)

for i in range(presets_count):
  button = ctk.CTkButton(
    app,
    text=presets[i*3],
    font=("Arial", 15),
    width=270,
    height=40,
    command=lambda i=i: click(i)
  )

  button.place(
    x=30,
    y=70+50*i
  )
  buttons.append(button)



size_label = ctk.CTkLabel(
  app,
  text="Size",
  font=("Arial", 15)
)

size_label.place(
  x=25,
  y=90+50*(presets_count)
)

size_entry = ctk.CTkEntry(
  app,
  width=60,
  height=40,
  placeholder_text="Size"
)

size_entry.place(
  x=19,
  y=115+50*(presets_count)
)

size_entry.bind(
  "<KeyRelease>",
  lambda event: change_entry(size_entry, size_slider)
)

size_slider = ctk.CTkSlider(
  app,
  from_=100,
  to=1200,
  number_of_steps=22,
  width=220,
  command=lambda value: change_slider(value, size_entry)
)

size_slider.place(
  x=90,
  y=127+50*(presets_count)
)

size_slider.set(600)
size_entry.insert(0, "600")


iter_label = ctk.CTkLabel(
  app,
  text="Max iter.",
  font=("Arial", 15)
)

iter_label.place(
  x=25,
  y=165+50*(presets_count)
)

iter_entry = ctk.CTkEntry(
  app,
  width=60,
  height=40,
  placeholder_text="Max iter."
)

iter_entry.place(
  x=19,
  y=190+50*(presets_count)
)

iter_entry.bind(
  "<KeyRelease>",
  lambda event: change_entry(iter_entry, iter_slider)
)

iter_slider = ctk.CTkSlider(
  app,
  from_=100,
  to=1200,
  number_of_steps=22,
  width=220,
  command=lambda value: change_slider(value, iter_entry)
)

iter_slider.place(
  x=90,
  y=202+50*(presets_count)
)

iter_slider.set(450)
iter_entry.insert(0, "450")


num_label = ctk.CTkLabel(
  app,
  text="Repeat",
  font=("Arial", 15)
)

num_label.place(
  x=25,
  y=240+50*(presets_count)
)

num_entry = ctk.CTkEntry(
  app,
  width=60,
  height=40,
  placeholder_text="Repeat"
)

num_entry.place(
  x=19,
  y=265+50*(presets_count)
)

num_entry.bind(
  "<KeyRelease>",
  lambda event: change_entry(num_entry, num_slider)
)

num_slider = ctk.CTkSlider(
  app,
  from_=1,
  to=10,
  number_of_steps=9,
  width=220,
  command=lambda value: change_slider(value, num_entry)
)

num_slider.place(
  x=90,
  y=277+50*(presets_count)
)

num_slider.set(1)
num_entry.insert(0, "1")





switch = ctk.CTkSwitch(
  app,
  text="Opcja"
)







preview = ctk.CTkFrame(
  app,
  width=300,
  height=300,
  fg_color="#404040",
  corner_radius=0
)

preview.place(x=360, y=60)

preview_label = ctk.CTkLabel(
  app,
  text="[T1]",
  font=("Arial", 15),
  bg_color = "#404040"
)

preview_label.place(
  x=370,
  y=65
)

status = ctk.CTkLabel(
  app,
  text="Status: Not testing",
  font=("Arial", 22)
)

status.place(
  x=360,
  y=370
)


start_test = ctk.CTkButton(
  app,
  text="Start test",
  font=("Arial", 20),
  width=425,
  height=75,
  command=start_test
)

start_test.place(
  x=360,
  y=235+50*(presets_count)
)




app.mainloop()
