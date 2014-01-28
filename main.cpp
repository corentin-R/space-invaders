#ifdef __cplusplus
#include <cstdlib>
#else
#include <stdlib.h>
#endif

#include <SDL/SDL.h>

#include <SDL.h>
#include <SDL/SDL_ttf.h>
#include "opencv/highgui.h"
#include "opencv/cv.h"

#include <iostream>
#include <stdlib.h>
#include <stdio.h>

// Maths methods
#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) < (b) ? (a) : (b))
#define abs(x) ((x) > 0 ? (x) : -(x))
#define sign(x) ((x) > 0 ? 1 : -1)

// Step mooving for object min & max
#define STEP_MIN 5
#define STEP_MAX 100
#include "proto.h"
#include <unistd.h>



IplImage *image;

// Position of the object we overlay
CvPoint objectPos = cvPoint(-1, -1);
// Color tracked and our tolerance towards it
int h = 0, s = 0, v = 0, tolerance = 8;


//* Transform the image into a two colored image, one color for the color we want to track, another color for the others colors
// * From this image, we get two datas : the number of pixel detected, and the center of gravity of these pixel

CvPoint binarisation(IplImage* image, int *nbPixels)
{

    int x, y;
    CvScalar pixel;
    IplImage *hsv, *mask;
    IplConvKernel *kernel;
    int sommeX = 0, sommeY = 0;
    *nbPixels = 0;

    // Create the mask &initialize it to white (no color detected)
    mask = cvCreateImage(cvGetSize(image), image->depth, 1);

    // Create the hsv image
    hsv = cvCloneImage(image);
    cvCvtColor(image, hsv, CV_BGR2HSV);

    // We create the mask
    cvInRangeS(hsv, cvScalar(h - tolerance -1, s - tolerance, 0), cvScalar(h + tolerance -1, s + tolerance, 255), mask);

    // Create kernels for the morphological operation
    kernel = cvCreateStructuringElementEx(5, 5, 2, 2, CV_SHAPE_ELLIPSE);

    // Morphological opening (inverse because we have white pixels on black background)
    cvDilate(mask, mask, kernel, 1);
    cvErode(mask, mask, kernel, 1);

    // We go through the mask to look for the tracked object and get its gravity center
    for(x = 0; x < mask->width; x++)
    {
        for(y = 0; y < mask->height; y++)
        {

            // If its a tracked pixel, count it to the center of gravity's calcul
            if(((uchar *)(mask->imageData + y*mask->widthStep))[x] == 255)
            {
                sommeX += x;
                sommeY += y;
                (*nbPixels)++;
            }
        }
    }

    // Show the result of the mask image
    cvShowImage("Mask couleur selectionée", mask);

    // We release the memory of kernels
    cvReleaseStructuringElement(&kernel);

    // We release the memory of the mask
    cvReleaseImage(&mask);
    // We release the memory of the hsv image
    cvReleaseImage(&hsv);

   // printf("premier posx =%d\n",(int)635-(sommeX / (*nbPixels)));

    // If there is no pixel, we return a center outside the image, else we return the center of gravity
    if(*nbPixels > 0)
        return cvPoint((int)635-(sommeX / (*nbPixels)), (int)(sommeY / (*nbPixels)));
    else
        return cvPoint(-1, -1);
}


// Add a circle on the video that fellow your colored object

void addObjectToVideo(IplImage* image, CvPoint objectNextPos, int nbPixels)
{

    int objectNextStepX, objectNextStepY;

    // Calculate circle next position (if there is enough pixels)
    if (nbPixels > 10/*10*/)
    {

        // Reset position if no pixel were found
        if (objectPos.x == -1 || objectPos.y == -1)
        {
            objectPos.x = objectNextPos.x;
            objectPos.y = objectNextPos.y;
        }

        // Move step by step the object position to the desired position
        if (abs(objectPos.x - objectNextPos.x) > STEP_MIN)
        {
            objectNextStepX = max(STEP_MIN, min(STEP_MAX, abs(objectPos.x - objectNextPos.x) / 2));
            objectPos.x += (-1) * sign(objectPos.x - objectNextPos.x) * objectNextStepX;
        }
        if (abs(objectPos.y - objectNextPos.y) > STEP_MIN)
        {
            objectNextStepY = max(STEP_MIN, min(STEP_MAX, abs(objectPos.y - objectNextPos.y) / 2));
            objectPos.y += (-1) * sign(objectPos.y - objectNextPos.y) * objectNextStepY;
        }

        // -1 = object isn't within the camera range
    }
    else
    {

        objectPos.x =objectPos.x ;//*/-1;
        objectPos.y = objectPos.x ;//*/1;

    }

    // Draw an object (circle) centered on the calculated center of gravity
    if (nbPixels > 0/*10*/)
        cvDrawCircle(image, objectPos, 15, CV_RGB(255, 0, 0), -1);

    // We show the image on the window
    cvShowImage("tracking couleur", image);

}


//* Get the color of the pixel where the mouse has clicked
// We put this color as model color (the color we want to tracked)

void getObjectColor(int event, int x, int y, int flags, void *param = NULL)
{

    // Vars
    CvScalar pixel;
    IplImage *hsv;

    if(event == CV_EVENT_LBUTTONUP)
    {

        // Get the hsv image
        hsv = cvCloneImage(image);
        cvCvtColor(image, hsv, CV_BGR2HSV);

        // Get the selected pixel
        pixel = cvGet2D(hsv, y, x);

        // Change the value of the tracked color with the color of the selected pixel
        h = (int)pixel.val[0];
        s = (int)pixel.val[1];
        v = (int)pixel.val[2];

        // Release the memory of the hsv image
        cvReleaseImage(&hsv);

    }

}

using namespace std;
//---------------------------------------------------------------------------------------------------------------------------------------

//void initialiseMatrice(vector<proto> *inva, int x, int y, int nbLargeur, int nbHauteur, int espacementX, int espcementY, SDL_Surface* screen);

//void dessinerMatrice(proto *inva, SDL_Surface* screen, int nbLargeur, int nbHauteur);

//-----------------------------------------------------------------------------------------------------------------------------------------

int main ( int argc, char** argv )
{


    // Image & hsvImage
    IplImage *hsv;
    // Video Capture
    CvCapture *capture;
    // Key for keyboard event
    char key;

    // Number of tracked pixels
    int nbPixels;
    // Next position of the object we overlay
    CvPoint objectNextPos;

    // Initialize the video Capture (200 => CV_CAP_V4L2)
    capture = cvCreateCameraCapture(200);

    // Check if the capture is ok
    if (!capture)
    {
        printf("Can't initialize the video capture.\n");
        return -1;
    }

    // Create the windows
    cvNamedWindow("tracking couleur", CV_WINDOW_AUTOSIZE);
    cvNamedWindow("Mask couleur selectionée", CV_WINDOW_AUTOSIZE);
    cvMoveWindow("tracking couleur", 0, 100);
    cvMoveWindow("Mask couleur selectionée", 650, 100);

    // Mouse event to select the tracked color on the original image
    cvSetMouseCallback("tracking couleur", getObjectColor);

    // While we don't want to quit
    while(key != 'Q' && key != 'q')
    {

        // We get the current image
        image = cvQueryFrame(capture);

        // If there is no image, we exit the loop
        if(!image)
            continue;

        objectNextPos = binarisation(image, &nbPixels);
        addObjectToVideo(image, objectNextPos, nbPixels);

        // We wait 10 ms
        key = cvWaitKey(10);

    }

    // Destroy the windows we have created
    //  cvDestroyWindow("GeckoGeek Color Tracking");
    //cvDestroyWindow("GeckoGeek Mask");

    // Destroy the capture
    //cvReleaseCapture(&capture);
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
    // initialize SDL video
    if ( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        printf( "Unable to init SDL: %s\n", SDL_GetError() );
        return 1;
    }
    SDL_WM_SetCaption("Space Invaders 1.0", NULL);
    SDL_WM_SetIcon(SDL_LoadBMP("61-Vaio2 Game Controler CNT.bmp"), NULL);




    unsigned long int tempsPrecedent = 0, tempsActuel = 0;
    unsigned long int tempsPrecedent1 = 0, tempsActuel1 = 0;
    unsigned long int tempsPrecedentSprite = 0, tempsActuelSprite = 0;
    unsigned long int t1=0, t2=0;

    unsigned int compteurSprite=0;

    int m, n;
    int a,b;

    int compteurInvaVivant=0;

    //  int pause=1;//variable pause





    // create a new window


    SDL_Surface* ecran = SDL_SetVideoMode(LARGEURS, HAUTEURS+SCORE, 16, SDL_HWSURFACE|SDL_DOUBLEBUF/*|SDL_FULLSCREEN*/);


    SDL_Surface* screen ;//= SDL_SetVideoMode(768, 450, 16, SDL_HWSURFACE|SDL_DOUBLEBUF);
    screen = SDL_CreateRGBSurface(SDL_HWSURFACE, LARGEURS,HAUTEURS, 16, 0, 0, 0, 0); // Allocation de la surface

    SDL_Rect positiontScreen;
    positiontScreen.x = 0;
    positiontScreen.y = 0;
    SDL_BlitSurface(screen, NULL,ecran ,&positiontScreen);

    if ( !ecran )
    {
        printf("Unable to set 640x480 video: %s\n", SDL_GetError());
        return 1;
    }


    //--------initialisation écriture----------------------------------------------------------------
    TTF_Init(); // Initialisation
    TTF_Font *police = NULL;


    police = TTF_OpenFont("visitor2.ttf", 50);
    SDL_Color couleurblanche = {255, 255, 255};
    SDL_Color couleurbleu = {0, 0, 255};

    SDL_Surface *texte1 = NULL;
    SDL_Rect positiontt;
    texte1 = TTF_RenderText_Blended(police, "pause", couleurblanche);
    positiontt.x = LARGEURS/2-100;
    positiontt.y = HAUTEURS+10;
    // SDL_BlitSurface(texte1, NULL, screen, &positiontt); /* Blit du texte par-dessus */

    SDL_Surface *texte2 = NULL;
    SDL_Rect positiontt2;
    int compteurVies=3;
    char score1[20] = ""; /* Tableau de char suffisamment grand */
    sprintf(score1, "vies : %d", compteurVies);
    texte2 = TTF_RenderText_Blended(police, score1, couleurblanche);
    positiontt2.x = 20;
    positiontt2.y = HAUTEURS+10;

    SDL_Surface *texte3 = NULL;
    SDL_Rect positiontt3;
    int score=0;
    char score3[20] = ""; /* Tableau de char suffisamment grand */
    sprintf(score3, "score : %d", score);
    texte3 = TTF_RenderText_Blended(police, score3, couleurblanche);
    positiontt3.x = LARGEURS-200;
    positiontt3.y = HAUTEURS+10;


    //---------------------------------------------------------------------------------------------------


    //-----------création et dessin des limites du terrain-----------------------------------------------------------------------------------------


    SDL_Surface *limite1;
    SDL_Rect positionlimite1;
    positionlimite1.x=0;
    positionlimite1.y=0;
    limite1 = SDL_CreateRGBSurface(SDL_HWSURFACE, 10, screen->h, 32, 0, 0, 0, 0); // Allocation de la surface
    SDL_FillRect(limite1, NULL, SDL_MapRGB(screen->format, 125, 15, 15)); // Remplissage de la surface avec du blanc
    SDL_BlitSurface(limite1, NULL, screen, &positionlimite1); // Collage de la surface sur l'écran

    SDL_Surface *limite2;
    SDL_Rect positionlimite2;
    positionlimite2.x=screen->w-10;
    positionlimite2.y=0;
    limite2 = SDL_CreateRGBSurface(SDL_HWSURFACE, 10, screen->h, 32, 0, 0, 0, 0); // Allocation de la surface
    SDL_FillRect(limite2, NULL, SDL_MapRGB(screen->format, 125, 15, 15)); // Remplissage de la surface avec du blanc
    SDL_BlitSurface(limite2, NULL, screen, &positionlimite2); // Collage de la surface sur l'écran

    //-----------création des protagonistes du jeu-----------------------------------------------------------------------------------------

    proto tank;
    tank.initialiserTirZero();
    tank.chargerImg(screen,0);
    tank.centrer(screen);
    tank.dessiner(screen);
    tank.modifVie(3);
    printf("vies tank %d\n",tank.return_vie());

    int nbLargeur=5;
    int nbHauteur=4;

    // créer un tableau d'entiers vide
    proto inva[8][5];
    int aleax=0, aleay=0;


//---------initialiser variables pour bouger les invas------------------------------------------

    int invaXmin=50;
    int invaXmax=screen->w-120;

    int invaYmin=50;
    int invaYmax=screen->h-100;

    int invaXgauche=invaXmin;
    int invaYgauche=invaYmin;
    //printf("------>%d\n",invaXmax);

    int invaXdroit = invaXgauche+(100/*tailleBMPx(inva[0][0])*/)*nbLargeur-80;
    int invaYdroit = invaYgauche+(80/*tailleBMPy(inva[0][0])*/)*nbHauteur-10;
    // printf("---->%d\n",invaXdroit);

    int direction =1;

//---------------------------------------------------


    int i, j;

    for(i=0; i<nbHauteur; i++)
    {
        for(j=0; j<nbLargeur; j++)
        {
            inva[i][j].modifVie(1);

            inva[i][j].initialiserTirZero();

            inva[i][j].chargerImg(screen,1);

            inva[i][j].placer(invaXmin+(j*100),invaYmin+(i*80));

            inva[i][j].dessiner(screen);

            inva[i][j].modifVie(1);

            //  printf("inva[%d][%d]\n",i,j);
            // printf("x=%d, y=%d\n",invaXmin+(j*100),invaYmin+(i*80));

        }
    }




    /*inva[1][1].chargerImg(screen,1);
    inva[1][1].placer(50+(1*110),50+(2*77));
    inva[1][1].dessiner(screen);
    */

    //initialiseMatrice(&inva, 0, 0,  nbLargeur, nbHauteur, 110, 77, screen);
    //dessinerMatrice(&inva[16], screen, nbLargeur, nbHauteur);



//-------------------------------------------------------------------------------------------------


    SDL_EnableKeyRepeat(1, 1);


    // program main loop
    bool done = false;

    while (!done )
    {

        //****************************************************************************************************
        // boucle de definition du maque
        image = cvQueryFrame(capture);

        // If there is no image, we exit the loop
        if(!image)
            continue;

        objectNextPos = binarisation(image, &nbPixels);
        addObjectToVideo(image, objectNextPos, nbPixels);

        // We wait 10 ms
        key = cvWaitKey(10);

        //******************************************************************************************************
      //  float xtemp=objectNextPos.x/635;
       // printf("posx main=%f\n",xtemp);

        tank.deplacer(screen, 0,0,objectNextPos.x);
        t1 = SDL_GetTicks();
        if (t1 - t2 > 1) // Si 30 ms se sont écoulées depuis le dernier tour de boucle
        {
            t2=t1;

            // message processing loop
            SDL_Event event;
            while (SDL_PollEvent(&event))
            {

                // check for events
                switch (event.type)
                {
                    // exit if the window is closed
                case SDL_QUIT:
                    done = true;
                    break;

                case SDL_MOUSEBUTTONDOWN:
                    if (tank.return_t()==0) //si il n'y a plus de tir actif
                    {
                        tank.tirer(screen,0);
                        // printf("t tank=%d\n",tank.return_t());

                        tank.dessiner(screen);
                    }
                    break;

                    // check for keypresses
                case SDL_KEYDOWN:
                {
                    //tank.deplacer(screen, 0,0,objectNextPos.x);
                    // exit if ESCAPE is pressed
                    if (event.key.keysym.sym == SDLK_ESCAPE)
                        done = true;

                    else if (event.key.keysym.sym == SDLK_SPACE )
                    {
                        int continuer = 1;

                        SDL_Event event2;
                        while(continuer==1)
                        {
                            SDL_WaitEvent(&event2);
                            printf("pause \n");
                            SDL_BlitSurface(texte1, NULL, ecran, &positiontt); /* Blit du texte par-dessus */
                            SDL_Flip(ecran);



                            if(event2.type== SDL_MOUSEBUTTONDOWN)//the island
                            {
                                continuer=0;
                                printf("touche fin pause\n");
                                SDL_FillRect(screen, 0, SDL_MapRGB(screen->format, 0, 0, 0));   //on efface l'éran après la pause


                            }
                        }
                    }
                }

                } // end switch

            } // end of message processing

            compteurVies=tank.return_vie();//mise à jour de l'afficheur des vies du char
            //printf("vies tank %d\n",tank.return_vie());
            sprintf(score1, "vies : %d", compteurVies);
            texte2 = TTF_RenderText_Blended(police, score1, couleurblanche);
            //score=compteurVies;


            //-------------------------------------------------------------------------------------------------
            tempsActuelSprite = SDL_GetTicks();
            if (tempsActuelSprite - tempsPrecedentSprite >= 500) // Si 30 ms se sont écoulées depuis le dernier tour de boucle
            {
                //------------avancer les invas-----------------------------------------------

                if(direction==1) // gauche->droite
                {

                    for(i=0; i<nbHauteur; i++)
                    {
                        for(j=0; j<nbLargeur; j++)
                        {
                            if(inva[i][j].return_vie()==1)
                            {
                                inva[i][j].deplacer(screen,1,direction,objectNextPos.x);// gauche->droite
                            }
                        }
                    }

                    invaXgauche+=20;
                    invaXdroit+=20;

                    if(invaXdroit>=invaXmax)
                    {
                        direction=2;
                    }
                }

                else if (direction == 2)
                {

                    for(i=0; i<nbHauteur; i++)
                    {
                        for(j=0; j<nbLargeur; j++)
                        {
                            if(inva[i][j].return_vie()==1)
                            {
                                inva[i][j].deplacer(screen,1,direction,objectNextPos.x);// gauche->droite
                            }
                        }
                    }

                    invaXgauche-=20;
                    invaXdroit-=20;

                    if(invaXgauche<=invaXmin)
                    {
                        direction=1;
                    }
                }

                //------------------------------------------------------------------

                if(compteurSprite == 0)
                {
                    for(i=0; i<nbHauteur; i++)
                    {
                        for(j=0; j<nbLargeur; j++)
                        {
                            if(inva[i][j].return_vie()==1)
                            {
                                inva[i][j].chargerImg(screen,1);

                                inva[i][j].dessiner(screen);
                            }


                        }
                    }
                    compteurSprite = 1;
                }

                else if(compteurSprite == 1)
                {
                    for(i=0; i<nbHauteur; i++)
                    {
                        for(j=0; j<nbLargeur; j++)
                        {
                            if(inva[i][j].return_vie()==1)
                            {
                                inva[i][j].chargerImg(screen,2);

                                inva[i][j].dessiner(screen);
                            }
                        }
                    }
                    compteurSprite = 0;
                }



                tempsPrecedentSprite = tempsActuelSprite; // Le temps "actuel" devient le temps "precedent" pour nos futurs calculs
            }


            //----------------------------------------------------------------------------------------------

            //---tir inva---------------------------------------------------------------------------

            tempsActuel1 = SDL_GetTicks();
            if (tempsActuel1 - tempsPrecedent1 >= 2000) // Si 30 ms se sont écoulées depuis le dernier tour de boucle
            {
                do
                {
                    aleax=rand()%nbLargeur;                 // l'inva qui tire doit être en vie
                    aleay=rand()%nbHauteur;
                }
                while(inva[aleax][aleay].return_vie()!=1);

                inva[aleax][aleay].tirer(screen,1);
                inva[aleax][aleay].dessiner(screen);


                tempsPrecedent1 = tempsActuel1; // Le temps "actuel" devient le temps "precedent" pour nos futurs calculs
            }


            // tank.avancerTir(screen, inva[1][1]);
            inva[aleax][aleay].avancerTir(screen,1);


            tank.avancerTir(screen,0);
            //  printf("t tank=%d\n",tank.return_t());

            inva[aleax][aleay].verifTouche(screen, tank,1,score);
            for(a=0; a<nbHauteur; a++)
            {
                for(b=0; b<nbLargeur; b++)
                {
                    if(inva[a][b].return_vie()==1)
                    {
                        sprintf(score3, "score : %d", score);
                        texte3 = TTF_RenderText_Blended(police, score3, couleurblanche);
                        score=tank.verifTouche(screen, inva[a][b],0,score);
                        //printf("t tank=%d\n",tank.return_t());
                        //   printf("score=%d\n",score);
                    }
                }

            }


            //---------------------------

            SDL_FillRect(ecran, 0, SDL_MapRGB(screen->format, 0, 0, 0));

            //effacer ecran
            SDL_FillRect(screen, 0, SDL_MapRGB(screen->format, 0, 0, 0));
            SDL_BlitSurface(limite1, NULL, screen, &positionlimite1); // Collage de la limitede bord 1 sur l'écran
            SDL_BlitSurface(limite2, NULL, screen, &positionlimite2); // Collage de la limitede bord 2 sur l'écran


            tank.dessiner(screen);

            int k, l;

            for(k=0; k<nbHauteur; k++)
            {
                for(l=0; l<nbLargeur; l++)
                {
                    if(inva[k][l].return_vie()==1)
                    {
                        inva[k][l].dessiner(screen);
                        compteurInvaVivant++;
                    }
                }

            }

            if(compteurVies<1)
            {
                SDL_Surface *texte4 = NULL;
                SDL_Rect positiontt4;
                //int score=0;
               // char score3[20] = ""; /* Tableau de char suffisamment grand */

                texte4 = TTF_RenderText_Blended(police, "PERDU !", couleurblanche);
                positiontt4.x = LARGEURS/2-100;
                positiontt4.y = HAUTEURS+10;

                SDL_BlitSurface(texte4, NULL, ecran, &positiontt4);
                SDL_Flip(ecran);
                SDL_Delay(3000);
                done = true;
            }
            if(score==16)
            {
                SDL_Surface *texte4 = NULL;
                SDL_Rect positiontt4;
                //int score=0;
               // char score3[20] = ""; /* Tableau de char suffisamment grand */

                texte4 = TTF_RenderText_Blended(police, "GAGNE !", couleurblanche);
                positiontt4.x = LARGEURS/2-100;
                positiontt4.y = HAUTEURS+10;

                SDL_BlitSurface(texte4, NULL, ecran, &positiontt4);
                SDL_Flip(ecran);
                SDL_Delay(3000);
                done = true;
            }
            //---------------------------

            //    tank.dessiner(screen);

            //metre à jour écran
            SDL_Flip(screen);
            SDL_BlitSurface(screen, NULL,ecran ,&positiontScreen);

            SDL_BlitSurface(texte2, NULL, ecran, &positiontt2);
            SDL_BlitSurface(texte3, NULL, ecran, &positiontt3);
            // SDL_Flip(screen);
            SDL_Flip(ecran);


        }




    }// end main loop
    // Destroy the windows we have created
    cvDestroyWindow("tracking couleur");
    cvDestroyWindow("Mask couleur selectionée");

    // Destroy the capture
    cvReleaseCapture(&capture);

    // all is well ;)

    SDL_FreeSurface(screen);
    SDL_FreeSurface(ecran);

    // make sure SDL cleans up before exit
    atexit(SDL_Quit);

    printf("Exited cleanly\n");
    return 0;
}
