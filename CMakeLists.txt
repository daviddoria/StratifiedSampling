cmake_minimum_required(VERSION 2.6)

Project(StratifiedSampling)
ENABLE_TESTING()

SET(BUILD_PARAVIEW_PLUGIN OFF CACHE BOOL "Build Paraview plugin?")

# Use an external VTK build if Paraview is not used (Paraview includes its own VTK)
if(NOT BUILD_PARAVIEW_PLUGIN)
  FIND_PACKAGE(VTK REQUIRED)
  INCLUDE(${VTK_USE_FILE})
endif(NOT BUILD_PARAVIEW_PLUGIN)

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

##### Build Library #####
add_library(StratifiedLibrary ${implementationSources})
TARGET_LINK_LIBRARIES(StratifiedLibrary ${VTK_LIBRARIES})

##### Optionally build Paraview Plugin #####
if(BUILD_PARAVIEW_PLUGIN)
  FIND_PACKAGE(ParaView REQUIRED)
  INCLUDE(${PARAVIEW_USE_FILE})

  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

  ADD_PARAVIEW_PLUGIN(StratifiedSamplingPlugin "1.0"
    SERVER_MANAGER_XML vtkStratifiedSampling.xml
    SERVER_MANAGER_SOURCES vtkStratifiedSampling.cxx)

  TARGET_LINK_LIBRARIES(StratifiedSamplingPlugin StratifiedLibrary)
endif(BUILD_PARAVIEW_PLUGIN)

##### Build Demo #####
ADD_EXECUTABLE(StratifiedSampling  vtkStratifiedSampling.cxx main.cpp)
TARGET_LINK_LIBRARIES(StratifiedSampling StratifiedLibrary ${VTK_LIBRARIES})

##### Build Test #####
ADD_EXECUTABLE(Test Test.cxx vtkStratifiedSampling.cxx)
TARGET_LINK_LIBRARIES(Test StratifiedLibrary ${VTK_LIBRARIES})

ADD_TEST(StratifiedSamplingTest Test)
