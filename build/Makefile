# Compiler and Linker
CXX			:= g++

# Target binary program
TARGET		:= game

# The Directories, Source, Includes, Objects, Binary, and Resources
SRCDIR		:= ../src
INCDIR		:= ../src/include
BUILDDIR	:= obj
TARGETDIR	:= bin
RESDIR		:= resources
SRCEXT		:= cpp
DEPEXT		:= d
OBJEXT		:= o

# Flags, Libraries and Includes
CXXFLAGS	:= -Wall \
			   -Wextra \
			   -Wno-long-long \
			   -Wno-variadic-macros \
			   -Wzero-as-null-pointer-constant \
			   -std=c++17 \
			   -g -O0

LDLIBS		:= -lsfml-graphics \
			   -lsfml-audio \
			   -lsfml-window \
			   -lsfml-system \
			   -llua \
			   -ltinyxml2 \
			   -lpthread \
			   -lboost_filesystem

INC			:= -I$(INCDIR) -I/usr/local/include
INCDEP		:= -I$(INCDIR)

#--==|| Don't edit below this line ||==--
SOURCES		:= $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS		:= $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.$(OBJEXT)))

# Default Make
all: resources $(TARGET)

# Copy resources from the resources directory to the target directory
resources: directories
	@cp -r $(RESDIR)/ $(TARGETDIR)/

# Make the directories
directories:
	@mkdir -p $(TARGETDIR)
	@mkdir -p $(BUILDDIR)

# Clean only objects
clean:
	@$(RM) -rf $(BUILDDIR)

# Full clean, objects and binaries
cleaner: clean
	@$(RM) -rf $(TARGETDIR)

# Pull in dependency info for *existing* .o files
-include $(OBJECTS:.$(OBJEXT)=.$(DEPEXT))

# Link
$(TARGET): $(OBJECTS)
	$(CXX) -o $(TARGETDIR)/$(TARGET) $^ $(LDLIBS)

# Compile
$(BUILDDIR)/%.$(OBJEXT): $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INC) -c -o $@ $<
	@$(CXX) $(CXXFLAGS) $(INCDEP) -MM $(SRCDIR)/$*.$(SRCEXT) > $(BUILDDIR)/$*.$(DEPEXT)
	@cp -f $(BUILDDIR)/$*.$(DEPEXT) $(BUILDDIR)/$*.$(DEPEXT).tmp
	@sed -e 's|.*:|$(BUILDDIR)/$*.$(OBJEXT):|' < $(BUILDDIR)/$*.$(DEPEXT).tmp > $(BUILDDIR)/$*.$(DEPEXT)
	@sed -e 's/.*://' -e 's/\\$$//' < $(BUILDDIR)/$*.$(DEPEXT).tmp | fmt -1 | sed -e 's/^ *//' -e 's/$$/:/' >> $(BUILDDIR)/$*.$(DEPEXT)
	@rm -f $(BUILDDIR)/$*.$(DEPEXT).tmp

# Non-File Targets
.PHONY: all remake clean cleaner resources
