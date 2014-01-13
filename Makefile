# Variable default definitions. Override them by exporting them in your shell.
CXX ?= g++
LINK ?= g++
OUTDIR ?= out
TESTJOBS ?=
GYPFLAGS ?= -Dlibrary=static_library -Dcomponent=static_library

# internal staff
ARCHES = ia32 x64 arm mipsel
DEFAULT_ARCHES = ia32 x64 arm
MODES = release debug optdebug
DEFAULT_MODES = release debug

GYPFILES = build/all.gyp

# Generates all combinations of ARCHES and MODES, e.g. "ia32.release".
BUILDS = $(foreach mode,$(MODES),$(addsuffix .$(mode),$(ARCHES)))

.PHONY: all clean dependencies native

# Target definitions. "all" is the default.
all: $(DEFAULT_MODES)

# Defines how to build a particular target (e.g. ia32.release).
$(BUILDS): $(OUTDIR)/Makefile.$$@
	@$(MAKE) -C "$(OUTDIR)" -f Makefile.$@ \
	    CXX="$(CXX)" LINK="$(LINK)" BUILDTYPE=Release \
	    builddir="$(shell pwd)/$(OUTDIR)/$@"
		

native: $(OUTDIR)/Makefile.native
	@$(MAKE) -C "$(OUTDIR)" -f Makefile.native \
	    CXX="$(CXX)" LINK="$(LINK)" BUILDTYPE=Release \
	    builddir="$(shell pwd)/$(OUTDIR)/$@"

# Clean targets. You can clean each architecture individually, or everything.
$(addsuffix .clean, $(ARCHES)):
	rm -f $(OUTDIR)/Makefile.$(basename $@)*
	rm -rf $(OUTDIR)/$(basename $@).release
	rm -rf $(OUTDIR)/$(basename $@).debug
	rm -rf $(OUTDIR)/$(basename $@).optdebug
	find $(OUTDIR) -regex '.*\(host\|target\)\.$(basename $@).*\.mk' -delete

native.clean:
	rm -f $(OUTDIR)/Makefile.native
	rm -rf $(OUTDIR)/native
	find $(OUTDIR) -regex '.*\(host\|target\)\.native\.mk' -delete

clean: $(addsuffix .clean, $(ARCHES)) native.clean

# GYP file generation targets.
OUT_MAKEFILES = $(addprefix $(OUTDIR)/Makefile.,$(BUILDS))
$(OUT_MAKEFILES): $(GYPFILES)
	GYP_GENERATORS=make \
	build/gyp/gyp --generator-output="$(OUTDIR)" build/all.gyp \
	              -I build/common.gypi --depth=. \
	              -S$(suffix $(basename $@))$(suffix $@) $(GYPFLAGS)

$(OUTDIR)/Makefile.native: $(GYPFILES)
	GYP_GENERATORS=make \
	build/gyp/gyp --generator-output="$(OUTDIR)" build/all.gyp \
	              -Ibuild/common.gypi --depth=. -S.native $(GYPFLAGS)

# Dependencies.
# Remember to keep these in sync with the DEPS file.
dependencies:
	svn checkout --force http://gyp.googlecode.com/svn/trunk build/gyp \
	    --revision 1806
