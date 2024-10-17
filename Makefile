MESH_LIB_FILES = mesh/*.cpp mesh/*.h
OUT_FILES = main.exe
cm:
	g++ -o $(OUT_FILES) $(MESH_LIB_FILES) CatmullClark.cpp
se:
	g++ -o $(OUT_FILES) $(MESH_LIB_FILES) SplittingEdges.cpp