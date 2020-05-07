#
# Sugar MELT
# Copyright(C) 2005 Daniel Pekelharing
#
# Melts all selected vertices down !

import objects
import options
import gui
import dansys

global speed
global start_frame

speed = 0.2
start_frame = 0

def set_speed(spin):
	global speed
	speed = spin.value()

def set_frame(spin):
	global start_frame
	start_frame = spin.value()

def build_gui():
	dialog = gui.dialog()
	dialog.set_title("Sugar MELT!")
	vbox = dialog.vbox()
	vbox.set_spacing(10)

	label1 = gui.label("Sugar MELT!")

	spin = gui.spinbutton(step = 0.1, max = 1000, page = 0.001)
	spin.set_value(speed)
	spin.set_digits(3)
	spin.connect("value-changed", set_speed)
	spin_label = gui.label("Speed: ")

	hbox = gui.hbox()
	hbox.pack_start(spin_label)
	hbox.pack_start(spin)

	spin2 = gui.spinbutton(step = 1, max = 100000, page = 10)
	spin2.set_value(start_frame)
	spin2.connect("value-changed", set_frame)
	spin2_label = gui.label("Start Frame: ")

	hbox2 = gui.hbox()
	hbox2.pack_start(spin2_label)
	hbox2.pack_start(spin2)

	vbox.pack_start(label1)
	vbox.pack_start(hbox)
	vbox.pack_start(hbox2)

	dialog.add_button("Cancel", 0)
	dialog.add_button("OK", 1)

	dialog.show_all()
	return dialog

verts = objects.selected_vertices()
dialog = build_gui()

if verts == None:
	gui.message(gui.MESSAGE_ERROR, "Hey you bummer! Please select some vertices!")
elif dialog.run() == 1:
	max = 0
	min = 100

	for vert in verts:
		vert.temp = True
		pos = vert.position()
		if pos[1] < min:
			min = pos[1]
		if pos[1] > max:
			max = pos[1]
		vert.set_frame(start_frame)

	avg = max + min
	avg = avg / 2
	cnt = 0
	done = False
	start_frame += 1

	while not done:
		for vert in verts:
			offset = vert.offset()
			pos = vert.position()
			if pos[1] > min + speed:
				vert.set_offset(offset[0], offset[1]-speed, offset[2])
				vert.temp = True
			elif pos[1] > min + (speed / 2):
				vert.set_offset(offset[0], offset[1]-(speed / 2), offset[2])
				vert.temp = True
			elif pos[1] > min + (speed / 10):
				vert.set_offset(offset[0], offset[1]-(speed / 10), offset[2])
				vert.temp = True
			elif pos[1] > min + (speed / 100):
				vert.set_offset(offset[0], offset[1]-(speed / 100), offset[2])
				vert.temp = True
			else:
				if vert.temp:
					cnt = cnt + 1
				vert.temp = False
			vert.set_frame(start_frame)
		if cnt == len(verts):
			done = True
		start_frame += 24

