SUBDIRS = CatPlugin/src DogPlugin/src Plug

.PHONY: all
all:
	@list='$(SUBDIRS)'; for subdir in $$list; do \
		echo "make in $$subdir";\
		cd $$subdir && $(MAKE);\
		cd -;\
	done

.PHONY: clean
clean:
	@list='$(SUBDIRS)'; for subdir in $$list; do \
		echo "Clean in $$subdir";\
		cd $$subdir && $(MAKE) clean;\
		cd -;\
	done
