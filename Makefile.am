include $(top_srcdir)/Makefile.tool.am

EXTRA_DIST = docs/LT-manual.xml

#----------------------------------------------------------------------------
# Lepton-<platform>
#----------------------------------------------------------------------------

noinst_PROGRAMS  = Lepton-@VGCONF_ARCH_PRI@-@VGCONF_OS@
if VGCONF_HAVE_PLATFORM_SEC
noinst_PROGRAMS += Lepton-@VGCONF_ARCH_SEC@-@VGCONF_OS@
endif

Lepton_SOURCES_COMMON = LT_main.c

Lepton_@VGCONF_ARCH_PRI@_@VGCONF_OS@_SOURCES      = \
        $(Lepton_SOURCES_COMMON)
Lepton_@VGCONF_ARCH_PRI@_@VGCONF_OS@_CPPFLAGS     = \
        $(AM_CPPFLAGS_@VGCONF_PLATFORM_PRI_CAPS@)
Lepton_@VGCONF_ARCH_PRI@_@VGCONF_OS@_CFLAGS       = \
        $(AM_CFLAGS_@VGCONF_PLATFORM_PRI_CAPS@)
Lepton_@VGCONF_ARCH_PRI@_@VGCONF_OS@_DEPENDENCIES = \
        $(TOOL_DEPENDENCIES_@VGCONF_PLATFORM_PRI_CAPS@)
Lepton_@VGCONF_ARCH_PRI@_@VGCONF_OS@_LDADD        = \
        $(TOOL_LDADD_@VGCONF_PLATFORM_PRI_CAPS@)
Lepton_@VGCONF_ARCH_PRI@_@VGCONF_OS@_LDFLAGS      = \
        $(TOOL_LDFLAGS_@VGCONF_PLATFORM_PRI_CAPS@)
Lepton_@VGCONF_ARCH_PRI@_@VGCONF_OS@_LINK = \
        $(top_builddir)/coregrind/link_tool_exe_@VGCONF_OS@ \
        @VALT_LOAD_ADDRESS_PRI@ \
        $(LINK) \
        $(Lepton_@VGCONF_ARCH_PRI@_@VGCONF_OS@_CFLAGS) \
        $(Lepton_@VGCONF_ARCH_PRI@_@VGCONF_OS@_LDFLAGS)

if VGCONF_HAVE_PLATFORM_SEC
Lepton_@VGCONF_ARCH_SEC@_@VGCONF_OS@_SOURCES      = \
        $(Lepton_SOURCES_COMMON)
Lepton_@VGCONF_ARCH_SEC@_@VGCONF_OS@_CPPFLAGS     = \
        $(AM_CPPFLAGS_@VGCONF_PLATFORM_SEC_CAPS@)
Lepton_@VGCONF_ARCH_SEC@_@VGCONF_OS@_CFLAGS       = \
        $(AM_CFLAGS_@VGCONF_PLATFORM_SEC_CAPS@)
Lepton_@VGCONF_ARCH_SEC@_@VGCONF_OS@_DEPENDENCIES = \
        $(TOOL_DEPENDENCIES_@VGCONF_PLATFORM_SEC_CAPS@)
Lepton_@VGCONF_ARCH_SEC@_@VGCONF_OS@_LDADD        = \
        $(TOOL_LDADD_@VGCONF_PLATFORM_SEC_CAPS@)
Lepton_@VGCONF_ARCH_SEC@_@VGCONF_OS@_LDFLAGS      = \
        $(TOOL_LDFLAGS_@VGCONF_PLATFORM_SEC_CAPS@)
Lepton_@VGCONF_ARCH_SEC@_@VGCONF_OS@_LINK = \
        $(top_builddir)/coregrind/link_tool_exe_@VGCONF_OS@ \
        @VALT_LOAD_ADDRESS_SEC@ \
        $(LINK) \
        $(Lepton_@VGCONF_ARCH_SEC@_@VGCONF_OS@_CFLAGS) \
        $(Lepton_@VGCONF_ARCH_SEC@_@VGCONF_OS@_LDFLAGS)
endif

