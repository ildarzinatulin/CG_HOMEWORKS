// Include standard headers
#include <stdio.h>
#include <stdlib.h>

// Include GLEW библиотека для загрузки расширений
#include <GL/glew.h>

// Include GLFW библиотека для создания графического контекста
#include <glfw3.h>
GLFWwindow* window;

// Include GLM математическая библиотека
#include <glm/glm.hpp>
using namespace glm;

#include <common/shader.hpp>
#include <glm/gtc/matrix_transform.hpp>

int main( void )
{
	// Initialise GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);


	// Open a window and create its OpenGL context
	window = glfwCreateWindow( 1024, 768, "Task 1", NULL, NULL);
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	// Create and compile our GLSL program from the shaders
	GLuint programID1 = LoadShaders("SimpleVertexShader.vertexshader", "SimpleFragmentShader1.fragmentshader");
    GLuint programID2 = LoadShaders("SimpleVertexShader.vertexshader", "SimpleFragmentShader2.fragmentshader");

    // Get a handle for our "MVP" uniform
    GLuint MatrixID1 = glGetUniformLocation(programID1, "MVP");
    GLuint MatrixID2 = glGetUniformLocation(programID2, "MVP");

	static const GLfloat g_vertex_buffer_data[] = { 
		 0.0f,  0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,

         0.0f,  0.5f, 0.0f,
        -0.5f,  0.0f, 0.0f,
         0.5f,  0.0f, 0.0f,
	};

	// Идентификатор буфера вершин
	GLuint vertexbuffer;
	// Создание одного буфера
	glGenBuffers(1, &vertexbuffer);
	// Делаем данный буфер текущим
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	// Передадим информацию о вершинах в OpenGL
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	// Чтобы работал альфа-канал
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Зададим матрицы, чтобы создать эффект движения

    // Матрица проекции
    glm::mat4 Projection = glm::perspective(
            glm::radians(45.0f),     // Вертикальное поле зрения в радианах
            4.0f / 3.0f,                    // Отношение сторон
            0.1f,                            // Ближняя плоскость отсечения. Должна быть больше 0.
            100.0f                            // Дальняя плоскость отсечения
    );
    glm::mat4 View;
    glm::mat4 Model = glm::mat4(1.0f);
    glm::mat4 MVP;

	do{
		// Clear the screen
		glClear( GL_COLOR_BUFFER_BIT );

		View = glm::lookAt(
                glm::vec3(cos(glfwGetTime()) * 5.0f, 0, sin(glfwGetTime()) * 5.0f), //glfwGetTime() - таймер работающий с момента инициализации glfw
                glm::vec3(0, 0, 0), // and looks at the origin
                glm::vec3(0, 1, 0) // Head is up (set to 0,-1,0 to look upside-down)
        );

        MVP = Projection * View * Model;

		// Use our shader
		glUseProgram(programID1);

		//Передаем нашу матрицу
        glUniformMatrix4fv(MatrixID1, 1, GL_FALSE, &MVP[0][0]);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			0, // The attribute we want to configure
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0, 3); // 3 indices starting at 0 -> 1 triangle

        glUseProgram(programID2);
        //Передаем нашу матрицу
        glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
        glDrawArrays(GL_TRIANGLES, 3, 3);

		glDisableVertexAttribArray(0);

		// Swap buffers
        // glfw: обмен содержимым front- и back- буферов. Отслеживание событий ввода\вывода (была ли нажата/отпущена кнопка, перемещен курсор мыши и т.п.)
        glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );


	// Cleanup VBO
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteProgram(programID1);
    glDeleteProgram(programID2);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

