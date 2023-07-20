#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>

#include <iostream>

#include "Renderer/ShaderProgram.h"
#include "Resources/ResourceManager.h"
#include "Renderer/Texture2D.h"

GLfloat point[] =
{
    0.0f,0.5f,0.0f,
    0.5f,-0.5f,0.0f,
    -0.5f,-0.5f,0.0f
};

GLfloat colors[] =
{
    1.0f,0.0f,0.0f,
    0.0f,1.0f,0.0f,
    0.0f,0.0f,1.0f
};

GLfloat texCord[] =
{
    0.5f,1.0f,
    1.0f,0.0f,
    0.0f,0.0f
};


glm::ivec2 g_windowSize(640, 480);

void glfwWindowSizeCallback(GLFWwindow* pWindow, int width, int height)
{
    g_windowSize.x = width;
    g_windowSize.y = height;

    glViewport(0, 0, width, height);
}

void glfwKeyCallback(GLFWwindow* pWindow, int key, int scancode, int action, int mode) 
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(pWindow, GL_TRUE);
    }
}

int main(int arg, char** argv)
{
    /* Initialize the library */
    if (!glfwInit())
    {
        std::cout << "glfwInit failed!" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    /* Create a windowed mode window and its OpenGL context */
    GLFWwindow* pWindow = glfwCreateWindow(g_windowSize.x, g_windowSize.y, "Battle City", nullptr, nullptr);
    if (!pWindow)
    {
        std::cout << "glfwCreateWindow (pWindow) failed!" << std::endl;
        glfwTerminate();
        return -1;
    }

    //Регистрация callback функций
    glfwSetWindowSizeCallback(pWindow, glfwWindowSizeCallback);
    glfwSetKeyCallback(pWindow, glfwKeyCallback);

    glfwMakeContextCurrent(pWindow);

    if(!gladLoadGL())
    {
        std::cout << "Can't load GLAD!" << std::endl;
        return -1;
    }

    //Вывод в консоль информации отладки
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;

    glClearColor(1,1,0,1);

    {
        ResourceManager resourceManager(argv[0]);
        auto pDefaultShaderProgram = resourceManager.loadShaders("DefaultShaders", "res/shaders/vertex.txt", "res/shaders/fragment.txt");
        if (!pDefaultShaderProgram)
        {
            std::cerr << "Can't create shader program: " << "DefaultShader" << std::endl;
            return -1;
        }

        auto tex = resourceManager.loadTexture("DefaultTexture", "res/textures/map_16x16.png");

        //Создание и подключение буффера
        GLuint points_vbo = 0;
        glGenBuffers(1, &points_vbo);

        //Сделали буффер текущим
        glBindBuffer(GL_ARRAY_BUFFER, points_vbo);

        //Заполняем буффер информацией. Выполняется для текущего буффера
        // GL_STATIC_DRAW - подсказка для драйвера. Служит для отрисовки статичного объекта
        // GL_DYNAMIC_DRAW - если данные об объекте будут частно меняться (перемещение, анимация и т.д.)
        glBufferData(GL_ARRAY_BUFFER, sizeof(point), point, GL_STATIC_DRAW);

        //Аналогичное подключение для массива цвета
        GLuint colors_vbo = 0;
        glGenBuffers(1, &colors_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, colors_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);

        GLuint texCords_vbo = 0;
        glGenBuffers(1, &texCords_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, texCords_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(texCord), texCord, GL_STATIC_DRAW);

        //Создание Vertex Array Object для того, чтобы данные о шейдерах, загруженные в видеокарту правильно обрабатывались
        GLuint vao = 0;
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao); //Аналогично работе с vbo

        //Связываем vbo & vao
        glEnableVertexAttribArray(0); //Включение нулевой позиции в шейдере
        glBindBuffer(GL_ARRAY_BUFFER, points_vbo); //Переключаемся на буфер с точками, делаем его текущим
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr); //Указываем позицию (0), количество цифр (3 т.к. их три в строке), формат данных, 
        //нормировать ли входные данные, шаг смещения между данными, шаг смещения от начала массива

        //Аналогично для цвета
        glEnableVertexAttribArray(1); //Позиция уже пекрвая
        glBindBuffer(GL_ARRAY_BUFFER, colors_vbo); //Переключаемся на буфер с цветами, делаем его текущим
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr); //Команда выполняется только для текущего буффера

        /*texCord*/
        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, texCords_vbo);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr); 

        pDefaultShaderProgram->use();
        pDefaultShaderProgram->setInt("tex", 0);

        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(pWindow))
        {
            /* Render here */
            glClear(GL_COLOR_BUFFER_BIT);

            //Отрисовка треугольника
            pDefaultShaderProgram->use();
            glBindVertexArray(vao);
            tex->bind();
            glDrawArrays(GL_TRIANGLES, 0, 3);

            /* Swap front and back buffers */
            glfwSwapBuffers(pWindow);

            /* Poll for and process events */
            glfwPollEvents();
        }
    }

    glfwTerminate();
    return 0;
}

