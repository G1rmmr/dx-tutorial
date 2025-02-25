#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "freeimage::FreeImage" for configuration "Release"
set_property(TARGET freeimage::FreeImage APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(freeimage::FreeImage PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX;RC"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/FreeImage.lib"
  )

list(APPEND _cmake_import_check_targets freeimage::FreeImage )
list(APPEND _cmake_import_check_files_for_freeimage::FreeImage "${_IMPORT_PREFIX}/lib/FreeImage.lib" )

# Import target "freeimage::FreeImagePlus" for configuration "Release"
set_property(TARGET freeimage::FreeImagePlus APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(freeimage::FreeImagePlus PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX;RC"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/FreeImagePlus.lib"
  )

list(APPEND _cmake_import_check_targets freeimage::FreeImagePlus )
list(APPEND _cmake_import_check_files_for_freeimage::FreeImagePlus "${_IMPORT_PREFIX}/lib/FreeImagePlus.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
