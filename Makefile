hw3: hw3.cpp vshader1.glsl fshader1.glsl Angel.h CheckError.h mat.h vec.h\
		textfile.h textfile.cpp InitShader.cpp Mesh.hpp PLYReader.hpp\
		MeshRenderer.hpp
	g++ hw3.cpp -g -Wall -lglut -lGL -lGLEW -o hw3

clean:
	rm hw3

