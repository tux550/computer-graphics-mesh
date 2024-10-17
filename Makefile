MESH_LIB_FILES = mesh/*.cpp mesh/*.h
OUT_FILES = main.exe
il:
	g++ -o $(OUT_FILES) algos/ImplicitToLines.cpp
is:
	g++ -o $(OUT_FILES) algos/Implicit2D.cpp
cc:
	g++ -o $(OUT_FILES) $(MESH_LIB_FILES) algos/CatmullClark.cpp
se:
	g++ -o $(OUT_FILES) $(MESH_LIB_FILES) algos/SplittingEdges.cpp