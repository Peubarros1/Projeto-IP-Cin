//Importação das bibliotecas para o projeto
#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

//Resoulução da tela de jogo(HD)
#define WIDTHSCREEN 1280
#define HEIGHTSCREEN 720

// Definição dos estados de transição de tela do jogo
typedef enum EstadosDeTela {Carregamento,Titulo,Gameplay,Personagem,Controles,Opcoes,Final}EstadosDeTela;

// Estrutura para os personagens com suas respectivas caracteristicas
typedef struct {

    char name[20];
    Rectangle rect;
    int health;
    int attackDamage;
    int jumpForce;
    int velocity;
    int jumpSpeed;
    int isJumping;

} Character;

// Estrutura para os projéteis com suas caracteristicas
typedef struct
{
    Vector2 position;
    Vector2 speed;
    bool active;

} Projectile;

//Estrutura para os itens do menu
typedef struct {
  
    Rectangle rect;
    const char* text;
    Color textColor;
    Color rectColor;
  
} MenuItem;

// Função para atualizar a posição do personagem e mantê-lo dentro dos limites da tela
void atualizarPersonagem(Character *character, int sentido, int screenWidth){
    //Atualiza a posição do personagem
    character->rect.x += sentido * 5;

    //Controle de movimentação do personagem dentro dos limites da tela
    if (character->rect.x < 0)
        character->rect.x = 0;
    if (character->rect.x + character->rect.width > WIDTHSCREEN)
        character->rect.x = screenWidth - character->rect.width;
}

// Função para desenhar um personagem
void desenharCharacter(Character character, Color color) {
    DrawRectangleRec(character.rect, color);
}

// Função para disparar um projétil
void dispararProjetil(Character character, Projectile *projetcile){

    //Verifica se o projetil não esta ativo antes de dispara-lo
    if (!projetcile->active) {
        projetcile->position = (Vector2){character.rect.x, character.rect.y + 150};
        projetcile->active = true;
    }
}
void aplicarGravidade(Character *player, int *countJump) {
    // Se o jogador estiver pulando atualize suas posições
    if (player->isJumping) {
        player->rect.y -= player->jumpSpeed;
        player->jumpForce -= player->jumpSpeed/1.7;
    
        // Verifica se o pulo terminou
        if (player->jumpForce <= 0) {
            player->isJumping = false;
        }
    } else {
        // Aplica a gravidade ao jogador
        if((player->rect.y + player->rect.height) < 600) {
            player->rect.y += 14; // Adicione sua própria lógica de gravidade aqui, se necessário
        }
        else
            (*countJump) = 0;
    }
}

int main(void) {

    // Inicialização da janela
    InitWindow(WIDTHSCREEN, HEIGHTSCREEN, "Bloody War");
    
    int playerCooldown = 0;
    int enemyCooldown = 0;
    int cooldownDuration = 60;

    // Variáveis para controlar o cronometro
    int counterFps = 0;
    int count = 120;
    int fpsAtual;

    //Variaveis para controlar os pulos
    int countJump = 0;
    int countJump2 = 0;

    // Definição da quantidade de items por menu
    int numItemsMenu = 3;
    int numItemsOptions = 2;

    //Criação dos personagens
    Character player = {"Bloodthirsty", {200, 350, 100, 250}, 100, 3, 150, 0,20,0};
    Character enemy = {"Warrior", {1000, 350, 100, 250}, 100, 3, 20, 0,20,0};

    //Definiçao do projetil do player e do enemy
    Projectile projetcilePlayer = {(Vector2){player.rect.x, player.rect.y + 150}, (Vector2){10, 0}, false};
    Projectile projetcileEnemy = {(Vector2){enemy.rect.x, enemy.rect.y + 150}, (Vector2){10, 0}, false};

    //Posições dos textos do menu
    Vector2 textPosition1 = {785.f, 130.f};
    Vector2 textPosition2 = {100, HEIGHTSCREEN / 3};

    // Instalação da fonte
    Font font = LoadFont("leadcoat.ttf");
    if (font.texture.id == 0) {
        TraceLog(LOG_WARNING, "Font could not be loaded! Exiting...");
        CloseWindow();
        return -1;
    }

    //Carregamento das imagens
    Image myImage = LoadImage("OIG_resized.png");
    Texture2D texture = LoadTextureFromImage(myImage);
    UnloadImage(myImage);

    Image myImage2 = LoadImage("Espadas_Duplas2.png");
    Texture2D texture2 = LoadTextureFromImage(myImage2);
    UnloadImage(myImage2);

    Image myImage3 = LoadImage("arena.png");
    Texture2D texture3 = LoadTextureFromImage(myImage3);
    UnloadImage(myImage3);

    Image myImage4 = LoadImage("escudo.png");
    Texture2D texture4 = LoadTextureFromImage(myImage4);
    UnloadImage(myImage4);
    
    // Estado inicial da tela
    EstadosDeTela estadoTela = Carregamento;
  
    //Definindo os botões do menu
    MenuItem itemsMenu[] = {
        {{793, 300, 400, 40}, "Click Here to battle!", WHITE, RED},
        {{900, 470, 200, 40}, "Options", WHITE, RED},
        {{900, 530, 200, 40}, "Controls", WHITE, RED},
    };

    //Definindo os botões dentro do sub-menu "Options"
    MenuItem itemsOptions[] = {
        {{560, 300, 200, 40}, "Sound", WHITE, RED},
        {{560, 360, 200, 40}, "Back", WHITE, RED}
    };

    //Loop enquanto a janela não estiver pronta
    while (!IsWindowReady()){}

    //Definindo 60 FPS
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
      
        switch(estadoTela) {
            case Carregamento:
            {
                //Tela de carregamento por 2 segundos
                counterFps++;
                if (counterFps > 120) {
                    estadoTela = Titulo;
                    counterFps = 0;
                } 
            } break;
            case Titulo:
            {
                //Para todo item do menu 
                for (int i = 0; i < numItemsMenu; i++){
                    //Verifica se o usuario selecionou alguma opção
                    if (CheckCollisionPointRec(GetMousePosition(), itemsMenu[i].rect)) {
                        
                        //Checa se o mouse esta encima do botão, se verdadeiro altera a cor para indicação
                        if(GetMouseX() == itemsMenu[i].rect.x || GetMouseY() == itemsMenu[i].rect.y)
                            itemsMenu[i].rectColor = MAROON;

                        //Verifica se alguma opção da janela de titulo foi selecionada
                        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                            if (i == 0) estadoTela = Gameplay;
                            else if (i == 1) estadoTela = Opcoes;
                            else if(i == 2) estadoTela = Controles;
                        }
                    }
                }
            } break;
            case Gameplay:
            {
                //Controle do cronometro
                counterFps++;
                if (counterFps == 120 || counterFps == fpsAtual + 60) {
                    fpsAtual = counterFps;
                    count--;
                }    
                if (counterFps == 7200)
                    estadoTela = Final;

                //Se apertar P fecha o jogo
                if (IsKeyPressed(KEY_P))
                    estadoTela = Final;

                ////Controle do personagem 1////
                //Movimentação do personagem principal

                if (IsKeyDown(KEY_D))
                    atualizarPersonagem(&player, 1, WIDTHSCREEN);
                if (IsKeyDown(KEY_A))
                    atualizarPersonagem(&player, -1, WIDTHSCREEN);

                //Ataque do personagem
                if (IsKeyPressed(KEY_C)) {
                    if (CheckCollisionRecs(player.rect, enemy.rect)) {
                        enemy.health -= player.attackDamage;
                    } 
                }

                //Atualização da posição do projetil
                if (IsKeyPressed(KEY_X)) {
                    dispararProjetil(player, &projetcilePlayer);
                }

                //Se o projetil estiver ativo, tera uma taxa de atualização da posição X
                if (projetcilePlayer.active) {
                    projetcilePlayer.position.x += projetcilePlayer.speed.x;

                    //Se o projeto ultrapassar os limites da tela, ele deixa de ser ativo
                    if (projetcilePlayer.position.x > WIDTHSCREEN){
                        projetcilePlayer.active = false;
                    }
                    if(CheckCollisionCircleRec(projetcilePlayer.position,25,enemy.rect)){
                        enemy.health -= player.attackDamage;
                        projetcilePlayer.active = false;
                    }
                }

                if (IsKeyPressed(KEY_W) && !player.isJumping && countJump < 2) {
                    countJump += 1;
                    player.isJumping = true;
                    player.jumpForce = 150;
                }

                aplicarGravidade(&player, &countJump);
                //Fim do controle do personagem 1//

                ////Controle do personagem 2////
                //Movimentação do segundo player
                if (IsKeyDown(KEY_RIGHT))
                    atualizarPersonagem(&enemy, 1, WIDTHSCREEN);
                if (IsKeyDown(KEY_LEFT))
                    atualizarPersonagem(&enemy, -1, WIDTHSCREEN);

                //Ataque do personagem
                if (IsKeyPressed(KEY_M)) {
                    if (CheckCollisionRecs(enemy.rect, player.rect)) {
                        player.health -= enemy.attackDamage;
                    } 
                }

                //Atualização da posição do projetil
                if (IsKeyPressed(KEY_N)) {
                    dispararProjetil(enemy, &projetcileEnemy);
                }

                //Se o projetil estiver ativo, tera uma taxa de atualização da posição X
                if (projetcileEnemy.active) {
                    projetcileEnemy.position.x -= projetcileEnemy.speed.x;

                    //Se o projeto ultrapassar os limites da tela, ele deixa de ser ativo
                    if (projetcileEnemy.position.x < 0){
                        projetcileEnemy.active = false;
                    }
                    if(CheckCollisionCircleRec(projetcileEnemy.position,25,player.rect)){
                        player.health -= enemy.attackDamage;
                        projetcileEnemy.active = false;
                    }
                }

                if (IsKeyPressed(KEY_UP) && !enemy.isJumping && countJump2 < 2) {
                    countJump2 += 1;
                    enemy.isJumping = true;
                    enemy.jumpForce = 150;
                }
                 if (CheckCollisionRecs(player.rect, enemy.rect)) {
            // Verifique se o cooldown não está ativo
            if (playerCooldown <= 0 && enemyCooldown <= 0) {
                // Aplica o efeito da colisão, como redução de saúde, por exemplo
                player.health -= enemy.attackDamage;
                enemy.health -= player.attackDamage;

                // Ativa o cooldown para ambos os personagens
                playerCooldown = cooldownDuration;
                enemyCooldown = cooldownDuration;
            }
        }

        if (playerCooldown > 0) {
            playerCooldown--;
        }
        if (enemyCooldown > 0) {
            enemyCooldown--;
        }


                aplicarGravidade(&enemy, &countJump2);
                //Fim do controle do personagem 2//

            } break;  
            case Opcoes:
            {
                int temSom = 1;
                ///Percorrendo as sub-opções do options
                for (int i = 0; i < numItemsOptions; i++){
                    
                    if (CheckCollisionPointRec(GetMousePosition(), itemsOptions[i].rect)) {

                        //Checa se o mouse esta encima do botão, se verdadeiro altera a cor para indicação
                        if(GetMouseX() == itemsOptions[i].rect.x || GetMouseY() == itemsOptions[i].rect.y)
                            itemsOptions[i].rectColor = MAROON;

                        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
                            //Configuração do som
                            if (i == 0) {
                                if (temSom) {
                                    SetMasterVolume(0.0);
                                    temSom = 0;
                                } else {
                                    SetMasterVolume(1.0);
                                    temSom = 1;
                                }
                            } else if (i == 1) {
                                estadoTela = Titulo; 
                            }
                        }
                    }
                }
            } break;
            case Controles:
            {
                //P volta a tela de titulo
                if (IsKeyPressed(KEY_P))
                    estadoTela = Titulo;

            }break;
            case Final:
            {
                //Volta para o inicio quando se apertar enter na tela final
                if (IsKeyPressed(KEY_ENTER))
                    estadoTela = Titulo;
            } break;
            default: break;
              
        }
        
        BeginDrawing();
        ClearBackground(BLACK);

        switch(estadoTela) {
            case Carregamento:
            {
                //Tela de esperapor 2 segundos
                DrawTexture(texture4, 500, 300, WHITE);
                DrawTextEx(font, "This a loading screen. Wait for 2 seconds", textPosition2, 70, 2, WHITE);
            } break;
            case Titulo:
            {
                //Desennhando as texturas
                DrawTexture(texture, 0, 0, WHITE);
                DrawTexture(texture2, 800, 345, WHITE);

                //Retangulos do titulo
                DrawRectangle(760, 40, 490, 240, RED);
                DrawRectangle(780, 65, 450, 190, MAROON);

                //Desenhando o titulo
                DrawTextEx(font, "Bloody War", textPosition1, 100, 2, BLACK);

                for (int i = 0; i < numItemsMenu; i++) { 
                    DrawRectangleRec(itemsMenu[i].rect, itemsMenu[i].rectColor);
                    DrawText(itemsMenu[i].text, (int)(itemsMenu[i].rect.x + itemsMenu[i].rect.width / 2 - MeasureText(itemsMenu[i].text, 20) / 2), (int)(itemsMenu[i].rect.y + itemsMenu[i].rect.height / 2 - 10), 20, itemsMenu[i].textColor);
                }
            } break;
            case Gameplay:
            {
                //Textura de fundo
                DrawTexture(texture3, 0, 0, WHITE);

                //Desenhando os personagens do game
                desenharCharacter(player, BLUE);
                desenharCharacter(enemy, GREEN);

                //Desenhando a base(piso) dos personagems
                DrawRectangle(0, 600, WIDTHSCREEN, 120, BROWN);

                //Barra de vida dos personagens
                DrawRectangle(10, 10, player.health * 2, 20, WHITE);
                DrawRectangle(1080, 10, enemy.health * 2, 20, RED);

                //Cronometro
                char numeroString[20];
                sprintf(numeroString, "%d", count);
                DrawText(numeroString, WIDTHSCREEN / 2, 10, 20, BLACK);

                //Desenha os nomes dos personagens 
                DrawText(player.name, 20, 70, 40, MAROON);
                DrawText(enemy.name, 1020, 70, 40, WHITE);

                //Desenhando o projetil caso esteja ativo
                if (projetcilePlayer.active == true) {
                    DrawCircleV(projetcilePlayer.position, 25, RED);
                }
                if (projetcileEnemy.active == true) {
                    DrawCircleV(projetcileEnemy.position, 25, RED);
                }

            } break;
            case Opcoes:
            {
                for (int i = 0; i < numItemsOptions; i++) { 
                    DrawRectangleRec(itemsOptions[i].rect, itemsOptions[i].rectColor);
                    DrawText(itemsOptions[i].text, (int)(itemsOptions[i].rect.x + itemsOptions[i].rect.width / 2 - MeasureText(itemsOptions[i].text, 20) / 2), (int)(itemsOptions[i].rect.y + itemsOptions[i].rect.height / 2 - 10), 20, itemsOptions[i].textColor);
                }
            } break;
            case Controles:
            {
                ClearBackground(BLACK);

                //Descrevendo os controles basicos
                DrawText("Player 1 Controls:",100,100,30,WHITE);
                DrawText("D -> move to the right | A -> move to the left",100,150,30,WHITE);
                DrawText("W -> to jump",100,200,30,WHITE);
                DrawText("C -> hand-to-hand attack | X -> shoot projectile ",100,250,30,WHITE);

                DrawText("Player 2 Controls:",100,350,30,GRAY);
                DrawText("Right arrow -> move to the right | Left arrow -> move to the left",100,400,30,GRAY);
                DrawText("Up arrow -> to jump",100,450,30,GRAY);
                DrawText("M -> hand-to-hand attack | N -> shoot projectile ",100,500,30,GRAY);

            }break;
            case Final:
            {
                //Tela final de jogo;
                DrawText("Ending Screen is Here", 100, HEIGHTSCREEN / 2, 60, WHITE);

            }break;
            default: break;
        }
        EndDrawing();
    }
    CloseWindow();
    return 0;
}