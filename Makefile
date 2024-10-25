# find mesh/*.cpp mesh/*.h
MESH_LIB_FILES = $(wildcard mesh/*.cpp) $(wildcard mesh/*.h)
OUT_FILES = main.exe
OUTPUT_FOLDERS = outputs
EPS_FILES = simplicit.eps
marching_cubes:
	g++ -o $(OUT_FILES) -I ./mesh $(MESH_LIB_FILES) marching/MarchingCubes.cpp
	./$(OUT_FILES)
marching_squares:
	g++ -o $(OUT_FILES) -I ./mesh  $(MESH_LIB_FILES) marching/MarchingSquares.cpp 
	./$(OUT_FILES)
	cd $(OUTPUT_FOLDERS) && epstopdf $(EPS_FILES)
cc:
	g++ -o $(OUT_FILES) $(MESH_LIB_FILES) algos/CatmullClark.cpp
se:
	g++ -o $(OUT_FILES) $(MESH_LIB_FILES) algos/SplittingEdges.cpp