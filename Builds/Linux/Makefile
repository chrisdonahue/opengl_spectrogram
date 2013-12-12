# Automatically generated makefile, created by the Introjucer
# Don't edit this file! Your changes will be overwritten when you re-save the Introjucer project!

# (this disables dependency generation if multiple architectures are set)
DEPFLAGS := $(if $(word 2, $(TARGET_ARCH)), , -MMD)

ifndef CONFIG
  CONFIG=Debug
endif

ifeq ($(CONFIG),Debug)
  BINDIR := build
  LIBDIR := build
  OBJDIR := build/intermediate/Debug
  OUTDIR := build

  ifeq ($(TARGET_ARCH),)
    TARGET_ARCH := -march=native
  endif

  CPPFLAGS := $(DEPFLAGS) -D "LINUX=1" -D "DEBUG=1" -D "_DEBUG=1" -D "JUCER_LINUX_MAKE_7346DA2A=1" -I /usr/include -I /usr/include/freetype2 -I ../../JuceLibraryCode
  CFLAGS += $(CPPFLAGS) $(TARGET_ARCH) -g -ggdb -O0
  CXXFLAGS += $(CFLAGS)
  LDFLAGS += $(TARGET_ARCH) -L$(BINDIR) -L$(LIBDIR) -L/usr/X11R6/lib/ 
  LDDEPS :=
  RESFLAGS :=  -D "LINUX=1" -D "DEBUG=1" -D "_DEBUG=1" -D "JUCER_LINUX_MAKE_7346DA2A=1" -I /usr/include -I /usr/include/freetype2 -I ../../JuceLibraryCode
  TARGET := opengl_spectrogram
  BLDCMD = $(CXX) -o $(OUTDIR)/$(TARGET) $(OBJECTS) $(LDFLAGS) $(RESOURCES) $(TARGET_ARCH)
endif

ifeq ($(CONFIG),Release)
  BINDIR := build
  LIBDIR := build
  OBJDIR := build/intermediate/Release
  OUTDIR := build

  ifeq ($(TARGET_ARCH),)
    TARGET_ARCH := -march=native
  endif

  CPPFLAGS := $(DEPFLAGS) -D "LINUX=1" -D "NDEBUG=1" -D "JUCER_LINUX_MAKE_7346DA2A=1" -I /usr/include -I /usr/include/freetype2 -I ../../JuceLibraryCode
  CFLAGS += $(CPPFLAGS) $(TARGET_ARCH) -Os
  CXXFLAGS += $(CFLAGS)
  LDFLAGS += $(TARGET_ARCH) -L$(BINDIR) -L$(LIBDIR) -fvisibility=hidden -L/usr/X11R6/lib/ 
  LDDEPS :=
  RESFLAGS :=  -D "LINUX=1" -D "NDEBUG=1" -D "JUCER_LINUX_MAKE_7346DA2A=1" -I /usr/include -I /usr/include/freetype2 -I ../../JuceLibraryCode
  TARGET := opengl_spectrogram
  BLDCMD = $(CXX) -o $(OUTDIR)/$(TARGET) $(OBJECTS) $(LDFLAGS) $(RESOURCES) $(TARGET_ARCH)
endif

OBJECTS := \
  $(OBJDIR)/audio_util_af41dd2b.o \
  $(OBJDIR)/kiss_fft_1cb00a17.o \
  $(OBJDIR)/kiss_fftr_1769ff11.o \
  $(OBJDIR)/graph_f53d38f8.o \
  $(OBJDIR)/shader_utils_7cbb8177.o \
  $(OBJDIR)/BinaryData_ce4232d4.o \

.PHONY: clean

$(OUTDIR)/$(TARGET): $(OBJECTS) $(LDDEPS) $(RESOURCES)
	@echo Linking opengl_spectrogram
	-@mkdir -p $(BINDIR)
	-@mkdir -p $(LIBDIR)
	-@mkdir -p $(OUTDIR)
	@$(BLDCMD)

clean:
	@echo Cleaning opengl_spectrogram
	-@rm -f $(OUTDIR)/$(TARGET)
	-@rm -rf $(OBJDIR)/*
	-@rm -rf $(OBJDIR)

strip:
	@echo Stripping opengl_spectrogram
	-@strip --strip-unneeded $(OUTDIR)/$(TARGET)

$(OBJDIR)/audio_util_af41dd2b.o: ../../src/audio_util.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling audio_util.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/kiss_fft_1cb00a17.o: ../../src/dependencies/kiss_fft130/kiss_fft.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling kiss_fft.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/kiss_fftr_1769ff11.o: ../../src/dependencies/kiss_fft130/tools/kiss_fftr.c
	-@mkdir -p $(OBJDIR)
	@echo "Compiling kiss_fftr.c"
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/graph_f53d38f8.o: ../../src/graph.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling graph.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/shader_utils_7cbb8177.o: ../../src/shader_utils.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling shader_utils.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/BinaryData_ce4232d4.o: ../../JuceLibraryCode/BinaryData.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling BinaryData.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

-include $(OBJECTS:%.o=%.d)
