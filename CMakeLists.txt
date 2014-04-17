cmake_minimum_required(VERSION 2.6)

Project(StratifiedSampling)
ENABLE_TESTING()

FIND_PACKAGE(VTK REQUIRED)
INCLUDE(${VTK_USE_FILE})

SET(BUILD_PARAVIEW_PLUGIN OFF CACHE BOOL "Build Paraview plugin?")

set(ImplementationPath ./implementation/)
INCLUDE_DIRECTORIES(${INCLUDE_DIRECTORIES} ${ImplementationPath})
SET(implementationSources
${ImplementationPath}/boxbox.cpp
${ImplementationPath}/diffuse.cc
${ImplementationPath}/edgeflip.cc
${ImplementationPath}/faceflip.cc
${ImplementationPath}/filter.cc
${ImplementationPath}/ICP.cc
${ImplementationPath}/KDtree.cc
${ImplementationPath}/lmsmooth.cc
${ImplementationPath}/pointsample.cpp
${ImplementationPath}/remove.cc
${ImplementationPath}/reorder_verts.cc
${ImplementationPath}/sample.cpp
${ImplementationPath}/stratify.cpp
${ImplementationPath}/subdiv.cc
${ImplementationPath}/tribox.cpp
${ImplementationPath}/TriMesh.cc
${ImplementationPath}/TriMesh_bounding.cc
${ImplementationPath}/TriMesh_connectivity.cc
${ImplementationPath}/TriMesh_curvature.cc
${ImplementationPath}/TriMesh_grid.cc
${ImplementationPath}/TriMesh_normals.cc
${ImplementationPath}/TriMesh_pointareas.cc
${ImplementationPath}/TriMesh_stats.cc
${ImplementationPath}/TriMesh_tstrips.cc)

add_library(StratifiedLibrary ${implementationSources})
TARGET_LINK_LIBRARIES(StratifiedLibrary ${VTK_LIBRARIES})

#if(BUILD_PARAVIEW_PLUGIN)
#  FIND_PACKAGE(ParaView REQUIRED)
#  INCLUDE(${PARAVIEW_USE_FILE})

#  INCLUDE_DIRECTORIES(${INCLUDE_DIRECTORIES} ../)

#  ADD_PARAVIEW_PLUGIN(StratifiedSamplingPlugin "1.0"
#    SERVER_MANAGER_XML vtkStratifiedSampling.xml
#    SERVER_MANAGER_SOURCES vtkStratifiedSampling.cxx
#    SERVER_SOURCES boxbox.cpp diffuse.cc edgeflip.cc faceflip.cc filter.cc ICP.cc KDtree.cc lmsmooth.cc pointsample.cpp remove.cc reorder_verts.cc sample.cpp stratify.cpp subdiv.cc tribox.cpp TriMesh.cc TriMesh_bounding.cc TriMesh_connectivity.cc TriMesh_curvature.cc TriMesh_grid.cc TriMesh_normals.cc TriMesh_pointareas.cc TriMesh_stats.cc TriMesh_tstrips.cc )

#endif(BUILD_PARAVIEW_PLUGIN)

##### Build Demo #####
ADD_EXECUTABLE(StratifiedSampling  vtkStratifiedSampling.cxx main.cpp)
TARGET_LINK_LIBRARIES(StratifiedSampling StratifiedLibrary ${VTK_LIBRARIES})

##### Build Test #####
ADD_EXECUTABLE(Test Test.cxx vtkStratifiedSampling.cxx)
TARGET_LINK_LIBRARIES(Test StratifiedLibrary ${VTK_LIBRARIES})

ADD_TEST(StratifiedSamplingTest Test)
