#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <io.h>
#include <time.h>

#define x_ screen_x / 2 - size_x
#define y_ 3 * (screen_y / 4) - size_y

int text_size_x;
int text_size_y;

int screen_x = 0;
int screen_y = 0;

const int size_x = 500;
const int size_y = 100;

int count = 0;

void file_amount() {
  struct _finddata_t data;
  intptr_t file;
  file = _findfirst("*.wav", &data);
  do {
    count++;
    printf("%s monkey\n", data.name);
  }while(_findnext(file, &data) == 0);
  _findclose(file);
}

void get_screen_dem() {
  DEVMODE mode;
  memset(&mode, 0, sizeof(mode));
  mode.dmSize = sizeof(mode);
  EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &mode);
  screen_x = mode.dmPelsWidth;
  screen_y = mode.dmPelsHeight;
}

int main(int argc,char* argv[]) {
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
  Mix_Init(MIX_INIT_FLAC  |
             MIX_INIT_MOD |
             MIX_INIT_MP3 |
             MIX_INIT_OGG);
  TTF_Init();
  
  get_screen_dem();
  file_amount();
  
  SDL_Window* window = SDL_CreateWindow("SDL", x_, y_, size_x, size_y, SDL_WINDOW_BORDERLESS);
  SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
  TTF_Font* font = TTF_OpenFont("GameCube.ttf", 24);
  
  SDL_Color ttf_color = {0,255,255,200};
  SDL_Surface* ttf_surface_paused = TTF_RenderText_Solid(font, "PAUSED", ttf_color);
  SDL_Surface* ttf_surface_playing = TTF_RenderText_Solid(font, "PLAYING", ttf_color);
  SDL_Texture* ttf_texture = SDL_CreateTextureFromSurface(renderer, ttf_surface_paused);

  TTF_SizeText(font, "PAUSED", &text_size_x, &text_size_y);
  
  TTF_CloseFont(font);
  TTF_Quit();

  SDL_Rect ttf_rect = {120, 10, text_size_x, text_size_y};
  
  Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 1, 2048);
  Mix_Chunk* chunk;

  SDL_Rect graph_rect1 = {100, 100, 30, -50};
  SDL_Rect graph_rect2 = {140, 100, 30, -50};
  SDL_Rect graph_rect3 = {180, 100, 30, -50};
  SDL_Rect graph_rect4 = {220, 100, 30, -50};
  SDL_Rect graph_rect5 = {260, 100, 30, -50};
  SDL_Rect graph_rect6 = {300, 100, 30, -50};
  SDL_Rect graph_rect7 = {340, 100, 30, -50};
  SDL_Rect graph_rect8 = {380, 100, 30, -50};
  SDL_Rect* graphs[8] = {&graph_rect1, &graph_rect2, &graph_rect3, &graph_rect4, &graph_rect5, &graph_rect6, &graph_rect7, &graph_rect8};
  
  SDL_Event event;

  for(;;) {
    if((SDL_PollEvent(&event) && event.type == SDL_QUIT) || (GetKeyState(VK_CONTROL) & 0x1000 && GetKeyState(VK_SPACE) & 0x1000 && GetKeyState(VK_ESCAPE) & 0x1000))
      goto quit;

    SDL_SetRenderDrawColor(renderer, 15,15,15,50);

    SDL_RenderClear(renderer);

    /* code here */
    ttf_rect.x += 1;
    if(ttf_rect.x > 495) {
      ttf_rect.x = -180;
    }

    ttf_rect.w = text_size_x;
    ttf_rect.h = text_size_y;
  
    SDL_RenderCopy(renderer, ttf_texture, NULL, &ttf_rect);
    SDL_DestroyTexture(ttf_texture);
    SDL_Delay(10);

    static int toggle = 0;
    
    if((GetKeyState(VK_CONTROL) & 0x1000 && GetKeyState(VK_SPACE) & 0x1000 && GetKeyState(VK_RETURN) & 0x1000) || toggle == 1) {
      ttf_texture = SDL_CreateTextureFromSurface(renderer, ttf_surface_playing);
      TTF_SizeText(font, "PLAYING", &text_size_x, &text_size_y);
      
      if ((GetKeyState(VK_CONTROL) & 0x1000 && GetKeyState(VK_SPACE) & 0x1000 && GetKeyState(VK_RETURN) & 0x1000)) {
	Mix_Resume(-1);

	time_t t;
	time(&t);
	srand(t);
	
	struct _finddata_t find_data;
	intptr_t file_ptr = _findfirst("*.wav", &find_data);
	do {
	chunk = Mix_LoadWAV(find_data.name); //fix the leak here also the segfault on entry
	}while((_findnext(file_ptr, &find_data) == 0) && ((rand() % (count + 1)) >= count));
	_findclose(file_ptr);
	Mix_PlayChannel(1, chunk, -1);
	//Mix_FreeChunk(chunk);
      }

      SDL_SetRenderDrawColor(renderer, 255,155,255,180);

      for(int q = 0; q < 7; q++) {
	graphs[q]->h = rand() % 35;
	graphs[q]->h = -graphs[q]->h;

	SDL_RenderFillRect(renderer, graphs[q]);
      }
                        
      toggle = 1;
    }
    if((GetKeyState(VK_CONTROL) & 0x1000 && GetKeyState(VK_SPACE) & 0x1000 && GetKeyState(VK_BACK) & 0x1000) || toggle == 0) {
      ttf_texture = SDL_CreateTextureFromSurface(renderer, ttf_surface_paused);
      TTF_SizeText(font, "PAUSED", &text_size_x, &text_size_y);
      Mix_Pause(-1);

      toggle = 0;
    }
    /* code here */
    
    SDL_RenderPresent(renderer);
  }

  quit:;
  for(int q=0;q<6;q++)
    printf("nigga you just quit");
  
  Mix_FreeChunk(chunk);
  Mix_CloseAudio();

  SDL_FreeSurface(ttf_surface_playing);
  SDL_FreeSurface(ttf_surface_paused);
  SDL_DestroyTexture(ttf_texture);
  
  SDL_DestroyWindow(window);
  SDL_DestroyRenderer(renderer);

  SDL_Quit();

  return 0;
}


