cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

function(khewil_download url zipped_file_path)
	get_filename_component(zipped_file_parent_dir ${zipped_file_path} DIRECTORY)
	if (EXISTS "${zipped_file_path}")
		message("${zipped_file_path} already exists - no need to download it")
	else()
		file(MAKE_DIRECTORY "${zipped_file_parent_dir}")
		message("Doing: ${KHEWIL_TOOL_CURL} --retry 5 --retry-delay 15 --location --output ${zipped_file_path} ${url}")
		execute_process(
			COMMAND
				"${KHEWIL_TOOL_CURL}"
				--retry 5 --retry-delay 15 --location
				--output "${zipped_file_path}"
				"${url}"
			)
	endif()
endfunction()

function(khewil_unzip zipped_file_path unzipped_dir)
	get_filename_component(unzipped_parent_dir ${unzipped_dir} DIRECTORY)
	if (EXISTS "${unzipped_dir}")
		message("${unzipped_dir} already exists - no need to unzip it")
	else()
		file(MAKE_DIRECTORY "${unzipped_parent_dir}")
		message("Doing: ${KHEWIL_TOOL_ZIP} x -y -o${unzipped_parent_dir} ${zipped_file_path}")
		execute_process(
			COMMAND
				"${KHEWIL_TOOL_ZIP}"
				x -y "-o${unzipped_parent_dir}"
				"${zipped_file_path}"
			)
	endif()
endfunction()

function(khewil_download_unzip url zipped_file_path unzipped_dir)
	khewil_download("${url}" "${zipped_file_path}")
	khewil_unzip("${zipped_file_path}" "${unzipped_dir}")
endfunction()

function(khewil_copy source_dir target_dir source_filer is_recursive)
	if (is_recursive)
		execute_process(COMMAND robocopy "${source_dir}" "${target_dir}" "${source_filer}" /NFL /NDL /NJH /NJS /NC /NS /NP /S /E)
	else()
		execute_process(COMMAND robocopy "${source_dir}" "${target_dir}" "${source_filer}" /NFL /NDL /NJH /NJS /NC /NS /NP)
	endif()
endfunction()

function(khewil_zip_upload unzipped_dir zipped_file_path)
	if (EXISTS "${zipped_file_path}")
		message("${zipped_file_path} already exists - no need to zip it")
	else()
		execute_process(
			COMMAND "${KHEWIL_TOOL_ZIP}" a -r "${zipped_file_path}" "${unzipped_dir}/*"
			)
	endif()
	#TODO: upload to a central artifact storage server
endfunction()

function(khewil_use_thirdparty target_name thirdparty_name thirdparty_components)
	find_package(${thirdparty_name})
	include_directories(${${thirdparty_name}_INCLUDES})
	foreach(thirdparty_component ${thirdparty_components})
		target_link_libraries(${target_name} "${${thirdparty_name}_LIBRARIES}/${thirdparty_component}.lib")
	endforeach()
endfunction()
