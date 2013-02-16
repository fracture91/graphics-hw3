
#ifndef __SCENE_H_
#define __SCENE_H_

#include "LSystemRenderer.hpp"

class Scene {
	private:
		int screenWidth;
		int screenHeight;
		mat4 projection;
		GLuint program;
		
		void resetProjection() {
			if(screenHeight == 0) {
				projection = mat4(); // don't want to divide by zero...
				return;
			}
			projection = mat4()
				* Perspective(90, (float)screenWidth/screenHeight, 0.0000001, 100000)
				* LookAt(vec3(20, 50, 20), vec3(-20, 20, -20), vec3(0, 1, 0));
		}

	public:
		LSystemRenderer& lsysRenderer;
		
		Scene(GLuint program, LSystemRenderer& lr):lsysRenderer(lr) {
			this->program = program;
			resetProjection();
		}

		void bufferPoints() {
			GLsizeiptr totalBytes = lsysRenderer.getTotalBytes();
			glBufferData(GL_ARRAY_BUFFER, totalBytes, NULL, GL_STATIC_DRAW);
			
			lsysRenderer.bufferPoints(0 /*todo*/);
			
			GLuint posLoc = glGetAttribLocation(program, "vPosition");
			glEnableVertexAttribArray(posLoc);
			glVertexAttribPointer(posLoc, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
		}

		void display() {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glEnable(GL_DEPTH_TEST);
			GLuint projLoc = glGetUniformLocationARB(program, "projection_matrix");
			glUniformMatrix4fv(projLoc, 1, GL_TRUE, projection);
			
			lsysRenderer.display();

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

