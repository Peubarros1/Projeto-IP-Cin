#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#define WIDTHSCREEN 1280
#define HEIGHTSCREEN 720
#define FATOR_REDUCAO 0.5
#define COOLDOWN_ATTACK 0.5

//Definição dos estados de transição de tela do jogo
typedef enum EstadosDeTela {Carregamento, Titulo, Gameplay, Personagem, Controles, Opcoes, Final}EstadosDeTela;

//Estrutura para os personagens com suas respectivas caracteristicas
typedef struct {

    Rectangle posRect;
    Rectangle SourceRect;
    char name[20];
    int health;
    int attackDamage;
    int jumpForce;
    int velocity;
    int jumpSpeed;
    int isJumping;
    bool isAttacking;

} Character;

//Estrutura para os projéteis com suas caracteristicas
typedef struct
{
    Character player;
    Rectangle Source;
    Rectangle Position;
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
    character->posRect.x += sentido * 5;

    //Controle de movimentação do personagem dentro dos limites da tela
    if (character->posRect.x < 0)
        character->posRect.x = 0;
    if (character->posRect.x + character->posRect.width > WIDTHSCREEN)
        character->posRect.x = screenWidth - character->posRect.width;
}

// Função para desenhar um personagem
void desenharCharacter(Character *character, Texture2D texture) {
    DrawTexturePro(texture, character->SourceRect, character->posRect,(Vector2){0, 0}, 0, WHITE);
}
// Função para disparar um projétil
void dispararProjetil(Projectile *projetcile){

    //Verifica se o projetil não esta ativo antes de dispara-lo
    if (!projetcile->active) {
        //projetcile->Source.x = projetcile->Source.x
        //projetcile->Source.y = projetcile->Source.y;
        projetcile->active = true;
    }
}

void aplicarGravidade(Character *Character, int *countJump) {
    // Se o jogador estiver pulando atualize suas posições
    if (Character->isJumping) {
        Character->posRect.y -= Character->jumpSpeed;
        Character->jumpForce -= Character->jumpSpeed/1.7;
    
        // Verifica se o pulo terminou
        if (Character->jumpForce <= 0) {
            Character->isJumping = false;
        }
    } else {
        // Aplica a gravidade ao jogador
        if((Character->posRect.y + Character->posRect.height) < 650) {
            Character->posRect.y += 14;
        }
        else
            (*countJump) = 0;
    }
}

int main(void) {

    InitWindow(WIDTHSCREEN, HEIGHTSCREEN, "Bloody War");

    int controlHeight = 0;

    int TimeProjectile = 0;
    int TimeProjectile2 = 0;

    //Tempo de ataque
    double timeAttack = 0;
    double timeAttack2 = 0;

    //Contador para os sprites dos personagens
    int countSprite = 0;
    int countSprite2 = 0;

    //Variáveis para controlar o cronometro
    int counterFps = 0;
    int count = 120;
    int fpsAtual;

    // Definição da quantidade de items por menu
    int numItemsMenu = 3;
    int numItemsOptions = 2;

    //Variaveis para controlar os pulos dos personagens
    int countJump = 0;
    int countJump2 = 0;
     //variáveis de tempo de cooldown para cada personagem
    double cooldownPlayer = 0;
    double cooldownEnemy = 0;

    Character player = {{200, 300, 350, 350},{50 * countSprite + 24,88,50, 100},"Bloodthirsty",100, 3, 150, 0,20,0,false};
    Character enemy = {{900,300,350,350},{100 * countSprite2,130,100,136},"Warrior",100,2,150,0,20,0,false};

    Projectile projetcilePlayer = {player,{player.posRect.x + 150,player.posRect.y + 30,100,100 },{200,120,490,420} ,(Vector2){15, 0}, false};
    Projectile projetcileEnemy = {enemy,{enemy.posRect.x,enemy.posRect.y,100,100 },{0,0,220,220},{10, 0}, false};

    Rectangle DefaultSourcePlayer1 = player.SourceRect;
    Rectangle DefaultSourcePlayer2 = enemy.SourceRect;
    Rectangle DefaultPositionPlayer2 = enemy.posRect;

    Rectangle posRect2;

    //Posições dos textos do menu
    Vector2 textPosition1 = {785.f, 130.f};
    Vector2 textPosition2 = {100, HEIGHTSCREEN / 3};
    Vector2 textPosition3 = {400,610};

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

    Image myImage3 = LoadImage("street-fighter-japanese-signs-1280x720.png");
    Texture2D texture3 = LoadTextureFromImage(myImage3);
    UnloadImage(myImage3);

    Image myImage4 = LoadImage("escudo.png");
    Texture2D texture4 = LoadTextureFromImage(myImage4);
    UnloadImage(myImage4);

    Image myImage5 = LoadImage("hero3.png");
    Texture2D texture5 = LoadTextureFromImage(myImage5);
    UnloadImage(myImage5);

    Image myImage6 = LoadImage("hero1.png");
    Texture2D texture6 = LoadTextureFromImage(myImage6);
    UnloadImage(myImage6);

    Image myImage7 = LoadImage("ballblue.png");
    Texture2D texture7 = LoadTextureFromImage(myImage7);
    UnloadImage(myImage7);

    Image myImage8 = LoadImage("ballred.png");
    Texture2D texture8 = LoadTextureFromImage(myImage8);
    UnloadImage(myImage8);
    
    
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

    while (!IsWindowReady()){}

    //Definindo 60 FPS
    SetTargetFPS(60);

    while (!WindowShouldClose()) {

        posRect2 = (Rectangle){player.posRect.x,player.posRect.y,100,100};
                
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
                //Se apertar P fecha o jogo
                if (IsKeyPressed(KEY_P))
                    estadoTela = Final;

                //Controle do cronometro
                counterFps++;
                if (counterFps == 120 || counterFps == fpsAtual + 60) {
                    fpsAtual = counterFps;
                    count--;
                }    
                if (counterFps == 7200)
                    estadoTela = Final;

                
                //Movimentação do personagem de numero 1
                if (IsKeyDown(KEY_D)){
                    atualizarPersonagem(&player, 1, WIDTHSCREEN);
                    countSprite = ((int)GetTime())%3;
                    player.SourceRect = (Rectangle){50 * countSprite + 24,88, 50, 100};
                }else
                    player.SourceRect = (Rectangle){24,88, 50, 110};
                
                if (IsKeyDown(KEY_A)){
                    atualizarPersonagem(&player, -1, WIDTHSCREEN);
                    countSprite = ((int)GetTime())%3;
                    player.SourceRect = (Rectangle){50 * countSprite + 24,88, 50, 100};
                }

                if (IsKeyPressed(KEY_C) && player.isAttacking == false) {
                player.isAttacking = true;
                timeAttack = GetTime() + 0.2; 
                }
                if (player.isAttacking && GetTime() < timeAttack){
                // Desenhe a animação de ataque
                player.SourceRect = (Rectangle){325, 180, 70, 90};
                if (CheckCollisionRecs(enemy.posRect, player.posRect)) {
                    enemy.health -= 1;
                }

                }
                else if(player.isAttacking && GetTime() >= timeAttack) {
                // Se não estiver atacando ou o tempo de ataque acabar, volte para a animação padrão
                player.SourceRect = DefaultSourcePlayer1;
                player.isAttacking = false;
                }
                
                //Atualização da posição do projetil
                if (IsKeyPressed(KEY_X)) {
                    dispararProjetil(&projetcilePlayer);
                    TimeProjectile = GetTime() + 2.0;
                }

                //Se o projetil estiver ativo, tera uma taxa de atualização da posição X
                if (projetcilePlayer.active) {
                    projetcilePlayer.Source.x += projetcilePlayer.speed.x;

                    if(GetTime() < TimeProjectile)
                        player.SourceRect = (Rectangle){390,570,82,80};
                    else    
                        player.SourceRect = DefaultSourcePlayer1;

                    //Se o projeto ultrapassar os limites da tela, ele deixa de ser ativo
                    if (projetcilePlayer.Source.x > WIDTHSCREEN){
                        projetcilePlayer.active = false;
                        projetcilePlayer = (Projectile){player,{player.posRect.x + 150,player.posRect.y + 30,100,100 },{200,120,490,420} ,(Vector2){15, 0}, false};
                    }
                    
                    if(CheckCollisionRecs(projetcilePlayer.Source,enemy.posRect)){
                        enemy.health -= player.attackDamage;
                        projetcilePlayer.active = false;
                        projetcilePlayer = (Projectile){player,{player.posRect.x + 150,player.posRect.y + 30,100,100 },{200,120,490,420} ,(Vector2){15, 0}, false};
                    }
                }

                if (IsKeyPressed(KEY_W) && !player.isJumping && countJump < 2) {
                    countJump += 1;
                    player.isJumping = true;
                    player.jumpForce = 150;
                }
                aplicarGravidade(&player, &countJump);


                //Controle do segundo personagem
                if (IsKeyDown(KEY_RIGHT)){
                    atualizarPersonagem(&enemy, 1, WIDTHSCREEN);
                    countSprite2 = ((int)GetTime())%3;
                    enemy.SourceRect = (Rectangle){100 * countSprite2,130,96,136};
                }else
                    enemy.SourceRect = (Rectangle){0,130,100,136};
                
                if (IsKeyDown(KEY_LEFT)){
                    atualizarPersonagem(&enemy, -1, WIDTHSCREEN);
                    countSprite2 = ((int)GetTime())%3;
                    enemy.SourceRect = (Rectangle){100 * countSprite2,130,96,136};
                }

                if (IsKeyPressed(KEY_M) && enemy.isAttacking == false) {
                enemy.isAttacking = true;
                timeAttack2 = GetTime() + 0.5; 
                }
                if (enemy.isAttacking && GetTime() < timeAttack2) {
                // Desenhe a animação de ataque
                enemy.SourceRect = (Rectangle){925, 670, 95, 150};
                if(controlHeight < 1){
                    enemy.posRect.y = enemy.posRect.y - 179;
                    controlHeight++;
                }
                if (CheckCollisionRecs(enemy.posRect, player.posRect)) {
                    player.health -= 1;
                }
                }else if(enemy.isAttacking && GetTime() >= timeAttack2) {
                // Se não estiver atacando ou o tempo de ataque acabar, volte para a animação padrão
                enemy.SourceRect = DefaultSourcePlayer2;
                enemy.posRect.y = DefaultPositionPlayer2.y;
                controlHeight = 0;
                enemy.isAttacking = false;
                }
                
                //Atualização da posição do projetil
                if (IsKeyPressed(KEY_N)) {
                    dispararProjetil(&projetcileEnemy);
                    TimeProjectile2 = GetTime() + 2.0;
                }
                //Se o projetil estiver ativo, tera uma taxa de atualização da posição X
                if (projetcileEnemy.active){
                    projetcileEnemy.Source.x -= projetcileEnemy.speed.x;

                    if(GetTime() < TimeProjectile2)
                        enemy.SourceRect = (Rectangle){820,286,140,110};
                    else    
                        enemy.SourceRect = DefaultSourcePlayer2;

                    //Se o projeto ultrapassar os limites da tela, ele deixa de ser ativo
                    if (projetcileEnemy.Source.x < 0){
                        projetcileEnemy.active = false;
                        projetcileEnemy = (Projectile){enemy,{enemy.posRect.x,enemy.posRect.y,100,100 },{0,0,220,220},{10, 0}, false};                   }
                    
                    if(CheckCollisionRecs(projetcileEnemy.Source,posRect2)){
                        player.health -= enemy.attackDamage;
                        projetcileEnemy.active = false;
                        projetcileEnemy = (Projectile){enemy,{enemy.posRect.x,enemy.posRect.y,100,100 },{0,0,220,220},{10, 0}, false};                    }
                }


                if (IsKeyPressed(KEY_UP) && !enemy.isJumping && countJump2 < 2) {
                    countJump2 += 1;
                    enemy.isJumping = true;
                    enemy.jumpForce = 150;
                }
                 if (CheckCollisionRecs(player.posRect, enemy.posRect)) {
        
        player.velocity *= FATOR_REDUCAO;
        enemy.velocity *= FATOR_REDUCAO;

        // Ajustar as posições dos personagens para evitar sobreposição
        if (player.posRect.x < enemy.posRect.x) {
            player.posRect.x -= 5;
            enemy.posRect.x += 5;
        } else {
            player.posRect.x += 5;
            enemy.posRect.x -= 5;
        }
    }
     // Verificar se o jogador pode atacar e aplicar o cooldown
            if (IsKeyPressed(KEY_C) && player.isAttacking == false && GetTime() > cooldownPlayer) {
                player.isAttacking = true;
                // Definir o tempo de cooldown para o momento atual mais o tempo de espera
                cooldownPlayer = GetTime() + COOLDOWN_ATTACK;
            }

            // Verificar se o inimigo pode atacar e aplicar o cooldown
            if (IsKeyPressed(KEY_M) && enemy.isAttacking == false && GetTime() > cooldownEnemy) {
                enemy.isAttacking = true;
                // Definir o tempo de cooldown para o momento atual mais o tempo de espera
                cooldownEnemy = GetTime() + COOLDOWN_ATTACK;
            }

            // Verificar o tempo de cooldown do jogador
            if (player.isAttacking && GetTime() >= cooldownPlayer) {
                // Reiniciar o estado de ataque do jogador e redefinir o tempo de cooldown
                player.isAttacking = false;
                cooldownPlayer = 0;
            }

            // Verificar o tempo de cooldown do inimigo
            if (enemy.isAttacking && GetTime() >= cooldownEnemy) {
                // Reiniciar o estado de ataque do inimigo e redefinir o tempo de cooldown
                enemy.isAttacking = false;
                cooldownEnemy = 0;
            }

                aplicarGravidade(&enemy, &countJump2);
                //Fim do controle do personagem 2//*/

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
                desenharCharacter(&player,texture5);
                desenharCharacter(&enemy,texture6);
                
                //Desenhando a base(piso) dos personagems
                DrawRectangle(0, 600, WIDTHSCREEN, 120, DARKBROWN);

                //Desenhando o titulo
                DrawTextEx(font, "Bloody War", textPosition3, 100, 2, RED);

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
                   DrawTexturePro(texture7,projetcilePlayer.Position,projetcilePlayer.Source,(Vector2){0, 0}, 0, WHITE);
                }
                if (projetcileEnemy.active == true) {
                   DrawTexturePro(texture8,projetcileEnemy.Position,projetcileEnemy.Source,(Vector2){0, 0}, 0, WHITE);
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