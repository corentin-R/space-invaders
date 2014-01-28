#ifdef __cplusplus
#include <cstdlib>
#else
#include <stdlib.h>
#endif
#ifdef __APPLE__
#include <SDL/SDL.h>
#else
#include <SDL.h>
#endif

//#include <SDL.h>
//#endif
#include <string>
#include <time.h>
#include <SDL/SDL_ttf.h>

#define LARGEURS 800//800
#define HAUTEURS 600//700
#define SCORE  50


class proto
{

public:
    //proto();
    void initialiserTirZero();
    void chargerImg(SDL_Surface* screen,int type);
    void deplacer(SDL_Surface* screen, int type, int direction, int posx);
    void centrer(SDL_Surface* screen);
    void dessiner(SDL_Surface* screen);
    void tirer(SDL_Surface* screen,int type);
    void avancerTir(SDL_Surface* screen,int type);
    int verifTouche(SDL_Surface* screen, proto &cible,int type,int score);

    void placer(int x, int y);

    void changerDirection(int d);

    int tailleBMPx(proto &objet);
    int tailleBMPy(proto &objet);

    int positionX(proto &objet);
    int positionY(proto &objet);

    void modifVie(int vieSuivante);


    int return_t();
    int return_vie();

private:

    SDL_Surface* bmp;
    SDL_Rect dstrect;

    SDL_Surface *raq1;
    SDL_Rect position1;
    int t;

    int vie;


};


//#endif // PROTO_H_INCLUDED
