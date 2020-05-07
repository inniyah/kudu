#!KUDU

"""
Name: Axle Spinner
Author: Daniel Pekelharing
Type: General
"""

#
# Axle-Spinner
#
# Copyright(C) 2005 Daniel Pekelharing
#
# Spins the selected bone at a configurable speed
# for a configurable amount of frames in the
# specified direction.

import objects
import gui

global speed, start, end, clockwise, clear

speed = 100
start = objects.current_frame()
end = start + 100
clockwise = True
clear = False

def set_speed(spin):
	global speed
	speed = spin.value()

def set_start(spin):
	global start
	start = spin.value()

def set_end(spin):
	global end
	end = spin.value()

def set_direction(radio, dir):
	global clockwise
	clockwise = dir

def set_clear(check):
	global clear
	clear = check.active()

def build_gui():

	dialog = gui.dialog()
	dialog.set_title("Axle-Spinner")
	vbox = dialog.vbox()

	speed_l = gui.label("Speed (RPM): ")
	speedb = gui.spinbutton(min = 1, max = 10000)
	speedb.connect("value-changed", set_speed)
	speedb.set_value(speed)
	speed_box = gui.hbox()
	speed_box.pack_start(speed_l)
	speed_box.pack_start(speedb)

	start_l = gui.label("Start at frame: ")
	startb = gui.spinbutton(min = 0, max = 1000000)
	startb.connect("value-changed", set_start)
	startb.set_value(start)
	start_box = gui.hbox()
	start_box.pack_start(start_l)
	start_box.pack_start(startb)

	end_l = gui.label("End at frame: ")
	endb = gui.spinbutton(min = 0, max = 1000000)
	endb.connect("value-changed", set_end)
	endb.set_value(end)
	end_box = gui.hbox()
	end_box.pack_start(end_l)
	end_box.pack_start(endb)

	clockwise = gui.radiobox("Clockwise")
	cclockwise = gui.radiobox("Counter Clockwise", radio_pair = clockwise)
	clockwise.connect("clicked", set_direction, True)
	cclockwise.connect("clicked", set_direction, False)

	clear = gui.checkbox("Clear this range?")
	clear.connect("clicked", set_clear)

	vbox.pack_start(speed_box)
	vbox.pack_start(start_box)
	vbox.pack_start(end_box)
	vbox.pack_start(clear)
	vbox.pack_start(clockwise)
	vbox.pack_start(cclockwise)

	dialog.show_all()
	dialog.add_button("OK", True)
	dialog.add_button("Cancel", False)

	return dialog


dialog = build_gui()

if dialog.run():
#	bone = objects.selected_bone()
	bones = objects.selected_bones()

	if clear:
		for bone in bones:
			keys = bone.key_frames()
			if keys != None:
				for key in keys:
					if key >= start and key <= end:
						bone.clear_frame(key)

	frame = start
	rps = (speed / 60)
	rpf = (rps / 24)

	frame_step = (0.5 / rpf)
	size_step = (179 / frame_step)
	size_step *= frame_step

	while frame < end:
		for bone in bones:
			bone.set_frame(frame)
			ang = bone.rAngle()
			if clockwise:
				ang += size_step
			else:
				ang -= size_step
			bone.set_rAngle(ang)

		frame += frame_step




