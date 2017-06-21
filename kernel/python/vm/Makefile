python-files-$(CONFIG_PYTHON) += cpu.o bytearray.o class.o codeobj.o dict.o float.o
python-files-$(CONFIG_PYTHON) += frame.o func.o global.o heap.o img.o int.o
python-files-$(CONFIG_PYTHON) += interp.o list.o mem.o module.o object.o pm.o
python-files-$(CONFIG_PYTHON) += seglist.o seq.o sli.o strobj.o thread.o tuple.o 
python-files-$(CONFIG_PYTHON) += platform.o pylib.o pylib-img.o init.o
python-files-$(CONFIG_PYTHON) += eeprom.o

clean-files += pylib.c pylib-img.c

obj-$(CONFIG_PYTHON) += $(python-files-y) $(python-files-m)

