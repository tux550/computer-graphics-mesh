SRC_MESH_FILES = mesh.cpp mesh.h
OUT_FILES = main.exe
p6:
	g++ -o $(OUT_FILES) $(SRC_MESH_FILES) p6.cpp
p5:
	g++ -o $(OUT_FILES) $(SRC_MESH_FILES) p5.cpp