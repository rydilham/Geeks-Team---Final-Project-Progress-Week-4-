#include "Demo.h"
#include "math.h";


Demo::Demo() {

}


Demo::~Demo() {
}



void Demo::Init() {
	// build and compile our shader program
	// ------------------------------------
	shaderProgram = BuildShader("vertexShader.vert", "fragmentShader.frag", nullptr);
	BuildTexturedCube();
	BuildTexturedPlane();
	InitCamera();
}

void Demo::DeInit() {
	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteVertexArrays(1, &VAO2);
	glDeleteBuffers(1, &VBO2);
	glDeleteBuffers(1, &EBO2);
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void Demo::ProcessInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	// zoom camera
	// -----------
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
		if (fovy < 90) {
			fovy += 0.0001f;
		}
	}

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		if (fovy > 0) {
			fovy -= 0.0001f;
		}
	}

	// update camera movement 
	// -------------
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		MoveCamera(CAMERA_SPEED);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		MoveCamera(-CAMERA_SPEED);
	}

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		StrafeCamera(-CAMERA_SPEED);
	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		StrafeCamera(CAMERA_SPEED);
	}

	// update camera rotation
	// ----------------------
	double mouseX, mouseY;
	double midX = screenWidth / 2;
	double midY = screenHeight / 2;
	float angleY = 0.0f;
	float angleZ = 0.0f;

	// Get mouse position
	glfwGetCursorPos(window, &mouseX, &mouseY);
	if ((mouseX == midX) && (mouseY == midY)) {
		return;
	}

	// Set mouse position
	glfwSetCursorPos(window, midX, midY);

	// Get the direction from the mouse cursor, set a resonable maneuvering speed
	angleY = (float)((midX - mouseX)) / 1000;
	angleZ = (float)((midY - mouseY)) / 1000;

	// The higher the value is the faster the camera looks around.
	viewCamY += angleZ * 2;

	// limit the rotation around the x-axis
	if ((viewCamY - posCamY) > 8) {
		viewCamY = posCamY + 8;
	}
	if ((viewCamY - posCamY) < -8) {
		viewCamY = posCamY - 8;
	}
	RotateCamera(-angleY);

}

void Demo::Update(double deltaTime) {
}

void Demo::Render() {
	/*glViewport(0, 0, this->screenWidth, this->screenHeight);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glEnable(GL_DEPTH_TEST);

	// Pass perspective projection matrix
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)this->screenWidth / (GLfloat)this->screenHeight, 0.1f, 100.0f);
	GLint projLoc = glGetUniformLocation(this->shaderProgram, "projection");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	// LookAt camera (position, target/direction, up)
	glm::vec3 viewPos = glm::vec3(1, 1, 1);
	glm::mat4 view = glm::lookAt(viewPos, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	GLint viewLoc = glGetUniformLocation(this->shaderProgram, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	*/

	glViewport(0, 0, this->screenWidth, this->screenHeight);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glEnable(GL_DEPTH_TEST);

	// Pass perspective projection matrix
	glm::mat4 projection = glm::perspective(fovy, (GLfloat)this->screenWidth / (GLfloat)this->screenHeight, 0.1f, 100.0f);
	GLint projLoc = glGetUniformLocation(this->shaderProgram, "projection");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	// LookAt camera (position, target/direction, up)
	glm::vec3 viewPos = glm::vec3(1, 1, 1);
	glm::mat4 view = glm::lookAt(glm::vec3(posCamX, posCamY, posCamZ), glm::vec3(viewCamX, viewCamY, viewCamZ), glm::vec3(upCamX, upCamY, upCamZ));
	GLint viewLoc = glGetUniformLocation(this->shaderProgram, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	// set lighting attribute
	GLint lightPosLoc = glGetUniformLocation(this->shaderProgram, "lightPos");
	glUniform3f(lightPosLoc, 0, 10, 0);
	GLint viewPosLoc = glGetUniformLocation(this->shaderProgram, "viewPos");
	glUniform3f(viewPosLoc, viewPos.x, viewPos.y, viewPos.z);
	GLint lightColorLoc = glGetUniformLocation(this->shaderProgram, "lightColor");
	glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);

	DrawBuilding();
	DrawStreet();
	DrawYard();
	DrawTexturedCube();

	DrawTexturedPlane();

	glDisable(GL_DEPTH_TEST);
}

void Demo::BuildTexturedCube() {
	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float vertices[] = {
		// format position, normals
		// front
		-0.5, -0.5, 0.5,  0.0f,  0.0f,  1.0f, // 0
		0.5, -0.5, 0.5,   0.0f,  0.0f,  1.0f, // 1
		0.5,  0.5, 0.5,   0.0f,  0.0f,  1.0f, // 2
		-0.5,  0.5, 0.5,  0.0f,  0.0f,  1.0f, // 3

		// right
		0.5,  0.5,  0.5, 1.0f,  0.0f,  0.0f, // 4
		0.5,  0.5, -0.5, 1.0f,  0.0f,  0.0f, // 5
		0.5, -0.5, -0.5, 1.0f,  0.0f,  0.0f, // 6
		0.5, -0.5,  0.5, 1.0f,  0.0f,  0.0f, // 7

		// back
		-0.5, -0.5, -0.5, 0.0f,  0.0f,  -1.0f, // 8 
		0.5,  -0.5, -0.5, 0.0f,  0.0f,  -1.0f, // 9
		0.5,   0.5, -0.5, 0.0f,  0.0f,  -1.0f, // 10
		-0.5,  0.5, -0.5, 0.0f,  0.0f,  -1.0f, // 11

		// left
		-0.5, -0.5, -0.5, -1.0f,  0.0f,  0.0f, // 12
		-0.5, -0.5,  0.5, -1.0f,  0.0f,  0.0f, // 13
		-0.5,  0.5,  0.5, -1.0f,  0.0f,  0.0f, // 14
		-0.5,  0.5, -0.5, -1.0f,  0.0f,  0.0f, // 15

		// upper
		0.5, 0.5,  0.5, 0.0f,  1.0f,  0.0f, // 16
		-0.5, 0.5, 0.5, 0.0f,  1.0f,  0.0f, // 17
		-0.5, 0.5, -0.5,0.0f,  1.0f,  0.0f, // 18
		0.5, 0.5, -0.5, 0.0f,  1.0f,  0.0f, // 19

		// bottom
		-0.5, -0.5, -0.5, 0.0f,  -1.0f,  0.0f, // 20
		0.5, -0.5, -0.5,  0.0f,  -1.0f,  0.0f, // 21
		0.5, -0.5,  0.5,  0.0f,  -1.0f,  0.0f, // 22
		-0.5, -0.5,  0.5, 0.0f,  -1.0f,  0.0f, // 23
	};

	unsigned int indices[] = {
		0,  1,  2,  0,  2,  3,   // front
		4,  5,  6,  4,  6,  7,   // right
		8,  9,  10, 8,  10, 11,  // back
		12, 14, 13, 12, 15, 14,  // left
		16, 18, 17, 16, 19, 18,  // upper
		20, 22, 21, 20, 23, 22   // bottom
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// define position pointer layout 0
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(0 * sizeof(GLfloat)));
	glEnableVertexAttribArray(0);

	// define normal pointer layout 2
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
	// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	glBindVertexArray(0);

	// remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}

static void tree(GLint objectColorLoc, GLint modelLoc, float treeX, float treeZ) {
	glUniform3f(objectColorLoc, 71 / 255.0f, 31 / 255.0f, 1 / 255.0f);

	glm::mat4 model;
	model = glm::translate(model, glm::vec3(treeX, 1, treeZ));
	model = glm::scale(model, glm::vec3(0.3, 2, 0.3));

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	for (int i = 0; i < 6; i++) {
		if (i == 0) {
			glUniform3f(objectColorLoc, 0.0f, 1.0f, 0.0f);

			glm::mat4 model2;
			model2 = glm::translate(model2, glm::vec3(treeX, 2, treeZ));
			model2 = glm::scale(model2, glm::vec3(1, 1, 1));

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model2));

			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		}
		for (int j = 0; j < 3; j++) {
			glUniform3f(objectColorLoc, 0.0f, 1.0f, 0.0f);

			glm::mat4 model2;
			model2 = glm::translate(model2, glm::vec3(treeX, 2, treeZ));
			if (j == 0) {
				model2 = glm::scale(model2, glm::vec3(1 + 0.1 * i, 1 - 0.1 * i, 1 - 0.1 * i));
			}
			else if (j == 1) {
				model2 = glm::scale(model2, glm::vec3(1 - 0.1 * i, 1 + 0.1 * i, 1 - 0.1 * i));
			}
			else {
				model2 = glm::scale(model2, glm::vec3(1 - 0.1 * i, 1 - 0.1 * i, 1 + 0.1 * i));
			}

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model2));

			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		}
	}

}

static void hedge(GLint objectColorLoc, GLint modelLoc, float hedgelocX, float hedgelocZ, int hedgecount) {
	glUniform3f(objectColorLoc, 0.0f, 1.0f, 0.0f);

	glm::mat4 model3;
	model3 = glm::translate(model3, glm::vec3(hedgelocX, 0.3, hedgelocZ));
	model3 = glm::scale(model3, glm::vec3(hedgecount * 0.3, 0.4, 0.3));

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model3));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	for (int i = 0; i < floor(hedgecount / 2); i++) {
		if (hedgecount % 2 > 0) {
			if (i == 0) {
				glUniform3f(objectColorLoc, 71 / 255.0f, 31 / 255.0f, 1 / 255.0f);

				glm::mat4 model4;
				model4 = glm::translate(model4, glm::vec3(hedgelocX, 0.05, hedgelocZ));
				model4 = glm::scale(model4, glm::vec3(0.05, 0.1, 0.05));

				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model4));

				glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
			}
			else {
				for (int j = 0; j < 2; j++) {
					if (j == 0) {
						glUniform3f(objectColorLoc, 71 / 255.0f, 31 / 255.0f, 1 / 255.0f);

						glm::mat4 model4;
						model4 = glm::translate(model4, glm::vec3(hedgelocX + 0.3 * i, 0.05, hedgelocZ));
						model4 = glm::scale(model4, glm::vec3(0.05, 0.1, 0.05));

						glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model4));

						glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
					}
					else {
						glUniform3f(objectColorLoc, 71 / 255.0f, 31 / 255.0f, 1 / 255.0f);

						glm::mat4 model4;
						model4 = glm::translate(model4, glm::vec3(hedgelocX - 0.3 * i, 0.05, hedgelocZ));
						model4 = glm::scale(model4, glm::vec3(0.05, 0.1, 0.05));

						glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model4));

						glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
					}
				}
			}
		}
		else {
			for (int j = 0; j < 2; j++) {
				if (j == 0) {
					glUniform3f(objectColorLoc, 71 / 255.0f, 31 / 255.0f, 1 / 255.0f);

					glm::mat4 model4;
					model4 = glm::translate(model4, glm::vec3(hedgelocX + 0.15 + 0.3 * i, 0.05, hedgelocZ));
					model4 = glm::scale(model4, glm::vec3(0.05, 0.1, 0.05));

					glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model4));

					glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
				}
				else {
					glUniform3f(objectColorLoc, 71 / 255.0f, 31 / 255.0f, 1 / 255.0f);

					glm::mat4 model4;
					model4 = glm::translate(model4, glm::vec3(hedgelocX - 0.15 - 0.3 * i, 0.05, hedgelocZ));
					model4 = glm::scale(model4, glm::vec3(0.05, 0.1, 0.05));

					glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model4));

					glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
				}
			}
		}
	}
}

static void car(GLint objectColorLoc, GLint modelLoc, float carX, float carZ) {
	//mobil
	glUniform3f(objectColorLoc, 1.0f, 0.0f, 0.0f);
	float carloc[2] = { carX ,carZ };
	//body
	glm::mat4 model5;
	model5 = glm::translate(model5, glm::vec3(carloc[0], 0.45, carloc[1]));
	model5 = glm::scale(model5, glm::vec3(1.1, 0.6, 3));

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model5));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	//uppercar
	glm::mat4 model6;
	model6 = glm::translate(model6, glm::vec3(carloc[0], 0.95, carloc[1] + 0.25));
	model6 = glm::scale(model6, glm::vec3(1.1, 0.4, 1.8));

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model6));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	float wheloc[2] = { carloc[1] - 1, carloc[1] + 0.73 };

	for (int j = 0; j < 2; j++) {
		float weloc = 0;
		for (int i = 0; i < 5; i++) {
			glUniform3f(objectColorLoc, 32 / 255.0f, 32 / 255.0f, 32 / 255.0f);
			glm::mat4 model7;
			if (i == 2) {
				model7 = glm::translate(model7, glm::vec3(carloc[0] + 0.55, 0.21, wheloc[j] + 0.06 + weloc));
				model7 = glm::scale(model7, glm::vec3(0.1, 0.42, 0.18));
				weloc += 0.18;
			}
			else if (i % 2 == 1) {
				model7 = glm::translate(model7, glm::vec3(carloc[0] + 0.55, 0.21, wheloc[j] + weloc));
				model7 = glm::scale(model7, glm::vec3(0.1, 0.3, 0.06));
				weloc += 0.06;
			}
			else {
				model7 = glm::translate(model7, glm::vec3(carloc[0] + 0.55, 0.21, wheloc[j] + weloc));
				model7 = glm::scale(model7, glm::vec3(0.1, 0.18, 0.06));
				weloc += 0.06;
			}

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model7));

			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		}
		weloc = 0;
		for (int i = 0; i < 5; i++) {
			glUniform3f(objectColorLoc, 32 / 255.0f, 32 / 255.0f, 32 / 255.0f);
			glm::mat4 model7;
			if (i == 2) {
				model7 = glm::translate(model7, glm::vec3(carloc[0] - 0.55, 0.21, wheloc[j] + 0.06 + weloc));
				model7 = glm::scale(model7, glm::vec3(0.1, 0.42, 0.18));
				weloc += 0.18;
			}
			else if (i % 2 == 1) {
				model7 = glm::translate(model7, glm::vec3(carloc[0] - 0.55, 0.21, wheloc[j] + weloc));
				model7 = glm::scale(model7, glm::vec3(0.1, 0.3, 0.06));
				weloc += 0.06;
			}
			else {
				model7 = glm::translate(model7, glm::vec3(carloc[0] - 0.55, 0.21, wheloc[j] + weloc));
				model7 = glm::scale(model7, glm::vec3(0.1, 0.18, 0.06));
				weloc += 0.06;
			}

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model7));

			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		}
	}
	//headlight
	for (int i = 0; i > 2; i++) {
		glUniform3f(objectColorLoc, 252 / 255.0f, 252 / 255.0f, 0 / 255.0f);
		glm::mat4 headlight;
		headlight = glm::translate(headlight, glm::vec3(carloc[0] + 0.56 * pow(-1, i), 0.649, carloc[1] - 1.6));//carloc[0], 0.45, carloc[1]
		headlight = glm::scale(headlight, glm::vec3(0.44, 0.18, 0.1)); //1.1, 0.6, 3

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(headlight));

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}

	//brakes
	//carplate
	//windshield
}

static void fence(GLint objectColorLoc, GLint modelLoc, float fenceX, float fenceZ, float fencelength) {
	float fenceloc[2] = { fenceX, fenceZ };
	float wallmid = fencelength / 2;

	glUniform3f(objectColorLoc, 0.0f, 1.0f, 1.0f);

	glm::mat4 model9;
	model9 = glm::translate(model9, glm::vec3(fenceloc[0], 0.45, fenceloc[1]));
	model9 = glm::scale(model9, glm::vec3(0.4, 0.9, 0.4));

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model9));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	//deco
	glUniform3f(objectColorLoc, 1.0f, 1.0f, 1.0f);
	for (int j = 0; j < 3; j++) {

		glm::mat4 model8;
		model8 = glm::translate(model8, glm::vec3(fenceloc[0], 0.15 - 0.05 * j, fenceloc[1]));
		model8 = glm::scale(model8, glm::vec3(0.5 + j * 0.05, 0.05, 0.5 + j * 0.05));

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model8));

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

		glm::mat4 model9;
		model9 = glm::translate(model9, glm::vec3(fenceloc[0], 0.9, fenceloc[1]));

		model9 = glm::scale(model9, glm::vec3(0.5 - j * 0.05, 0.15 + j * 0.05, 0.5 - j * 0.05));

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model9));

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}

	//wall
	glUniform3f(objectColorLoc, 0.0f, 1.0f, 1.0f);

	glm::mat4 model10;
	model10 = glm::translate(model10, glm::vec3(fenceloc[0] + wallmid, 0.35, fenceloc[1]));
	model10 = glm::scale(model10, glm::vec3(fencelength, 0.7, 0.3));

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model10));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


}
void Demo::DrawBuilding()
{
	UseShader(shaderProgram);

	glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized

	GLint objectColorLoc = glGetUniformLocation(this->shaderProgram, "objectColor");
	glUniform3f(objectColorLoc, 135.0 / 255.0f, 206.0 / 255.0f, 250.0 / 255.0f);


	//pintu 1
	glm::mat4 model11;
	model11 = glm::translate(model11, glm::vec3(5.44, 0.9, 1.74));
	model11 = glm::scale(model11, glm::vec3(1.2, 1.8, 0));

	GLint modelLoc = glGetUniformLocation(this->shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model11));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	//bangunan 1
	for (int i = 0;i < 4;i++) {
		objectColorLoc = glGetUniformLocation(this->shaderProgram, "objectColor");
		glUniform3f(objectColorLoc, 160.0 / 255.0f, 82.0 / 255.0f, 45.0 / 255.0f);

		glm::mat4 model12;
		model12 = glm::translate(model12, glm::vec3(5.5, 3 + i * 0, 4));
		model12 = glm::scale(model12, glm::vec3(4.5, 6, 4.5));

		glGetUniformLocation(this->shaderProgram, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model12));

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}
	//jendela 1.1
	for (int i = 0;i < 2;i++) {
		objectColorLoc = glGetUniformLocation(this->shaderProgram, "objectColor");
		glUniform3f(objectColorLoc, 135.0 / 255.0f, 206.0 / 255.0f, 250.0 / 255.0f);

		glm::mat4 model13;
		model13 = glm::translate(model13, glm::vec3(4.1, 3 + i * 1.9, 1.74));
		model13 = glm::scale(model13, glm::vec3(1, 1, 0));

		glGetUniformLocation(this->shaderProgram, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model13));

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}
	//jendela 1.2
	for (int i = 0;i < 2;i++) {
		objectColorLoc = glGetUniformLocation(this->shaderProgram, "objectColor");
		glUniform3f(objectColorLoc, 135.0 / 255.0f, 206.0 / 255.0f, 250.0 / 255.0f);

		glm::mat4 model14;
		model14 = glm::translate(model14, glm::vec3(6.9, 3 + i * 1.9, 1.74));
		model14 = glm::scale(model14, glm::vec3(1, 1, 0));

		glGetUniformLocation(this->shaderProgram, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model14));

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}
	//jendela 2.1
	for (int i = 0;i < 3;i++) {
		objectColorLoc = glGetUniformLocation(this->shaderProgram, "objectColor");
		glUniform3f(objectColorLoc, 135.0 / 255.0f, 206.0 / 255.0f, 250.0 / 255.0f);

		glm::mat4 model15;
		model15 = glm::translate(model15, glm::vec3(3.24, 1.1 + i * 1.9, 5.4));
		model15 = glm::scale(model15, glm::vec3(0, 1, 1));

		glGetUniformLocation(this->shaderProgram, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model15));

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}
	//jendela 2.2
	for (int i = 0;i < 3;i++) {
		objectColorLoc = glGetUniformLocation(this->shaderProgram, "objectColor");
		glUniform3f(objectColorLoc, 135.0 / 255.0f, 206.0 / 255.0f, 250.0 / 255.0f);

		glm::mat4 model16;
		model16 = glm::translate(model16, glm::vec3(3.24, 1.1 + i * 1.9, 2.6));
		model16 = glm::scale(model16, glm::vec3(0, 1, 1));

		glGetUniformLocation(this->shaderProgram, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model16));

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}

	//jendela 3.1
	for (int i = 0;i < 3;i++) {
		objectColorLoc = glGetUniformLocation(this->shaderProgram, "objectColor");
		glUniform3f(objectColorLoc, 135.0 / 255.0f, 206.0 / 255.0f, 250.0 / 255.0f);

		glm::mat4 model18;
		model18 = glm::translate(model18, glm::vec3(7.76, 1.1 + i * 1.9, 5.4));
		model18 = glm::scale(model18, glm::vec3(0, 1, 1));

		glGetUniformLocation(this->shaderProgram, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model18));

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}
	//jendela 3.2
	for (int i = 0;i < 3;i++) {
		objectColorLoc = glGetUniformLocation(this->shaderProgram, "objectColor");
		glUniform3f(objectColorLoc, 135.0 / 255.0f, 206.0 / 255.0f, 250.0 / 255.0f);

		glm::mat4 model19;
		model19 = glm::translate(model19, glm::vec3(7.76, 1.1 + i * 1.9, 2.6));
		model19 = glm::scale(model19, glm::vec3(0, 1, 1));

		glGetUniformLocation(this->shaderProgram, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model19));

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}
	glBindVertexArray(0);
}
void Demo::DrawStreet()
{
	UseShader(shaderProgram);

	glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized

	GLint objectColorLoc = glGetUniformLocation(this->shaderProgram, "objectColor");
	glUniform3f(objectColorLoc, 0.0f, 0.0f, 0.0f);


	
	glm::mat4 model20;
	model20 = glm::translate(model20, glm::vec3(5, 0, -4));
	model20 = glm::scale(model20, glm::vec3(20, 0.1, 3.2));

	GLint modelLoc = glGetUniformLocation(this->shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model20));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	//garis
	for (int i = 0;i < 11;i++) {
		objectColorLoc = glGetUniformLocation(this->shaderProgram, "objectColor");
		glUniform3f(objectColorLoc, 1.0f, 1.0f, 1.0f);

		glm::mat4 model21;
		
		model21 = glm::translate(model21, glm::vec3(14-i*1.8, 0.1, -4));
		/*model21 = glm::rotate(model21,angle, glm::vec3(0,1,0));*/
		model21 = glm::scale(model21, glm::vec3(0.9, 0, 0.1));

		glGetUniformLocation(this->shaderProgram, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model21));

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}
	glBindVertexArray(0);
}
void Demo::DrawYard()
{
	UseShader(shaderProgram);

	glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized

	GLint objectColorLoc = glGetUniformLocation(this->shaderProgram, "objectColor");
	glUniform3f(objectColorLoc, 0.0 / 255.0f, 185.0 / 255.0f, 0.0 / 255.0f);

	glm::mat4 model22;
	model22 = glm::translate(model22, glm::vec3(5, 0, 5));
	model22 = glm::scale(model22, glm::vec3(20, 0, 15));

	GLint modelLoc = glGetUniformLocation(this->shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model22));

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	glBindTexture(GL_TEXTURE_2D, 0);




	//garis
	for (int i = 0;i < 11;i++) {
		objectColorLoc = glGetUniformLocation(this->shaderProgram, "objectColor");
		glUniform3f(objectColorLoc, 1.0f, 1.0f, 1.0f);

		glm::mat4 model21;

		model21 = glm::translate(model21, glm::vec3(14 - i * 1.8, 0.1, -4));
		/*model21 = glm::rotate(model21,angle, glm::vec3(0,1,0));*/
		model21 = glm::scale(model21, glm::vec3(0.9, 0, 0.1));

		glGetUniformLocation(this->shaderProgram, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model21));

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}



	//tugu
	for (int i = 0;i < 1;i++) {
		objectColorLoc = glGetUniformLocation(this->shaderProgram, "objectColor");
		glUniform3f(objectColorLoc, 1.0f, 1.0f, 1.0f);

		glm::mat4 model21;

		model21 = glm::translate(model21, glm::vec3(12, 1.5+i*2, 1));
		/*model21 = glm::rotate(model21,angle, glm::vec3(0,1,0));*/
		model21 = glm::scale(model21, glm::vec3(0.7, 2.5, 0.7));

		glGetUniformLocation(this->shaderProgram, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model21));

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}
	//alas tugu
	for (int i = 0;i < 1;i++) {
		objectColorLoc = glGetUniformLocation(this->shaderProgram, "objectColor");
		glUniform3f(objectColorLoc, 1.0f, 1.0f, 1.0f);

		glm::mat4 model21;

		model21 = glm::translate(model21, glm::vec3(12, 0.15 + i * 2, 1));
		/*model21 = glm::rotate(model21,angle, glm::vec3(0,1,0));*/
		model21 = glm::scale(model21, glm::vec3(1.5, 0.3, 1.5));

		glGetUniformLocation(this->shaderProgram, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model21));

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}
	//atap tugu1
	for (int i = 0;i < 1;i++) {
		objectColorLoc = glGetUniformLocation(this->shaderProgram, "objectColor");
		glUniform3f(objectColorLoc, 1.0f, 1.0f, 1.0f);

		glm::mat4 model21;

		model21 = glm::translate(model21, glm::vec3(12, 3.1 + i * 2, 1));
		/*model21 = glm::rotate(model21,angle, glm::vec3(0,1,0));*/
		model21 = glm::scale(model21, glm::vec3(0.5, 0.2, 0.5));

		glGetUniformLocation(this->shaderProgram, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model21));

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}
	//atap tugu2
	for (int i = 0;i < 1;i++) {
		objectColorLoc = glGetUniformLocation(this->shaderProgram, "objectColor");
		glUniform3f(objectColorLoc, 1.0f, 1.0f, 1.0f);

		glm::mat4 model21;

		model21 = glm::translate(model21, glm::vec3(12, 3 + i * 2, 1));
		/*model21 = glm::rotate(model21,angle, glm::vec3(0,1,0));*/
		model21 = glm::scale(model21, glm::vec3(0.1, 1, 0.1));

		glGetUniformLocation(this->shaderProgram, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model21));

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}
	glBindVertexArray(0);
}

//static void gedung(GLint objectColorLoc, GLint modelLoc, float gedungX, float gedungZ) {
//	//mobil
//	glUniform3f(objectColorLoc, 1.0f, 0.0f, 0.0f);
//	float gedungloc[2] = { gedungX ,gedungZ };
//	//body
//	glm::mat4 model11;
//	model11 = glm::translate(model11, glm::vec3(gedungloc[0], 0.75, gedungloc[1]));
//	model11 = glm::scale(model11, glm::vec3(1.1, 0.6, 3));
//
//	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model11));
//
//	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
//
//
//	}


void Demo::DrawTexturedCube()
{
	UseShader(shaderProgram);

	glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized

	GLint objectColorLoc = glGetUniformLocation(this->shaderProgram, "objectColor");
	GLint modelLoc = glGetUniformLocation(this->shaderProgram, "model");

	tree(objectColorLoc, modelLoc, 0, 0); // posX, posY

	hedge(objectColorLoc, modelLoc, 1, 1, 10); //posX, posY, hedgecount

	car(objectColorLoc, modelLoc, 0, 3);

	fence(objectColorLoc, modelLoc, -1, -2, 10);



	glBindVertexArray(0);

}

void Demo::BuildTexturedPlane()
{
	// Build geometry
	GLfloat vertices[] = {
		// format position, normals
		// bottom
		-50.0, -0.5, -50.0,  0.0f,  1.0f,  0.0f,
		 50.0, -0.5, -50.0,  0.0f,  1.0f,  0.0f,
		 50.0, -0.5,  50.0,  0.0f,  1.0f,  0.0f,
		-50.0, -0.5,  50.0,  0.0f,  1.0f,  0.0f,


	};

	GLuint indices[] = { 0,  2,  1,  0,  3,  2 };

	glGenVertexArrays(1, &VAO2);
	glGenBuffers(1, &VBO2);
	glGenBuffers(1, &EBO2);

	glBindVertexArray(VAO2);

	glBindBuffer(GL_ARRAY_BUFFER, VBO2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO2);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(0);
	// Normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0); // Unbind VAO
}

void Demo::DrawTexturedPlane()
{
	UseShader(shaderProgram);

	glBindVertexArray(VAO2); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized

	GLint objectColorLoc = glGetUniformLocation(this->shaderProgram, "objectColor");
	glUniform3f(objectColorLoc, 0.8f, 0.8f, 0.8f);

	glm::mat4 model;
	GLint modelLoc = glGetUniformLocation(this->shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
}


void Demo::InitCamera()
{
	posCamX = 0.0f;
	posCamY = 1.0f;
	posCamZ = 8.0f;
	viewCamX = 0.0f;
	viewCamY = 1.0f;
	viewCamZ = 0.0f;
	upCamX = 0.0f;
	upCamY = 1.0f;
	upCamZ = 0.0f;
	CAMERA_SPEED = 0.0001f;
	fovy = 45.0f;
	glfwSetInputMode(this->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}


void Demo::MoveCamera(float speed)
{
	float x = viewCamX - posCamX;
	float z = viewCamZ - posCamZ;
	// forward positive cameraspeed and backward negative -cameraspeed.
	posCamX = posCamX + x * speed;
	posCamZ = posCamZ + z * speed;
	viewCamX = viewCamX + x * speed;
	viewCamZ = viewCamZ + z * speed;
}

void Demo::StrafeCamera(float speed)
{
	float x = viewCamX - posCamX;
	float z = viewCamZ - posCamZ;
	float orthoX = -z;
	float orthoZ = x;

	// left positive cameraspeed and right negative -cameraspeed.
	posCamX = posCamX + orthoX * speed;
	posCamZ = posCamZ + orthoZ * speed;
	viewCamX = viewCamX + orthoX * speed;
	viewCamZ = viewCamZ + orthoZ * speed;
}

void Demo::RotateCamera(float speed)
{
	float x = viewCamX - posCamX;
	float z = viewCamZ - posCamZ;
	viewCamZ = (float)(posCamZ + glm::sin(speed) * x + glm::cos(speed) * z);
	viewCamX = (float)(posCamX + glm::cos(speed) * x - glm::sin(speed) * z);
}

int main(int argc, char** argv) {
	RenderEngine& app = Demo();
	app.Start("Project Team Geeks", 800, 600, false, true);
}