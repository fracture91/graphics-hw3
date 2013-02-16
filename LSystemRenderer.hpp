
#ifndef __LSYSTEMRENDERER_H_
#define __LSYSTEMRENDERER_H_

#include <vector>
#include <stdlib.h>

#include "LSystem.hpp"
#include "PLYReader.hpp"

using std::vector;



class LSystemRenderer {
	private:
		GLuint program;
		GLuint bufferStart;
		mat4 projection;
		vector<LSystem*>& allSystems;
		vector<LSystem*> systemsToDraw;
		vector<vec4> colors;
		vector<vec4> startPoints;
		vec4 randomRange[2];

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
				* LookAt(vec3(20, 50, 20), vec3(-20, 20, -20), vec3(0, 1, 0));
		}

		// draw a component of a turtle (sphere or cylinder)
		void drawTurtleComponent(Turtle* turtle, Mesh* comp) {
			bool isCylinder = comp == cylinder;
			vec3 size = comp->getBoundingBox()->getSize();

			// want to scale the cylinder to be segmentLength long, thin
			// sphere is scaled uniformly
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
			GLuint modelLoc = glGetUniformLocationARB(program, "model_matrix");
			glUniformMatrix4fv(modelLoc, 1, GL_TRUE, finalModel);

			// draw the component
			glDrawArrays(GL_TRIANGLES, comp->getDrawOffset(), comp->getNumPoints());
		}

		vec4 randomColor() {
			vec4 color(0, 0, 0, 1);
			for(int i = 0; i < 3; i++) {
				color[i] = (float)rand() / RAND_MAX;
			}
			return color;
		}

		// return a random point in the rectangle defined by randomRange
		vec4 randomPoint() {
			vec4 point(0, 0, 0, 1);
			for(int i = 0; i < 3; i++) {
				float lower = randomRange[0][i];
				float upper = randomRange[1][i];
				point[i] = lower + (float)rand() / ((float)RAND_MAX / (upper - lower));
			}
			return point;
		}

		// draw the given lsystem starting at the given position
		void drawSystem(LSystem* sys, vec4 startPoint, vec4 color) {
			Turtle* turtle = sys->getTurtleCopy();
			stack<mat4> modelView;
			// move to start point and point the tree upwards
			modelView.push(Translate(startPoint) * RotateX(-90));
			turtle->ctm = &modelView;
			string turtleString = sys->getTurtleString();

			GLuint colorLoc = glGetUniformLocationARB(program, "inColor");
			glUniform4fv(colorLoc, 1, color);

			for(string::iterator it = turtleString.begin(); it != turtleString.end(); ++it) {
				char currentChar = *it;
				
				if(currentChar == 'F') {
					drawTurtleComponent(turtle, sphere);
					drawTurtleComponent(turtle, cylinder);
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

			delete turtle;

		}

	public:
		LSystemRenderer(GLuint program, GLuint bufferStart, vector<LSystem*>& allSystems)
				: allSystems(allSystems) {
			this->program = program;
			this->bufferStart = bufferStart;
			screenWidth = 0;
			screenHeight = 0;
			PLYReader sphereReader("meshes/sphere.ply");
			sphere = sphereReader.read();
			PLYReader cylinderReader("meshes/cylinder.ply");
			cylinder = cylinderReader.read();
			showOneSystem(0);
			resetProjection();
			bufferPoints();
		}

		void display() {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glEnable(GL_DEPTH_TEST);
			GLuint projLoc = glGetUniformLocationARB(program, "projection_matrix");
			glUniformMatrix4fv(projLoc, 1, GL_TRUE, projection);

			vec4 startPoint(0, 0, 0, 1);
			for (vector<LSystem*>::const_iterator i = systemsToDraw.begin(); i != systemsToDraw.end(); ++i) {
				int index = i - systemsToDraw.begin();
				drawSystem(*i, startPoints[index], colors[index]);
			}

			glDisable(GL_DEPTH_TEST); 

			// output to hardware, double buffered
			glFlush();
			glutSwapBuffers();
		}

		void showOneSystem(int index) {
			systemsToDraw.clear();
			systemsToDraw.push_back(allSystems[index]);
			startPoints.clear();
			startPoints.push_back(vec4(0, 0, 0, 1));
			colors.clear();
			colors.push_back(randomColor());
		}

		void showAllSystemsRandomly(vec4 min, vec4 max) {
			randomRange[0] = min;
			randomRange[1] = max;
			systemsToDraw.clear();
			startPoints.clear();
			colors.clear();
			for (vector<LSystem*>::const_iterator sys = allSystems.begin(); sys != allSystems.end(); ++sys) {
				systemsToDraw.push_back(*sys);
				startPoints.push_back(randomPoint());
				colors.push_back(randomColor());
			}
		}

		void reshape(int screenWidth, int screenHeight) {
			this->screenWidth = screenWidth;
			this->screenHeight = screenHeight;
			glViewport(0, 0, screenWidth, screenHeight);
			resetProjection();
		}

};

#endif

