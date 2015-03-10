# hopfer.benjamin@gmail.com
# http://benjaminhopfer.com
# 
# Example application for Qt / vtk
# 32bit/64bit cross compile
#
# Source file listings
# --------------------------------
# GATHER AND PREPARE ALL OWN FILES and TARGETS
# 
set(GLOBAL_SRCS_H
	${CMAKE_CURRENT_SOURCE_DIR}/Phonger.h
)

set(GLOBAL_SRCS_UI
	${CMAKE_CURRENT_SOURCE_DIR}/Phonger.ui
)

set(GLOBAL_SRCS_CXX 
	${CMAKE_CURRENT_SOURCE_DIR}/Phonger.cxx
	
	${CMAKE_CURRENT_SOURCE_DIR}/main.cxx
)


set(MOC_HEADERS ${GLOBAL_SRCS_H})
	
set(UI_FILES ${GLOBAL_SRCS_UI})

set(QT_RES )
	
set(H_FILES ${GLOBAL_SRCS_H})
	
SET(CXX_FILES ${GLOBAL_SRCS_CXX})

# We don't use AUTOMOC and AUTOUIC, because we want to add the according files to the "gen" sourcegroup
qt5_wrap_ui(UISrcs ${UI_FILES})
qt5_wrap_cpp(MOCSrcs ${MOC_HEADERS})
qt5_add_resources(RCCSrcs ${QT_RES})

add_executable(${PROJ_NAME} ${H_FILES} ${CXX_FILES} ${UISrcs} ${MOCSrcs} ${RCCSrcs})
qt5_use_modules(${PROJ_NAME} Core Gui Widgets)
target_link_libraries(${PROJ_NAME} ${VTK_LIBRARIES} optimized ${Qt5Core_QTMAIN_LIBRARIES} )

# --------------------------------
# SET UP FOR VISUAL STUDIO

# Configure the template file that allows debugging
set(USER_FILE ${PROJ_NAME}.vcxproj.user) 	
set(OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/${USER_FILE})
configure_file(UserTemplate.user ${USER_FILE} @ONLY)

source_group(gen FILES ${UISrcs} ${MOCSrcs} ${RCCSrcs})
source_group(global FILES ${GLOBAL_SRCS_H} ${GLOBAL_SRCS_UI} ${GLOBAL_SRCS_CXX} ${GLOBAL_SRCS_MAIN})