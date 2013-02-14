
#ifndef __LSYSTEMRENDERER_H_
#define __LSYSTEMRENDERER_H_

#include "LSystem.hpp"
#include "PLYReader.hpp"



class LSystemRenderer {
	private:
		GLuint program;
		GLuint bufferStart;
		LSystem& lsys;
		mat4 projection;

		int screenWidth;
		int screenHeight;
		
		Mesh* sphere;
		GLsizeiptr sphereLength;

		Mesh* cylinder;
		GLsizeiptr cylinderLength;

		void bufferPoints() {
			cylinderLength = cylinder->getNumPoints();
			vec4* cylinderPoints = cylinder->getPoints();
			GLsizeiptr cylinderBytes = sizeof(cylinderPoints[0]) * cylinderLength;

			sphereLength = sphere->getNumPoints();
			vec4* spherePoints = sphere->getPoints();
			GLsizeiptr sphereBytes = sizeof(spherePoints[0]) * sphereLength;

			GLsizeiptr totalBytes = sphereBytes + cylinderBytes;
			glBufferData(GL_ARRAY_BUFFER, totalBytes, NULL, GL_STATIC_DRAW);
			glBufferSubData(GL_ARRAY_BUFFER, bufferStart, cylinderBytes, cylinderPoints);
			glBufferSubData(GL_ARRAY_BUFFER, bufferStart + cylinderBytes, sphereBytes, spherePoints);

			GLuint posLoc = glGetAttribLocation(program, "vPosition");
			glEnableVertexAttribArray(posLoc);
			glVertexAttribPointer(posLoc, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
		}

		void resetProjection() {
			if(screenHeight == 0) {
				projection = mat4(); // don't want to divide by zero...
				return;
			}
			projection = mat4()
				* Perspective(90, (float)screenWidth/screenHeight, 0.0000001, 100000)
				* LookAt(vec3(10, 30, 10), vec3(0, 0, 0), vec3(0, 1, 0));
		}

	public:
		LSystemRenderer(GLuint program, GLuint bufferStart, LSystem& lsys)
				: lsys(lsys) {
			this->program = program;
			this->bufferStart = bufferStart;
			screenWidth = 0;
			screenHeight = 0;
			PLYReader sphereReader("meshes/sphere.ply");
			sphere = sphereReader.read();
			PLYReader cylinderReader("meshes/cylinder.ply");
			cylinder = cylinderReader.read();
			resetProjection();
			bufferPoints();
		}

		void display() {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glEnable(GL_DEPTH_TEST);
			GLuint projLoc = glGetUniformLocationARB(program, "projection_matrix");
			glUniformMatrix4fv(projLoc, 1, GL_TRUE, projection);

			Turtle* turtle = lsys.getTurtleCopy();
			stack<mat4> modelView;
			modelView.push(RotateX(-90)); // point the tree upwards
			turtle->ctm = &modelView;
			string turtleString = lsys.getTurtleString();

			for(string::iterator it = turtleString.begin(); it != turtleString.end(); ++it) {
				char currentChar = *it;
				
				if(currentChar == 'F') {
					vec3 size = cylinder->getBoundingBox()->getSize();

					// want to scale the cylinder to be segmentLength long, thin
					float ratio = 0.25 / size.y;
					mat4 scale = Scale(ratio, ratio, turtle->segmentLength / size.z);

					// move cylinder so its end is flush with the yz plane
					vec4 center = cylinder->getBoundingBox()->getCenter();
					vec4 dest = vec4(0, 0, size.z/2, 1);
					mat4 trans = Translate(dest - center);

					// rotate so tree grows upwards out of xz plane
					//mat4 rotate = RotateX(90);

					mat4 finalModel = modelView.top() * trans * scale;

					// draw next cylinder
					GLuint modelLoc = glGetUniformLocationARB(program, "model_matrix");
					glUniformMatrix4fv(modelLoc, 1, GL_TRUE, finalModel);

					glDrawArrays(GL_TRIANGLES, bufferStart, cylinderLength);
				}

				switch(currentChar) {
					case 'F':
					case 'f':
						turtle->forward();
						break;
					case '+':
						turtle->rotate(Turtle::X, true);
						break;
					case '-':
						turtle->rotate(Turtle::X, false);
						break;
					case '&':
						turtle->rotate(Turtle::Y, true);
						break;
					case '^':
						turtle->rotate(Turtle::Y, false);
						break;
					case '\\':
						turtle->rotate(Turtle::Z, true);
						break;
					case '/':
						turtle->rotate(Turtle::Z, false);
						break;
					case '|':
						turtle->turnAround();
						break;
					case '[':
						turtle->push();
						break;
					case ']':
						turtle->pop();
						break;
				}
			}

			glDisable(GL_DEPTH_TEST); 

			// output to hardware, double buffered
			glFlush();
			glutSwapBuffers();
		}

		void reshape(int screenWidth, int screenHeight) {
			this->screenWidth = screenWidth;
			this->screenHeight = screenHeight;
			glViewport(0, 0, screenWidth, screenHeight);
			resetProjection();
		}

};

#endif

