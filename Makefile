MAKE = make
SUBDIRS = tools aica intro compr


all: subdirs printinfo

clean: clean_subdirs
	rm -f *~

subdirs: $(patsubst %, _dir_%, $(SUBDIRS))
 
$(patsubst %, _dir_%, $(SUBDIRS)):
	$(MAKE) -C $(patsubst _dir_%, %, $@)

clean_subdirs: $(patsubst %, _clean_dir_%, $(SUBDIRS))
 
$(patsubst %, _clean_dir_%, $(SUBDIRS)):
	$(MAKE) -C $(patsubst _clean_dir_%, %, $@) clean

printinfo:
	@wc -c aica/music.elf.bin
	@wc -c intro/intro.bin
	@wc -c compr/compr.bin
