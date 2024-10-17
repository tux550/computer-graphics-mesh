MESH_LIB_FILES = mesh/mesh.cpp mesh/mesh.h
OUT_FILES = main.exe
cm:
	g++ -o $(OUT_FILES) $(MESH_LIB_FILES) CatmullClark.cpp
se:
	g++ -o $(OUT_FILES) $(MESH_LIB_FILES) SplittingEdges.cpp