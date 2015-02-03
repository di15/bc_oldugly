# Install script for directory: C:/Users/polyfrag/Desktop/Projects/BattleComputer/aaa000/libs/win/libzip-0.11.1/man

# Set the install prefix
IF(NOT DEFINED CMAKE_INSTALL_PREFIX)
  SET(CMAKE_INSTALL_PREFIX "C:/Users/polyfrag/Desktop/Projects/BattleComputer/aaa000/libs/win/libzip-0.11.1/build")
ENDIF(NOT DEFINED CMAKE_INSTALL_PREFIX)
STRING(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
IF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  IF(BUILD_TYPE)
    STRING(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  ELSE(BUILD_TYPE)
    SET(CMAKE_INSTALL_CONFIG_NAME "Release")
  ENDIF(BUILD_TYPE)
  MESSAGE(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
ENDIF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)

# Set the component getting installed.
IF(NOT CMAKE_INSTALL_COMPONENT)
  IF(COMPONENT)
    MESSAGE(STATUS "Install component: \"${COMPONENT}\"")
    SET(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  ELSE(COMPONENT)
    SET(CMAKE_INSTALL_COMPONENT)
  ENDIF(COMPONENT)
ENDIF(NOT CMAKE_INSTALL_COMPONENT)

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man1" TYPE FILE RENAME "zipcmp.1" FILES "C:/Users/polyfrag/Desktop/Projects/BattleComputer/aaa000/libs/win/libzip-0.11.1/man/zipcmp.man")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man1" TYPE FILE RENAME "zipmerge.1" FILES "C:/Users/polyfrag/Desktop/Projects/BattleComputer/aaa000/libs/win/libzip-0.11.1/man/zipmerge.man")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man1" TYPE FILE RENAME "ziptorrent.1" FILES "C:/Users/polyfrag/Desktop/Projects/BattleComputer/aaa000/libs/win/libzip-0.11.1/man/ziptorrent.man")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "libzip.3" FILES "C:/Users/polyfrag/Desktop/Projects/BattleComputer/aaa000/libs/win/libzip-0.11.1/man/libzip.man")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_add.3" FILES "C:/Users/polyfrag/Desktop/Projects/BattleComputer/aaa000/libs/win/libzip-0.11.1/man/zip_add.man")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_add_dir.3" FILES "C:/Users/polyfrag/Desktop/Projects/BattleComputer/aaa000/libs/win/libzip-0.11.1/man/zip_add_dir.man")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_close.3" FILES "C:/Users/polyfrag/Desktop/Projects/BattleComputer/aaa000/libs/win/libzip-0.11.1/man/zip_close.man")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_delete.3" FILES "C:/Users/polyfrag/Desktop/Projects/BattleComputer/aaa000/libs/win/libzip-0.11.1/man/zip_delete.man")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_dir_add.3" FILES "C:/Users/polyfrag/Desktop/Projects/BattleComputer/aaa000/libs/win/libzip-0.11.1/man/zip_dir_add.man")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_discard.3" FILES "C:/Users/polyfrag/Desktop/Projects/BattleComputer/aaa000/libs/win/libzip-0.11.1/man/zip_discard.man")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_error_clear.3" FILES "C:/Users/polyfrag/Desktop/Projects/BattleComputer/aaa000/libs/win/libzip-0.11.1/man/zip_error_clear.man")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_error_get.3" FILES "C:/Users/polyfrag/Desktop/Projects/BattleComputer/aaa000/libs/win/libzip-0.11.1/man/zip_error_get.man")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_error_get_sys_type.3" FILES "C:/Users/polyfrag/Desktop/Projects/BattleComputer/aaa000/libs/win/libzip-0.11.1/man/zip_error_get_sys_type.man")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_error_to_str.3" FILES "C:/Users/polyfrag/Desktop/Projects/BattleComputer/aaa000/libs/win/libzip-0.11.1/man/zip_error_to_str.man")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_errors.3" FILES "C:/Users/polyfrag/Desktop/Projects/BattleComputer/aaa000/libs/win/libzip-0.11.1/man/zip_errors.man")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_fclose.3" FILES "C:/Users/polyfrag/Desktop/Projects/BattleComputer/aaa000/libs/win/libzip-0.11.1/man/zip_fclose.man")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_fdopen.3" FILES "C:/Users/polyfrag/Desktop/Projects/BattleComputer/aaa000/libs/win/libzip-0.11.1/man/zip_fdopen.man")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_file_add.3" FILES "C:/Users/polyfrag/Desktop/Projects/BattleComputer/aaa000/libs/win/libzip-0.11.1/man/zip_file_add.man")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_file_extra_field_delete.3" FILES "C:/Users/polyfrag/Desktop/Projects/BattleComputer/aaa000/libs/win/libzip-0.11.1/man/zip_file_extra_field_delete.man")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_file_extra_field_get.3" FILES "C:/Users/polyfrag/Desktop/Projects/BattleComputer/aaa000/libs/win/libzip-0.11.1/man/zip_file_extra_field_get.man")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_file_extra_field_set.3" FILES "C:/Users/polyfrag/Desktop/Projects/BattleComputer/aaa000/libs/win/libzip-0.11.1/man/zip_file_extra_field_set.man")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_file_extra_fields_count.3" FILES "C:/Users/polyfrag/Desktop/Projects/BattleComputer/aaa000/libs/win/libzip-0.11.1/man/zip_file_extra_fields_count.man")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_file_get_comment.3" FILES "C:/Users/polyfrag/Desktop/Projects/BattleComputer/aaa000/libs/win/libzip-0.11.1/man/zip_file_get_comment.man")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_file_rename.3" FILES "C:/Users/polyfrag/Desktop/Projects/BattleComputer/aaa000/libs/win/libzip-0.11.1/man/zip_file_rename.man")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_file_set_comment.3" FILES "C:/Users/polyfrag/Desktop/Projects/BattleComputer/aaa000/libs/win/libzip-0.11.1/man/zip_file_set_comment.man")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_file_strerror.3" FILES "C:/Users/polyfrag/Desktop/Projects/BattleComputer/aaa000/libs/win/libzip-0.11.1/man/zip_file_strerror.man")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_fopen.3" FILES "C:/Users/polyfrag/Desktop/Projects/BattleComputer/aaa000/libs/win/libzip-0.11.1/man/zip_fopen.man")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_fopen_encrypted.3" FILES "C:/Users/polyfrag/Desktop/Projects/BattleComputer/aaa000/libs/win/libzip-0.11.1/man/zip_fopen_encrypted.man")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_fread.3" FILES "C:/Users/polyfrag/Desktop/Projects/BattleComputer/aaa000/libs/win/libzip-0.11.1/man/zip_fread.man")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_get_archive_comment.3" FILES "C:/Users/polyfrag/Desktop/Projects/BattleComputer/aaa000/libs/win/libzip-0.11.1/man/zip_get_archive_comment.man")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_get_archive_flag.3" FILES "C:/Users/polyfrag/Desktop/Projects/BattleComputer/aaa000/libs/win/libzip-0.11.1/man/zip_get_archive_flag.man")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_get_file_comment.3" FILES "C:/Users/polyfrag/Desktop/Projects/BattleComputer/aaa000/libs/win/libzip-0.11.1/man/zip_get_file_comment.man")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_get_name.3" FILES "C:/Users/polyfrag/Desktop/Projects/BattleComputer/aaa000/libs/win/libzip-0.11.1/man/zip_get_name.man")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_get_num_entries.3" FILES "C:/Users/polyfrag/Desktop/Projects/BattleComputer/aaa000/libs/win/libzip-0.11.1/man/zip_get_num_entries.man")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_get_num_files.3" FILES "C:/Users/polyfrag/Desktop/Projects/BattleComputer/aaa000/libs/win/libzip-0.11.1/man/zip_get_num_files.man")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_name_locate.3" FILES "C:/Users/polyfrag/Desktop/Projects/BattleComputer/aaa000/libs/win/libzip-0.11.1/man/zip_name_locate.man")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_open.3" FILES "C:/Users/polyfrag/Desktop/Projects/BattleComputer/aaa000/libs/win/libzip-0.11.1/man/zip_open.man")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_rename.3" FILES "C:/Users/polyfrag/Desktop/Projects/BattleComputer/aaa000/libs/win/libzip-0.11.1/man/zip_rename.man")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_set_archive_comment.3" FILES "C:/Users/polyfrag/Desktop/Projects/BattleComputer/aaa000/libs/win/libzip-0.11.1/man/zip_set_archive_comment.man")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_set_archive_flag.3" FILES "C:/Users/polyfrag/Desktop/Projects/BattleComputer/aaa000/libs/win/libzip-0.11.1/man/zip_set_archive_flag.man")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_set_default_password.3" FILES "C:/Users/polyfrag/Desktop/Projects/BattleComputer/aaa000/libs/win/libzip-0.11.1/man/zip_set_default_password.man")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_set_file_comment.3" FILES "C:/Users/polyfrag/Desktop/Projects/BattleComputer/aaa000/libs/win/libzip-0.11.1/man/zip_set_file_comment.man")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_set_file_compression.3" FILES "C:/Users/polyfrag/Desktop/Projects/BattleComputer/aaa000/libs/win/libzip-0.11.1/man/zip_set_file_compression.man")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_source_buffer.3" FILES "C:/Users/polyfrag/Desktop/Projects/BattleComputer/aaa000/libs/win/libzip-0.11.1/man/zip_source_buffer.man")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_source_file.3" FILES "C:/Users/polyfrag/Desktop/Projects/BattleComputer/aaa000/libs/win/libzip-0.11.1/man/zip_source_file.man")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_source_filep.3" FILES "C:/Users/polyfrag/Desktop/Projects/BattleComputer/aaa000/libs/win/libzip-0.11.1/man/zip_source_filep.man")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_source_free.3" FILES "C:/Users/polyfrag/Desktop/Projects/BattleComputer/aaa000/libs/win/libzip-0.11.1/man/zip_source_free.man")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_source_function.3" FILES "C:/Users/polyfrag/Desktop/Projects/BattleComputer/aaa000/libs/win/libzip-0.11.1/man/zip_source_function.man")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_source_zip.3" FILES "C:/Users/polyfrag/Desktop/Projects/BattleComputer/aaa000/libs/win/libzip-0.11.1/man/zip_source_zip.man")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_stat.3" FILES "C:/Users/polyfrag/Desktop/Projects/BattleComputer/aaa000/libs/win/libzip-0.11.1/man/zip_stat.man")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_stat_init.3" FILES "C:/Users/polyfrag/Desktop/Projects/BattleComputer/aaa000/libs/win/libzip-0.11.1/man/zip_stat_init.man")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_unchange.3" FILES "C:/Users/polyfrag/Desktop/Projects/BattleComputer/aaa000/libs/win/libzip-0.11.1/man/zip_unchange.man")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_unchange_all.3" FILES "C:/Users/polyfrag/Desktop/Projects/BattleComputer/aaa000/libs/win/libzip-0.11.1/man/zip_unchange_all.man")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_unchange_archive.3" FILES "C:/Users/polyfrag/Desktop/Projects/BattleComputer/aaa000/libs/win/libzip-0.11.1/man/zip_unchange_archive.man")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_file_error_clear.3" FILES "C:/Users/polyfrag/Desktop/Projects/BattleComputer/aaa000/libs/win/libzip-0.11.1/man/zip_error_clear.man")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_file_error_get.3" FILES "C:/Users/polyfrag/Desktop/Projects/BattleComputer/aaa000/libs/win/libzip-0.11.1/man/zip_error_get.man")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_file_replace.3" FILES "C:/Users/polyfrag/Desktop/Projects/BattleComputer/aaa000/libs/win/libzip-0.11.1/man/zip_file_add.man")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_file_extra_field_delete_by_id.3" FILES "C:/Users/polyfrag/Desktop/Projects/BattleComputer/aaa000/libs/win/libzip-0.11.1/man/zip_file_extra_field_delete.man")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_file_extra_field_get_by_id.3" FILES "C:/Users/polyfrag/Desktop/Projects/BattleComputer/aaa000/libs/win/libzip-0.11.1/man/zip_file_extra_field_get.man")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_file_extra_fields_count_by_id.3" FILES "C:/Users/polyfrag/Desktop/Projects/BattleComputer/aaa000/libs/win/libzip-0.11.1/man/zip_file_extra_fields_count.man")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_fopen_index.3" FILES "C:/Users/polyfrag/Desktop/Projects/BattleComputer/aaa000/libs/win/libzip-0.11.1/man/zip_fopen.man")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_fopen_index_encrypted.3" FILES "C:/Users/polyfrag/Desktop/Projects/BattleComputer/aaa000/libs/win/libzip-0.11.1/man/zip_fopen_encrypted.man")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_replace.3" FILES "C:/Users/polyfrag/Desktop/Projects/BattleComputer/aaa000/libs/win/libzip-0.11.1/man/zip_add.man")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_stat_index.3" FILES "C:/Users/polyfrag/Desktop/Projects/BattleComputer/aaa000/libs/win/libzip-0.11.1/man/zip_stat.man")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/man/man3" TYPE FILE RENAME "zip_strerror.3" FILES "C:/Users/polyfrag/Desktop/Projects/BattleComputer/aaa000/libs/win/libzip-0.11.1/man/zip_file_strerror.man")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

