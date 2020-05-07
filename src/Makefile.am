bin_PROGRAMS = kudu
kudu_SOURCES = kudu.c kudu.h \
		globals.h \
		camera.c camera.h \
		joints.c joints.h \
		bones.c bones.h \
		graphics.c graphics.h \
		materials.c materials.h \
		vertices.c vertices.h \
		wingedge.c wingedge.h \
		mathematics.c mathematics.h \
		utils.c utils.h \
		image.c image.h \
		options.c options.h \
		stack.c stack.h \
		selection.c selection.h \
		rwxobject.c rwxobject.h \
		objobject.c objobject.h \
		gui.c gui.h \
		gui_vertex.c gui_vertex.h \
		gui_bone.c gui_bone.h \
		menus.c menus.h \
		texture.c texture.h \
		tga_image.c tga_image.h \
		rgb_image.c rgb_image.h \
		object.c object.h \
		kerror.c kerror.h \
		kudu_menus.c kudu_menus.h \
		font.c font.h defaultfont.h \
		bones_edit.c bones_edit.h \
		gui_materials.c gui_materials.h \
		about.c about.h \
		skin_edit.c skin_edit.h \
		gui_object.c gui_object.h \
		frames.c frames.h \
		sequence.c sequence.h \
		toolbar.c toolbar.h \
		joints_edit.c joints_edit.h \
		uiplugin.c uiplugin.h \
		gui_options.c gui_options.h

AM_CPPFLAGS = -DKUDU_DATADIR=\"$(pkgdatadir)\"
LIBS =  @LIBS@ @GSTUFF_LIBS@ @GTKGLEXT_LIBS@
INCLUDES = @GSTUFF_CFLAGS@ @GTKGLEXT_CFLAGS@

if SUPPORT_PYTHON
kudu_SOURCES += scripts.c scripts.h \
		gui_scripts.c gui_scripts.h \
		objectsmodule.c objectsmodule.h \
		guimodule.c guimodule.h 
LIBS += @PYTHON_EXTRA_LIBS@ @PYTHON_LDFLAGS@
INCLUDES += @PYTHON_CPPFLAGS@
endif


