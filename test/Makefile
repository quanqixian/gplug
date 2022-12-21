SUBDIRS = CatPlugin/src DogPlugin/src Plug

.PHONY: all
all:
	@list='$(SUBDIRS)'; for subdir in $$list; do \
		echo "make in $$subdir";\
		pushd $$subdir && $(MAKE);\
		popd;\
	done

.PHONY: clean
clean:
	@list='$(SUBDIRS)'; for subdir in $$list; do \
		echo "Clean in $$subdir";\
		pushd $$subdir && $(MAKE) clean;\
		popd;\
	done
