
#ifndef __LSYSTEMRENDERER_H_
#define __LSYSTEMRENDERER_H_

#include <vector>

#include "LSystem.hpp"
#include "PLYReader.hpp"

using std::vector;



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
			vector<Mesh*> meshes;
			meshes.push_back(cylinder);
			meshes.push_back(sphere);

			GLsizeiptr totalBytes = 0;
			for (vector<Mesh*>::const_iterator i = meshes.begin(); i != meshes.end(); ++i) {
				totalBytes += (*i)->getNumBytes();
			}
			glBufferData(GL_ARRAY_BUFFER, totalBytes, NULL, GL_STATIC_DRAW);

			GLuint start = bufferStart;
			for (vector<Mesh*>::const_iterator i = meshes.begin(); i != meshes.end(); ++i) {
				Mesh* mesh = *i;
				GLsizeiptr bytes = mesh->getNumBytes();
				mesh->setDrawOffset(start / sizeof(mesh->getPoints()[0]));
				glBufferSubData(GL_ARRAY_BUFFER, start, bytes, mesh->getPoints());
				start += bytes;
			}

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
				* LookAt(vec3(20, 300, 20), vec3(0, 0, 0), vec3(0, 1, 0));
		}

		// draw a component of a turtle (sphere or cylinder)
		void drawTurtleComponent(Turtle* turtle, Mesh* comp) {
			bool isCylinder = comp == cylinder;
			vec3 size = comp->getBoundingBox()->getSize();

			// want to scale the cylinder to be segmentLength long, thin
			float thickRatio = turtle->thickness / size.y;
			float zTarget = isCylinder ? turtle->segmentLength
			                           : turtle->thickness;
			mat4 scale = Scale(thickRatio, thickRatio, zTarget / size.z);

			vec4 center = comp->getBoundingBox()->getCenter();
			// move cylinder so its end is flush with the yz plane
			vec4 dest = vec4(0, 0, size.z/2, 1);
			if(!isCylinder) {
				dest.z = 0; // sphere intersects plane
			}
			mat4 trans = Translate(dest - center);

			mat4 finalModel = turtle->ctm->top() * scale * trans;

			// draw next cylinder
			GLuint modelLoc = glGetUniformLocationARB(program, "model_matrix");
			glUniformMatrix4fv(modelLoc, 1, GL_TRUE, finalModel);

			glDrawArrays(GL_TRIANGLES, comp->getDrawOffset(), comp->getNumPoints());
		}

		// draw the turtle in its current state
		void drawTurtle(Turtle* turtle) {
			drawTurtleComponent(turtle, sphere);
			drawTurtleComponent(turtle, cylinder);
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
			
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
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
					drawTurtle(turtle);
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

