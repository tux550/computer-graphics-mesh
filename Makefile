SRC_MESH_FILES = $(wildcard mesh/*.cpp)
SRC_MESH_HEADERS = $(wildcard mesh/*.h)
OUTPUT_FOLDERS = outputs
OUTPUT_FILE_EPS = implicit.eps

# HELP
help: # Automatically parse from Makefile
	@echo "Available commands:"
	@egrep "^[a-zA-Z_]+:.*?# .*$$" $(MAKEFILE_LIST) | awk 'BEGIN {FS = ":.*?# "}; {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}'

# BUILD
ray_tracer: # Build RayTracer
	g++ -o RayTracer.exe -I ./mesh  $(SRC_MESH_FILES)  render/render.cpp

marching_cubes: # Build MarchingCubes
	g++ -o MarchingCubes.exe -I ./mesh $(SRC_MESH_FILES) marching/MarchingCubes.cpp
marching_squares: # Build MarchingSquares
	g++ -o MarchingSquares.exe -I ./mesh  $(SRC_MESH_FILES) marching/MarchingSquares.cpp 
catmull_clark: # Build CatmullClark
	g++ -o CatmullClark.exe -I ./mesh $(SRC_MESH_FILES) algos/CatmullClark.cpp
splitting_edges: # Build SplittingEdges
	g++ -o SplittingEdges.exe -I ./mesh $(SRC_MESH_FILES) misc/SplittingEdges.cpp

# UTILS	
topdf: # Transform eps files to pdf (MarchingSquares)
	cd $(OUTPUT_FOLDERS) && epstopdf $(OUTPUT_FILE_EPS)

# DOCKER
build_image: # Build docker image
	docker build -t mesh .
shell: build_image # Run docker image
	docker run -it --rm -v $(shell pwd):/app mesh bash
clean:
	rm -f *.exe