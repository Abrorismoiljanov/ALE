#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL_keyboard.h>
#include <SDL_mouse.h>
#include <SDL_scancode.h>
#include <SDL_stdinc.h>
#include "imgui.h"
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_opengl3.h"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>
#include <vector>

extern "C" {
    #include "tinyfiledialogs.h"
}
#include "ImGuiFileDialog.h"
#include "selection.h"
#include "stb_image.h"
#include "event.h"
#include "filecheck.h"


std::string OpenFileDialog() {
    const char* file = tinyfd_openFileDialog(
        "Select Texture",
        "",
        3,
        (const char*[]){"*.png","*.jpg","*.jpeg"},
        "Image files",
        0
    );
    if (file) return std::string(file);
    return std::string();
}


static std::vector<std::string> objFiles = GetOBJFiles("../assets/models/");

bool InputTextStd(const char* label, std::string& str, ImGuiInputTextFlags flags = 0)
{
    return ImGui::InputText(label, str.data(), str.capacity() + 1, flags | ImGuiInputTextFlags_CallbackResize,
        [](ImGuiInputTextCallbackData* data) -> int {
            std::string* s = (std::string*)data->UserData;
            if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
            {
                s->resize(data->BufTextLen);
                data->Buf = (char*)s->c_str();
            }
            return 0;
        }, &str);
}


int main(int argc, char** argv) {
    // -------------------------
    // Initialize SDL
    // -------------------------
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS) != 0) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return -1;
    }

    // Set OpenGL attributes
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    // Create window with OpenGL context
    SDL_Window* window = SDL_CreateWindow("ALE", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED ,1920, 1080, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1); // Enable vsync
    
    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        SDL_Log("Failed to initialize GLEW");
        return -1;
    }


    GLuint shaderProgram = CompileShader("../Shaders/Vertexshader.glsl", "../Shaders/Fragmentshader.glsl");

    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::lookAt(glm::vec3(2,2,5), glm::vec3(0,0,0), glm::vec3(0,1,0));
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1920.0f/1080.0f, 0.1f, 100.0f);
    
    bool relativeMouse = false;
    
    Camera camera;
    int savedX, savedY;
    int dx = 0, dy = 0;
    bool firstMouse = true;


    std::vector<Mesh> meshes;
    int selectedIndex = -1;
    

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init("#version 330");

    // -------------------------
    // Main loop
    // -------------------------
    
    glEnable(GL_DEPTH_TEST);
    bool running = true;
    SDL_Event event;

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
    
    float gridSize = 10.0f;
    int divisions = 20;
    float step = gridSize / divisions;
        
    std::string objpath;
    std::string texturepath;

    while (running) {
        const Uint8* keystate = SDL_GetKeyboardState(NULL);
        // input sistemasi uchun mo`ljallangan hudud
        // TODO: buni alohida header faylga funksiya qilib ko`chirish
       float currentFrame = SDL_GetTicks() / 1000.0f;
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;


        EventControl(dx, dy, keystate, event, running, relativeMouse, firstMouse, window, savedX, savedY, camera, deltaTime);    
           
        glUseProgram(shaderProgram);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f); 
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Start ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;
    

        ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always); // fixed position
        ImGui::SetNextWindowSize(ImVec2(400, 1080), ImGuiCond_Always); // fixed sizev
        
        ImGui::Begin("Meshes", nullptr, window_flags);
        
    for (size_t i = 0; i < objFiles.size(); ++i) {
      std::string buttonLabel = fs::path(objFiles[i]).filename().stem().string();
  
      if (ImGui::Button(buttonLabel.c_str())) {
            Mesh newMesh = loadOBJ(objFiles[i], "../assets/textures/default.png"); // only load on click
            newMesh.name = fs::path(objFiles[i]).filename().stem().string();
            meshes.push_back(newMesh);  
      }       
    }
      
        ImGui::End();
        
        ImGui::SetNextWindowPos(ImVec2(1700, 0), ImGuiCond_Always); // fixed position
        ImGui::SetNextWindowSize(ImVec2(200, 540), ImGuiCond_Always); // fixed sizev

        ImGui::Begin("List", nullptr, window_flags);
        
        for (size_t i = 0; i < meshes.size(); ++i) {
        Mesh& mesh = meshes[i];
 
        bool isSelected = (i == selectedIndex);

        if (isSelected) {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.6f, 1.0f, 1.0f));        // normal color
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.7f, 1.0f, 1.0f)); // hovered
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.5f, 0.9f, 1.0f));  // pressed
        }

        std::string buttonLabel = mesh.name + "##" + std::to_string(i);
        if (ImGui::Button(buttonLabel.c_str())) {
            selectedIndex = i;
        }
        if (isSelected) {
        ImGui::PopStyleColor(3); // pop the 3 colors we pushed
    }
    }

        ImGui::End();
 
        ImGui::SetNextWindowPos(ImVec2(1700, 540), ImGuiCond_Always); // fixed position
        ImGui::SetNextWindowSize(ImVec2(200, 540), ImGuiCond_Always); // fixed sizev

        Mesh* selectedmesh = (selectedIndex < meshes.size()) ? &meshes[selectedIndex] : nullptr;

        ImGui::Begin("Inspector", nullptr, window_flags);
        
        if (selectedmesh) {
            float x = selectedmesh->GetPosX();
            float y = selectedmesh->GetPosY();
            float z = selectedmesh->GetPosZ();

            float ax = selectedmesh->GetAngleX();
            float ay = selectedmesh->GetAngleY();
            float az = selectedmesh->GetAngleZ();

            float sx = selectedmesh->GetScaleX();
            float sy = selectedmesh->GetScaleY();
            float sz = selectedmesh->GetScaleZ();
            
            char buffer[128];
            strncpy(buffer, selectedmesh->name.c_str(), sizeof(buffer));
            buffer[sizeof(buffer) - 1] = '\0'; // Ensure null termination

            if (ImGui::InputText("Name", buffer, sizeof(buffer))) {
                selectedmesh->name = std::string(buffer); // Save back to the mesh
            }

 

            if (ImGui::DragFloat("X##", &x, 0.1f)) selectedmesh->SetPosX(x);
            if (ImGui::DragFloat("Y##", &y, 0.1f)) selectedmesh->SetPosY(y);
            if (ImGui::DragFloat("Z##", &z, 0.1f)) selectedmesh->SetPosZ(z);

            if (ImGui::DragFloat("Angle X##", &ax, 0.1f)) selectedmesh->SetAngleX(ax);
            if (ImGui::DragFloat("Angle Y##", &ay, 0.1f)) selectedmesh->SetAngleY(ay);
            if (ImGui::DragFloat("Angle Z##", &az, 0.1f)) selectedmesh->SetAngleZ(az);

            if (ImGui::DragFloat("Scale X##", &sx, 0.1f)) selectedmesh->SetScaleX(sx);
            if (ImGui::DragFloat("Scale Y##", &sy, 0.1f)) selectedmesh->SetScaleY(sy);
            if (ImGui::DragFloat("Scale Z##", &sz, 0.1f)) selectedmesh->SetScaleZ(sz);
 
if (ImGui::Button("Change Texture")) {
        // Create default config (you don't need to fill anything manually)
        IGFD::FileDialogConfig cfg;

        cfg.path = "../assets/textures",

        // Open the dialog
        ImGuiFileDialog::Instance()->OpenDialog(
            "ChooseFileDlgKey",       // Dialog ID
            "Choose Texture",         // Dialog title
            ".png,.jpg,.jpeg",        // File filters
            cfg// default config
        );
    }

    // Check if dialog was closed and file was selected
    if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey")) {
        if (ImGuiFileDialog::Instance()->IsOk()) {
            std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();

            if (!filePathName.empty()) {
                selectedmesh->SetTexture(filePathName.c_str());
            }
        }

        // Always close the dialog after displaying it
        ImGuiFileDialog::Instance()->Close();
    }
 if (ImGui::Button("Remove Mesh")) {
        // Erase the mesh at selectedIndex
        meshes.erase(meshes.begin() + selectedIndex);
        // Reset selection
        if (meshes.empty()) {
            selectedIndex = -1;
        } else {
            // Clamp selectedIndex to valid range
            selectedIndex = std::min(selectedIndex, (int)meshes.size() - 1);
        }
        selectedmesh = (selectedIndex >= 0) ? &meshes[selectedIndex] : nullptr;
    }

        }
    
        ImGui::End();

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        SDL_GL_GetDrawableSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = camera.GetProjectionMatrix(display_w, display_h);
       

// Set uniforms
glm::vec3 lightDir = glm::normalize(glm::vec3(-1.0f, -1.0f, -1.0f));
glm::vec3 lightColor = glm::vec3(1.0f);   // white light
glm::vec3 ambientColor = glm::vec3(0.2f); // subtle ambient

glUniform3fv(glGetUniformLocation(shaderProgram, "lightDir"), 1, glm::value_ptr(lightDir));
glUniform3fv(glGetUniformLocation(shaderProgram, "lightColor"), 1, glm::value_ptr(lightColor));
glUniform3fv(glGetUniformLocation(shaderProgram, "ambientColor"), 1, glm::value_ptr(ambientColor));

        for (auto& mesh : meshes) {
            mesh.Render(shaderProgram, view, projection);
        }

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }
    

    // для Еблана: Nothing it is just basic cleaning. maybe I will not put cleaning here in the future but just reminder
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
