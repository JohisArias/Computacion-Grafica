#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader_m.h"
#include "camera.h"
#include "model.h"

#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Iluminacion, sombras y radiosidad en OBJ importado - Grupo 3", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile shaders
    // -------------------------
    Shader ourShader("1.model_loading.vs", "1.model_loading.fs");
    Shader lightShader("2.light.vs", "2.light.fs");

    // load models
    // -----------
    Model ourModel("./modelo3d/Low-Poly Plant_.obj");
    //NOTA: los archivos obj deben tener un archivo mtl en donde se indican las texturas, etc que estilizan los objetos. Estos archivos tienen que 
    //estar bien configurados porque usualmente internamente tienen direciones que apuntan hacia los archivos jpg que forman las texturas y figuras que
    //se imprimen en los modelos obj. Configurar bien estos paths relativos y las variables que pueden tener estos para que no existan errores de renderizacion
    //o compilacion. En este caso, el modelo no se renderiza completamente bien porque los archivos de jpg a los que se ha configurado apuntar el arhivo mtl
    //no son los correctos para que funcione el renderizado.

    // draw in wireframe
    glPolygonMode(GL_FRONT_AND_BACK, GL_POINTS);

    // Creación de datos para el plano
    float planeVertices[] = {
        // Posiciones           // Normales         // Coordenadas de textura
        -3.0f, -0.5f, -3.0f,    0.0f, 1.0f, 0.0f,   0.0f, 0.0f,
         3.0f, -0.5f, -3.0f,    0.0f, 1.0f, 0.0f,   1.0f, 0.0f,
         3.0f, -0.5f,  3.0f,    0.0f, 1.0f, 0.0f,   1.0f, 1.0f,
        -3.0f, -0.5f,  3.0f,    0.0f, 1.0f, 0.0f,   0.0f, 1.0f,
    };

    // Índices para el plano (para dibujar como dos triángulos)
    unsigned int planeIndices[] = {
        0, 1, 2,
        0, 2, 3
    };

    // Creación del VAO, VBO y EBO para el plano
    unsigned int planeVAO, planeVBO, planeEBO;
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glGenBuffers(1, &planeEBO);

    glBindVertexArray(planeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, planeEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(planeIndices), planeIndices, GL_STATIC_DRAW);

    // Configuración de atributos de vértices
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // Fin de configuración de VAO
    glBindVertexArray(0);

    // Datos para el cubo de luz
    GLfloat lightVertices[] =
    { //     COORDINATES     //
        -0.1f, -0.1f,  0.1f,
        -0.1f, -0.1f, -0.1f,
         0.1f, -0.1f, -0.1f,
         0.1f, -0.1f,  0.1f,
        -0.1f,  0.1f,  0.1f,
        -0.1f,  0.1f, -0.1f,
         0.1f,  0.1f, -0.1f,
         0.1f,  0.1f,  0.1f
    };

    GLuint lightIndices[] =
    {
        0, 1, 2,
        0, 2, 3,
        0, 4, 7,
        0, 7, 3,
        3, 7, 6,
        3, 6, 2,
        2, 6, 5,
        2, 5, 1,
        1, 5, 4,
        1, 4, 0,
        4, 5, 6,
        4, 6, 7
    };

    // Creación del VAO, VBO y EBO para la luz
    unsigned int lightVAO, lightVBO, lightEBO;

    // Generates Vertex Array Object and binds it
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);

    glGenBuffers(1, &lightVBO);
    glBindBuffer(GL_ARRAY_BUFFER, lightVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(lightVertices), lightVertices, GL_STATIC_DRAW);

    glGenBuffers(1, &lightEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lightEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(lightIndices), lightIndices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, lightVBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Unbind all to prevent accidentally modifying them
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // Propiedades de la primera luz
    glm::vec3 LightPosition1(-0.5f, 0.5f, 0.05f);
    glm::vec4 LightColor1(1.0f, 0.5f, 0.5f, 1.0f); // Rosa

    // Propiedades de la segunda luz
    glm::vec3 LightPosition2(1.0f, 0.05f, 1.0f);
    glm::vec4 LightColor2(1.0f, 1.0f, 1.0f, 1.0f); // Blanco

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(1.0f, 0.87f, 0.68f, 1.0f);   // color de fondo
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 lightModel1 = glm::mat4(1.0f);
        lightModel1 = glm::translate(lightModel1, LightPosition1);
        glm::mat4 lightModel2 = glm::mat4(1.0f);
        lightModel2 = glm::translate(lightModel2, LightPosition2);

        // don't forget to enable shader before setting uniforms
        ourShader.use();

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        // Configura la textura del objeto
        ourShader.setBool("useTexture", true);          // Activa la textura
        ourShader.setInt("texture_diffuse1", 0);

        // render the loaded model
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -0.5f, 0.0f)); // translate it down so it's at the center of the scene (-0.5 due to plane in 'y' position)
        //model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
        model = glm::scale(model, glm::vec3(1.8f)); // Ajusta la escala del objeto a 1.8 veces mas grande
        ourShader.setMat4("model", model);
        ourModel.Draw(ourShader);

        // Configura el color de la superficie
        ourShader.setBool("useTexture", false);         // Desactiva la textura
        ourShader.setVec3("surfaceColor", glm::vec3(0.5f, 0.5f, 0.5f)); // color gris

        // render the plane      
        glm::mat4 planeModel = glm::mat4(1.0f);
        ourShader.setMat4("model", planeModel);
        glUniformMatrix4fv(glGetUniformLocation(ourShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(planeModel));

        // Primera luz
        glUniform4f(glGetUniformLocation(ourShader.ID, "lights[0].color"), LightColor1.x, LightColor1.y, LightColor1.z, LightColor1.w);
        glUniform3f(glGetUniformLocation(ourShader.ID, "lights[0].position"), LightPosition1.x, LightPosition1.y, LightPosition1.z);

        // Segunda luz
        glUniform4f(glGetUniformLocation(ourShader.ID, "lights[1].color"), LightColor2.x, LightColor2.y, LightColor2.z, LightColor2.w);
        glUniform3f(glGetUniformLocation(ourShader.ID, "lights[1].position"), LightPosition2.x, LightPosition2.y, LightPosition2.z);

        glBindVertexArray(planeVAO);
        glDrawElements(GL_LINES, 6, GL_UNSIGNED_INT, 0);    //dibuja con triangulos los 6 vertices, empezando desde el vertice con indice 0
        glBindVertexArray(0);

        ourShader.use();
        // Exports the camera Position to the Fragment Shader for specular lighting
        glUniform3f(glGetUniformLocation(ourShader.ID, "viewPosition"), camera.Position.x, camera.Position.y, camera.Position.z);
        // Export the camMatrix to the Vertex Shader of the plane
        glUniformMatrix4fv(glGetUniformLocation(ourShader.ID, "camMatrix"), 1, GL_FALSE, glm::value_ptr(projection * view));
        glBindVertexArray(planeVAO);            // Bind the VAO so OpenGL knows to use it        
        glDrawElements(GL_TRIANGLES, sizeof(planeIndices) / sizeof(int), GL_UNSIGNED_INT, 0);

        // Renderizar los cubos (representando los puntos de luz)
        lightShader.use();

        // Primer cubo luz
        glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(lightModel1));
        glUniform4f(glGetUniformLocation(lightShader.ID, "lightColor"), LightColor1.x, LightColor1.y, LightColor1.z, LightColor1.w);
        // Export the camMatrix to the Vertex Shader of the light cube
        glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "camMatrix"), 1, GL_FALSE, glm::value_ptr(projection * view));
        glBindVertexArray(lightVAO);            // Bind the VAO so OpenGL knows to use it
        glDrawElements(GL_TRIANGLES, sizeof(lightIndices) / sizeof(int), GL_UNSIGNED_INT, 0);

        // Segundo cubo luz
        glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(lightModel2));
        glUniform4f(glGetUniformLocation(lightShader.ID, "lightColor"), LightColor2.x, LightColor2.y, LightColor2.z, LightColor2.w);
        // Export the camMatrix to the Vertex Shader of the light cube
        glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "camMatrix"), 1, GL_FALSE, glm::value_ptr(projection * view));
        glBindVertexArray(lightVAO);            // Bind the VAO so OpenGL knows to use it
        glDrawElements(GL_TRIANGLES, sizeof(lightIndices) / sizeof(int), GL_UNSIGNED_INT, 0);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}