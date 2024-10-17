MESH_LIB_FILES = mesh/*.cpp mesh/*.h
OUT_FILES = main.exe
cc:
	g++ -o $(OUT_FILES) $(MESH_LIB_FILES) algos/CatmullClark.cpp
se:
	g++ -o $(OUT_FILES) $(MESH_LIB_FILES) algos/SplittingEdges.cpp