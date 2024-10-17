MESH_LIB_FILES = mesh/*.cpp mesh/*.h
OUT_FILES = main.exe
EPS_FILES = implicit.eps
il:
	g++ -o $(OUT_FILES) algos/ImplicitToLines.cpp
	./$(OUT_FILES)
	epstopdf $(EPS_FILES)
is:
	g++ -o $(OUT_FILES) algos/Implicit2D.cpp
	./$(OUT_FILES)
	epstopdf $(EPS_FILES)
cc:
	g++ -o $(OUT_FILES) $(MESH_LIB_FILES) algos/CatmullClark.cpp
se:
	g++ -o $(OUT_FILES) $(MESH_LIB_FILES) algos/SplittingEdges.cpp