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


#include <string>
#include <time.h>
#include <SDL/SDL_ttf.h>

#include "proto.h"



using namespace std;

/*proto::proto()
{
    SDL_Surface* bmp = SDL_LoadBMP("cb.bmp");
    SDL_Rect dstrect;
}*/
void proto::initialiserTirZero()
{
    t=0;
}

void proto::chargerImg(SDL_Surface* screen,int type)
{
    // load an image
    if(type==0)
    {
        bmp = SDL_LoadBMP("tank.bmp");
    }

    else if (type == 1)
    {
        bmp = SDL_LoadBMP("inva.bmp");
    }

    else if(type == 2)
    {
        bmp = SDL_LoadBMP("inva2.bmp");
    }



}
//------------------------------------------------------------------------------------------------------------------------------------

void proto::centrer(SDL_Surface* screen)
{
    // centre the bitmap on screen
    //SDL_Rect dstrect;
    dstrect.x = (screen->w - bmp->w) / 2;
    dstrect.y = (screen->h - bmp->h);
}


//------------------------------------------------------------------------------------------------------------------------------------

void proto::deplacer(SDL_Surface* screen, int type, int direction, int posx)
{
    if(type==0)//déplacement char
    {

        int pas=7;


        if(dstrect.x <= (screen->w-bmp->w)-10 && dstrect.x >=0+10
                && dstrect.y <= (screen->h-bmp->h) && dstrect.y >=0)
        {

            //SDL_Event event;
            //   SDL_WaitEvent(&event);

            /*  if (event.key.keysym.sym == SDLK_UP)
              {
                  (dstrect).y=(dstrect).y-3;

              }

              else if (event.key.keysym.sym == SDLK_DOWN)
              {
                  (dstrect).y=(dstrect).y+3;
              }*/

            /* if (event.key.keysym.sym == SDLK_LEFT)
             {
                 (dstrect).x=(dstrect).x-pas;
             }

             else if (event.key.keysym.sym == SDLK_RIGHT)
             {
                 (dstrect).x=(dstrect).x+pas;
             }*/


            dstrect.x=/*(int)*/posx;/*LARGEURS;
            printf("posx=%f\n",posx);
            printf("posx converti=%d\n",dstrect.x);*/

        }

        else if(dstrect.x >= (screen->w-bmp->w)-10)
        {
            (dstrect).x=(dstrect).x-pas;
        }


        else if(dstrect.x <= 0+10)
        {
            (dstrect).x=(dstrect).x+pas;
        }
    }

    else if(type ==1)//déplacement invas
    {
        if (direction==1)//gauche->droite
        {
            dstrect.x=dstrect.x+20;
        }

        else if (direction==2)//droite->gauche
        {

            dstrect.x=dstrect.x-20;

        }
    }
}


//-----------------------------------------------------------------------------------------------------------------------------------------
void proto::dessiner(SDL_Surface* screen)
{

    // clear screen
//   SDL_FillRect(screen, 0, SDL_MapRGB(screen->format, 0, 0, 0));



    // draw bitmap
    SDL_BlitSurface(bmp, 0, screen, &dstrect);

    // DRAWING ENDS HERE

    // finally, update the screen :)
//   SDL_Flip(screen);

    // free loaded bitmap
    // SDL_FreeSurface(bmp);

    // DRAWING STARTS HERE
    if(t == 1)
    {
        SDL_FillRect(raq1, NULL, SDL_MapRGB(screen->format, 0, 255, 0)); // Remplissage de la surface avec du blanc
        SDL_BlitSurface(raq1, NULL, screen, &position1); // Collage de la surface sur l'écran
        //  SDL_Flip(screen);
    }


}
//------------------------------------------------------------------------------------------------------------------------------------
void proto::tirer(SDL_Surface* screen,int type)
{
    //raq1 = NULL;
    if(type==0)//char qui tire
    {
        raq1 = SDL_CreateRGBSurface(SDL_HWSURFACE, 10, 50, 32, 0, 0, 0, 0); // Allocation de la surface
    }
    else if(type==1)//inva qui tire
    {
        raq1 = SDL_CreateRGBSurface(SDL_HWSURFACE, 10, 50, 32, 0, 250, 0, 0); // Allocation de la surface
    }

    position1.x = dstrect.x+(bmp->w/2-10);
    position1.y = dstrect.y-30;


    t=1;


    printf("-------tirer----------------\n");
    printf("position1.x =%d\n",position1.x );
    printf("position1.y =%d\n",position1.y );
    printf("-----------------------\n");
}

//----------------------------------------------------------------------------------------------------------------------------------
void proto::avancerTir(SDL_Surface* screen, int type)
{
    // printf("position1.y =%d\n",position1.y );

    if(t==1 && type==0 )//char qui tire
    {
        position1.y=position1.y-30;


        /* if(position1.y <= 1)
         {
             t=0;
             SDL_FreeSurface(raq1);

         }*/
    }

    else if (t==1 && type== 1)//inva qui tire
    {
        position1.y=position1.y+30;


        /* if(position1.y  >= 700);
          {
              t=0;
              SDL_FreeSurface(raq1);

          }*/
    }

    /*
            printf("-----------------------\n");
            printf("inva.x =%d\n",cible.dstrect.x );
            printf("inva.y =%d\n",cible.dstrect.y );
            printf("inva.cible.bmp->w =%d\n",cible.bmp->w );
            printf("inva.cible.bmp->h =%d\n",cible.bmp->h );
            printf("-----------------------\n");
            printf("position1.x =%d\n",position1.x );
            printf("position1.y =%d\n",position1.y );
            printf("-----------------------\n");*/
}
//-----------------------------------------------------------------------------------------------------------------------------------

void proto::placer( int x, int y)
{
    dstrect.x = x;
    dstrect.y = y;
}

//--------------------------------------------------------------------------------------------------------------------------------------

int proto::verifTouche(SDL_Surface* screen, proto &cible,int type, int score)
{

    if(type==0 )//char qui tire
    {

        if(position1.y <= 1)
        {
            t=0;
            /*position1.y=position1.y;
            SDL_FreeSurface(raq1);*/

        }

        if(position1.y <= (cible.dstrect.y+cible.bmp->h/*-10*/)
                && position1.x <= (cible.dstrect.x+cible.bmp->w)
                && position1.x >= (cible.dstrect.x)
                && position1.y>=2)
        {
            t=0;

            printf(" inva touché\n");
            //cible.placer(screen->h,screen->w);
            cible.modifVie(0);
            score++;


            position1.y=0;
            position1.x=0;

            //   SDL_FreeSurface(raq1);


        }
    }


//******************************************************************************************


    else if ( type== 1)//inva qui tire
    {

        if(position1.y  >= screen->h)
        {
            t=0;
            //position1.y=position1.y;
            // SDL_FreeSurface(raq1);
        }

        if(position1.y+50 >= (cible.dstrect.y)
                && (position1.x <= (cible.dstrect.x+cible.bmp->w))
                && (position1.x >= (cible.dstrect.x))
                && position1.y<=screen->h-2)
        {
            t=0;
            printf("char touché\n");
            //cible.centrer(screen);

            cible.modifVie(0);

            position1.y=screen->h;
            position1.x=screen->w;
            //    SDL_FreeSurface(raq1);

        }
    }
    return score;

}


//--------------------------------------------------------------------------------------------------------------------

int proto::tailleBMPx(proto &objet)
{
    return objet.bmp->w;
}


int proto::tailleBMPy(proto &objet)
{
    return objet.bmp->h;
}
//--------------------------------------------------------------------------------------------------------------------------

int proto::positionX(proto &objet)
{
    return dstrect.x;
}

int proto::positionY(proto &objet)
{
    return dstrect.y;
}

//--------------------------------------------------------------------------------------------------------------------------------


void proto::modifVie(int vieSuivante)
{

    if(vieSuivante==0)
    {
        vie--;
    }
    else
    {
        vie = vieSuivante;
    }

}

//---------------------------------------------------------------------------------------------------------------

int proto::return_t()
{
    return t;
}

//----------------------------------------------------------------------------------------------------------------------
int proto::return_vie()
{
    return vie;
}










