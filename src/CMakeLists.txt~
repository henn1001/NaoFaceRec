# Sets the used source files
set(_srcs
  	main.cpp
        faceVACS.cpp
        faceVACS.h
	edialog.h
	idialog.h
)

# Create a executable named nfr
# with the source file specified in "_srcs"
qi_create_bin(nfr ${_srcs})


# Here we say that our executable depends on
# - ALCOMMON (main naoqi lib)
# - ALVISION (for vision definitions)
# - OPENCV (display)
#
# It automatically links with the corresponding libraries and make their headers
# available.
qi_use_lib(nfr ALCOMMON ALVISION OPENCV2_CORE OPENCV2_HIGHGUI)
# Used a standart cmake command because there is no cmake-config file for the FRSDK
target_link_libraries(nfr frsdk-8.9.5 hasp_linux_x86_64_67109)  
