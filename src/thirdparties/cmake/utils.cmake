
message(STATUS "KHEWIL_UTIL_METHOD=${KHEWIL_UTIL_METHOD}")

if("${KHEWIL_UTIL_METHOD}" STREQUAL "COPY_FILES")
	message(STATUS "KHEWIL_UTIL_SOURCE_DIR=${KHEWIL_UTIL_SOURCE_DIR}")
	message(STATUS "KHEWIL_UTIL_SOURCE_FILTER=${KHEWIL_UTIL_SOURCE_FILTER}")
	message(STATUS "KHEWIL_UTIL_TARGET_DIR=${KHEWIL_UTIL_TARGET_DIR}")
	message(STATUS "KHEWIL_UTIL_RECURSIVE=${KHEWIL_UTIL_RECURSIVE}")
	if (KHEWIL_UTIL_RECURSIVE)
		execute_process(COMMAND robocopy "${KHEWIL_UTIL_SOURCE_DIR}" "${KHEWIL_UTIL_TARGET_DIR}" "${KHEWIL_UTIL_SOURCE_FILTER}" /NFL /NDL /NJH /NJS /NC /NS /NP /S /E)
	else()
		execute_process(COMMAND robocopy "${KHEWIL_UTIL_SOURCE_DIR}" "${KHEWIL_UTIL_TARGET_DIR}" "${KHEWIL_UTIL_SOURCE_FILTER}" /NFL /NDL /NJH /NJS /NC /NS /NP)
	endif()
#	file(GLOB KHEWIL_UTIL_FILES "${KHEWIL_UTIL_SOURCE_DIR_FILTER}")
#	foreach(KHEWIL_UTIL_FILE ${KHEWIL_UTIL_FILES})
#		message("Copying ${KHEWIL_UTIL_FILE} to ${KHEWIL_UTIL_TARGET_DIR}")
#		file(COPY "${KHEWIL_UTIL_FILE}" DESTINATION "${KHEWIL_UTIL_TARGET_DIR}")
#	endforeach()
endif()