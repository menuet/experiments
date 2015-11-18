
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
elseif("${KHEWIL_UTIL_METHOD}" STREQUAL "PRE_BUILD_PACKAGE")
	message(STATUS "KHEWIL_UTIL_TOOL_CURL=${KHEWIL_UTIL_TOOL_CURL}")
	message(STATUS "KHEWIL_UTIL_TOOL_ZIP=${KHEWIL_UTIL_TOOL_ZIP}")
	message(STATUS "KHEWIL_UTIL_SRC_ZIP_URL=${KHEWIL_UTIL_SRC_ZIP_URL}")
	message(STATUS "KHEWIL_UTIL_SRC_ZIP_DIR=${KHEWIL_UTIL_SRC_ZIP_DIR}")
	message(STATUS "KHEWIL_UTIL_SRC_UNZIP_DIR=${KHEWIL_UTIL_SRC_UNZIP_DIR}")
	message(STATUS "KHEWIL_UTIL_SRC_NAMEVERSION=${KHEWIL_UTIL_SRC_NAMEVERSION}")
	message(STATUS "KHEWIL_UTIL_SRC_EXTENSION=${KHEWIL_UTIL_SRC_EXTENSION}")
	if (!EXISTS "${KHEWIL_UTIL_SRC_ZIP_DIR}/${KHEWIL_UTIL_SRC_NAMEVERSION}.${KHEWIL_UTIL_SRC_EXTENSION}")
		file(MAKE_DIRECTORY "${KHEWIL_UTIL_SRC_ZIP_DIR}")
		execute_process(
			COMMAND
				"${KHEWIL_UTIL_TOOL_CURL}"
				--retry 5 --retry-delay 15 --location
				--output "${KHEWIL_UTIL_SRC_ZIP_DIR}/${KHEWIL_UTIL_SRC_NAMEVERSION}.${KHEWIL_UTIL_SRC_EXTENSION}"
				"${KHEWIL_UTIL_SRC_ZIP_URL}"
			)
	endif()
	if (!EXISTS "${KHEWIL_UTIL_SRC_UNZIP_DIR}/${KHEWIL_UTIL_SRC_NAMEVERSION}")
		file(MAKE_DIRECTORY "${KHEWIL_UTIL_SRC_UNZIP_DIR}")
		execute_process(
			COMMAND
				"${KHEWIL_UTIL_TOOL_ZIP}"
				x -y "-o${KHEWIL_UTIL_SRC_UNZIP_DIR}"
				"${KHEWIL_UTIL_SRC_ZIP_DIR}/${KHEWIL_UTIL_SRC_NAMEVERSION}.${KHEWIL_UTIL_SRC_EXTENSION}"
			)
	endif()
elseif("${KHEWIL_UTIL_METHOD}" STREQUAL "POST_BUILD_PACKAGE")
endif()
