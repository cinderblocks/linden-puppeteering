# -*- cmake -*-
#
# Compared to other libraries, compiling this one is a mess.  The
# reason is that we have several source files that have two different
# sets of behaviour, depending on whether they're intended to be part
# of the viewer or the map server.
#
# Unfortunately, the affected code is a rat's nest of #ifdefs, so it's
# easier to play compilation tricks than to actually fix the problem.

project(llwindow)

include(00-Common)
include(DirectX)
include(LLCommon)
include(LLImage)
include(LLMath)
include(LLRender)
include(LLVFS)
include(LLWindow)
include(LLXML)
include(Mozlib)
include(UI)

include_directories(
    ${LLCOMMON_INCLUDE_DIRS}
    ${LLIMAGE_INCLUDE_DIRS}
    ${LLMATH_INCLUDE_DIRS}
    ${LLRENDER_INCLUDE_DIRS}
    ${LLVFS_INCLUDE_DIRS}
    ${LLWINDOW_INCLUDE_DIRS}
    ${LLXML_INCLUDE_DIRS}
    )

set(llwindow_SOURCE_FILES
    llkeyboard.cpp
    llwindowheadless.cpp
    )

set(llwindows_HEADER_FILES
    CMakeLists.txt

    llkeyboard.h
    llwindowheadless.h
    )

set(viewer_SOURCE_FILES
    llwindow.cpp
    llmousehandler.cpp
    )

set(viewer_HEADER_FILES
    llwindow.h
    llpreeditor.h
    llmousehandler.h
    )

# Libraries on which this library depends, needed for Linux builds
# Sort by high-level to low-level
set(llwindow_LINK_LIBRARIES
    ${UI_LIBRARIES}     # for GTK
    ${SDL_LIBRARY}
    )

if (DARWIN)
  list(APPEND llwindow_SOURCE_FILES
    llkeyboardmacosx.cpp
    llwindowmacosx.cpp
    llwindowmacosx-objc.mm
    )
  list(APPEND llwindow_HEADER_FILES
    llkeyboardmacosx.h
    llwindowmacosx.h
    llwindowmacosx-objc.h
    )

  # We use a bunch of deprecated system APIs.
  set_source_files_properties(
      llkeyboardmacosx.cpp
      llwindowmacosx.cpp
      PROPERTIES
      COMPILE_FLAGS "-Wno-deprecated-declarations -fpascal-strings"
      )
endif (DARWIN)

if (LINUX)
  list(APPEND viewer_SOURCE_FILES 
       llkeyboardsdl.cpp 
       llwindowsdl.cpp
       )
  list(APPEND viewer_HEADER_FILES
       llkeyboardsdl.h
       llwindowsdl.h
       )
endif (LINUX)

if (WINDOWS)
  list(APPEND llwindow_SOURCE_FILES
       llwindowwin32.cpp
       lldxhardware.cpp
       llkeyboardwin32.cpp
       )
  list(APPEND llwindow_HEADER_FILES
       llwindowwin32.h
       lldxhardware.h
       llkeyboardwin32.h
       )
  list(APPEND llwindow_LINK_LIBRARIES
       comdlg32     # Common Dialogs for ChooseColor
       )
endif (WINDOWS)

if (SOLARIS)
  list(APPEND llwindow_SOURCE_FILES
       llwindowsolaris.cpp
       )
  list(APPEND llwindow_HEADER_FILES
       llwindowsolaris.h
       )
endif (SOLARIS)

set_source_files_properties(${llwindow_HEADER_FILES}
                            PROPERTIES HEADER_FILE_ONLY TRUE)

if (SERVER AND NOT WINDOWS AND NOT DARWIN)
  set(server_SOURCE_FILES
       llwindowmesaheadless.cpp
       )
  set(server_HEADER_FILES
       llwindowmesaheadless.h
       )
  copy_server_sources(
      llwindow
      )


  set_source_files_properties(
    ${server_SOURCE_FILES}
    PROPERTIES
    COMPILE_FLAGS "-DLL_MESA=1 -DLL_MESA_HEADLESS=1"
    )
  add_library (llwindowheadless
    ${llwindow_SOURCE_FILES}
    ${server_SOURCE_FILES}
    )
  # *TODO: This should probably have target_link_libraries
endif (SERVER AND NOT WINDOWS AND NOT DARWIN)

if (llwindow_HEADER_FILES)
  list(APPEND llwindow_SOURCE_FILES ${llwindow_HEADER_FILES})
endif (llwindow_HEADER_FILES)
  list(APPEND viewer_SOURCE_FILES ${viewer_HEADER_FILES})

if (VIEWER)
  add_library (llwindow
    ${llwindow_SOURCE_FILES}
    ${viewer_SOURCE_FILES}
    )
  target_link_libraries (llwindow ${llwindow_LINK_LIBRARIES})
endif (VIEWER)

