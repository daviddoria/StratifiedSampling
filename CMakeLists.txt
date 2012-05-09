cmake_minimum_required(VERSION 2.6)

Project(StratifiedSampling)
ENABLE_TESTING()

FIND_PACKAGE(VTK REQUIRED)
INCLUDE(${VTK_USE_FILE})


SET(BUILD_PARAVIEW_PLUGIN OFF CACHE BOOL "Build Paraview plugin?")

SET(sources boxbox.cpp
diffuse.cc
edgeflip.cc
faceflip.cc
filter.cc
ICP.cc
KDtree.cc
lmsmooth.cc
pointsample.cpp
remove.cc
reorder_verts.cc
sample.cpp
stratify.cpp
subdiv.cc
tribox.cpp
TriMesh.cc
TriMesh_bounding.cc
TriMesh_connectivity.cc
TriMesh_curvature.cc
TriMesh_grid.cc
TriMesh_normals.cc
TriMesh_pointareas.cc
TriMesh_stats.cc
TriMesh_tstrips.cc)

if(BUILD_PARAVIEW_PLUGIN)
FIND_PACKAGE(ParaView REQUIRED)
INCLUDE(${PARAVIEW_USE_FILE})

INCLUDE_DIRECTORIES(${INCLUDE_DIRECTORIES} ../)

ADD_PARAVIEW_PLUGIN(StratifiedSamplingPlugin "1.0"
  SERVER_MANAGER_XML vtkStratifiedSampling.xml
SERVER_MANAGER_SOURCES vtkStratifiedSampling.cxx  
SERVER_SOURCES boxbox.cpp diffuse.cc edgeflip.cc faceflip.cc filter.cc ICP.cc KDtree.cc lmsmooth.cc pointsample.cpp remove.cc reorder_verts.cc sample.cpp stratify.cpp subdiv.cc tribox.cpp TriMesh.cc TriMesh_bounding.cc TriMesh_connectivity.cc TriMesh_curvature.cc TriMesh_grid.cc TriMesh_normals.cc TriMesh_pointareas.cc TriMesh_stats.cc TriMesh_tstrips.cc )

endif(BUILD_PARAVIEW_PLUGIN)

ADD_EXECUTABLE(StratifiedSampling  vtkStratifiedSampling.cxx main.cpp
${sources}
)

TARGET_LINK_LIBRARIES(StratifiedSampling ${VTK_LIBRARIES})

##############
ADD_EXECUTABLE(Test Test.cxx vtkStratifiedSampling.cxx 
${sources})
TARGET_LINK_LIBRARIES(Test ${VTK_LIBRARIES})

ADD_TEST(StratifiedSamplingTest Test)