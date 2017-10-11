
set(MONGO_NAMEVERSION "mongo-cxx-driver-legacy-1.0.7")
set(MONGO_ROOT "${KHEWIL_THIRDPARTIES_INSTALL_DIR}/${MONGO_NAMEVERSION}")

if (EXISTS "${MONGO_ROOT}")
	set(MONGO_FOUND ON)
	set(MONGO_INCLUDES "${MONGO_ROOT}/inc")
	set(MONGO_LIBRARIES "${MONGO_ROOT}/lib")
	set(MONGO_BINARIES "${MONGO_ROOT}/bin")
	set(MONGO_BINARIES "${MONGO_BINARIES}" PARENT_SCOPE)
else()
	set(MONGO_FOUND OFF)
endif()

message(STATUS "MONGO_FOUND=${MONGO_FOUND}")
message(STATUS "MONGO_ROOT=${MONGO_ROOT}")
message(STATUS "MONGO_INCLUDES=${MONGO_INCLUDES}")
message(STATUS "MONGO_LIBRARIES=${MONGO_LIBRARIES}")
message(STATUS "MONGO_BINARIES=${MONGO_BINARIES}")