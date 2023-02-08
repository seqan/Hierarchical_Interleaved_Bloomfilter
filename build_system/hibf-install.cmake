# ------------------------------------------------------------------------------------------------------------
# Copyright (c) 2006-2023, Knut Reinert & Freie Universität Berlin
# Copyright (c) 2016-2023, Knut Reinert & MPI für molekulare Genetik
# This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
# shipped with this file and also available at: https://github.com/seqan/Hierarchical_Interleaved_Bloomfilter/blob/main/LICENSE.md
# ------------------------------------------------------------------------------------------------------------

# This file describes where and which parts of HIBF should be installed to.

cmake_minimum_required (VERSION 3.14)

include (GNUInstallDirs)

# install documentation files in /share/doc
install (FILES "${HIBF_CLONE_DIR}/CHANGELOG.md" #
               "${HIBF_CLONE_DIR}/CODE_OF_CONDUCT.md" #
               "${HIBF_CLONE_DIR}/CONTRIBUTING.md" #
               "${HIBF_CLONE_DIR}/LICENSE.md" #
               "${HIBF_CLONE_DIR}/README.md"
         TYPE DOC)

# install cmake files in /share/cmake
install (FILES "${HIBF_CLONE_DIR}/build_system/hibf-config.cmake"
               "${HIBF_CLONE_DIR}/build_system/hibf-config-version.cmake"
         DESTINATION "${CMAKE_INSTALL_DATADIR}/cmake/hibf")

# install hibf header files in /include/hibf
install (DIRECTORY "${HIBF_INCLUDE_DIR}/hibf" TYPE INCLUDE)

# install submodule header files, e.g. all external dependencies in /home/user/hibf/submodules/*,
# in /include/hibf/submodules/<submodule>/include
foreach (submodule_dir ${HIBF_DEPENDENCY_INCLUDE_DIRS})
    # e.g. submodule_dir: (1) /home/user/hibf/submodules/sdsl-lite/include or (2) /usr/include
    # strip /home/user/hibf/submodules/ and /include part.
    file (RELATIVE_PATH submodule "${HIBF_SUBMODULES_DIR}/submodules" "${submodule_dir}/..")
    # submodule is either a single module name, like sdsl-lite or a relative path to a folder ../../../usr
    # skip relative folders and only keep submodules that reside in the submodules folder
    if (NOT submodule MATCHES "^\\.\\.") # skip relative folders
        install (DIRECTORY "${submodule_dir}"
                 DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/hibf/submodules/${submodule}")
    endif ()
endforeach ()
