#include <SDL_events.h>
#include <SDL2/SDL.h>
#include <SDL_video.h>
#include <Camera.h>
#include_next <imgui_impl_sdl2.h>

void EventControl(int &dx, int &dy, const Uint8* keystate, SDL_Event& event,
                 bool &running, bool &relativeMouse, bool &firstMouse, 
                 SDL_Window* window, int &savedX, int &savedY,
                 Camera &camera, float deltaTime){

    while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
                if (event.type == SDL_QUIT) running = false;
            
                if (event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_Q) {
                    relativeMouse = !relativeMouse; // flip state
                
                    if (relativeMouse) {
                        SDL_GetMouseState(&savedX, &savedY);
                        SDL_SetRelativeMouseMode(SDL_TRUE);
                        firstMouse = true;
                    } else {
                        SDL_WarpMouseInWindow(window, savedX, savedY);
                        SDL_SetRelativeMouseMode(SDL_FALSE);
                        firstMouse = false;
                    }
                }
            }

        
        if (keystate[SDL_SCANCODE_ESCAPE]) {
            running = false;
        }   

        if (SDL_GetRelativeMouseMode()) {
            SDL_GetRelativeMouseState(&dx, &dy);

            if (firstMouse) {
                dx = 0;
                dy = 0;
                firstMouse = false;
            }
        }
 
        camera.ProcessKeyboard(keystate, deltaTime, relativeMouse);
        camera.ProcessMouseMovement(dx, dy);

}
