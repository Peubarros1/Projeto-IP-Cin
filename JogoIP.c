//Importação das bibliotecas
#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>

//Definindo as dimensões da tela(Resolução HD 1280x720)
#define WIDTHSCREEN 1280
#define HEIGHTSCREEN 720

//Definindo os estados de transição de tela
typedef enum EstadosDeTela{Carregamento, Titulo,Escolhadepersonagem, Gameplay, Final}EstadosDeTela;

int main(void){

    //Inicializando a tela com as dimensões pre-processadas
    InitWindow(WIDTHSCREEN,HEIGHTSCREEN,"Bloody War");

    //Posições dos textos que aparecem no menu do jogo
    //Totalizando quatro textos
    Vector2 textPosition1 = {785.f, 130.f};
    Vector2 textPosition2 = {793.f, 300.f};
    Vector2 textPosition3 = {940.f, 470.f};
    Vector2 textPosition4 = {930.f, 530.f};

    //Instalação da fonte do tipo ttf
    Font font = LoadFont("leadcoat.ttf");

    //Verificando se não ocorreu nenhum problema na instalação da fonte
    if (font.texture.id == 0) {
        TraceLog(LOG_WARNING, "Font could not be loaded! Exiting...");
        CloseWindow();
        return -1;
    }

    //Processo para carregar as imagens do menu do jogo
    //Primeira Imagem
    Image myImage = LoadImage("OIG_resized.png");
    Texture2D texture = LoadTextureFromImage(myImage);
    UnloadImage(myImage);

    //Segunda Imagem
    Image myImage2 = LoadImage("Espadas_Duplas2.png");
    Texture2D texture2 = LoadTextureFromImage(myImage2);
    UnloadImage(myImage2);

    //Variavel que armazena o estado de tela atual
    EstadosDeTela estadoTela = Carregamento;

    //Loop enquanto a tela não estiver pronta
    while(!IsWindowReady()){

    }
    //Variavel para contar a quantidade de quadros
    int counterFps = 0;

    //Cravando 60 FPS
    SetTargetFPS(60);

    //Loop principal onde o jogo vai rodar
    while(!WindowShouldClose()){

        switch(estadoTela)
        {
            //Fica na tela de carregamento por dois segundo, ou seja, 120 Frames
            case Carregamento:
            {
                counterFps++;
                if(counterFps > 120)
                    estadoTela = Titulo;

            }break;
            //Entra no jogo se o usuario digitar Enter
            case Titulo:
            {
                if(IsKeyPressed(KEY_ENTER))
                    estadoTela = Escolhadepersonagem;

            }break;
            //escolher o persongem e vai pra tela da gameplay
            case Escolhadepersonagem:
            {
                if (IsMouseButtonUp(MOUSE_LEFT_BUTTON)) 
                    estadoTela = Gameplay;
            }break;
            //se apertar p vai pra tela final
            case Gameplay:
            {
                 if(IsKeyPressed(KEY_P))
                    estadoTela = Final;
            }break; 
            
            //Volta para o menu se o usuario digitar Enter
            case Final:
            {
                if(IsKeyPressed(KEY_ENTER))
                    estadoTela = Titulo;
            }break;

            default:break;

        }
        //Inicio da parte grafica(Desenho)
        BeginDrawing();
        //Definindo o fundo de cor preta
        ClearBackground(BLACK);

            switch(estadoTela)
            {
                //Tela de carregamento
                case Carregamento:
                {
                    DrawText("Loading Screen", 20, 20, 50, RED);
                    DrawText("This a loading screen. Wait for 2 seconds!",100,HEIGHTSCREEN/2,50, WHITE);

                }break;
                //Tela de titulo
                case Titulo:
                {

                    //Introduzindo as duas imagens(texturas)
                    DrawTexture(texture,0,0, WHITE);
                    DrawTexture(texture2,800,345, WHITE);

                    //Introduzindo os retangulos do titulo
                    DrawRectangle(760,40,490,240,RED);
                    DrawRectangle(780,65,450,190,MAROON);

                    //Titulo e etc...
                    DrawTextEx(font,"Bloody War",textPosition1,100,2,BLACK);
                    DrawTextEx(font,"Press Enter for continue!",textPosition2,40,2,WHITE);
                    DrawTextEx(font,"Options",textPosition3,40,2,RAYWHITE);
                    DrawTextEx(font,"Controls",textPosition4,40,2,RAYWHITE);

                }break;
                case Gameplay:
                {
                    //Logica do jogo aqui dentro
                    //O que acontece no jogo sera introduzido aqui, que é a tela pos menu
                    //Aqui a parte grafica do jogo acontece
                    DrawText("Gameplay Screen is Here",100,HEIGHTSCREEN/2,60,WHITE);

    
                }break;
                case Final:
                {
                    //Tela final aqui dentro
                    DrawText("Ending Screen is Here",100,HEIGHTSCREEN/2,60,WHITE);

                }break;
                default: break;
            }
    
        EndDrawing();
        //Fim da parte grafica(Desenho)
    }
    //Fechar janela
    CloseWindow();

return 0;
}
