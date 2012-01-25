#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/glut.h>


/********************************SARWSH POLYGWNWN ME TON ALGORI8MO YX*************************************************************************************/


int screenWidth = 640;
int screenHeight = 480;
int length=0; //megethos pinaka koryfwn==#koryfwn-1
int startXY=0;

int menu; // for menu creation
int sel; // for menu choices


struct polyVertices{
	int x;
	int y;

};

struct polyVertices coordinates[10]; //syntetagmenes xrhsth apo to pontiki, gia sxediasmo polygwnou

struct poly
{
	int bucketSize;
	int maxY;
	int minY;
};

struct poly polygon; //idiotites polygwnou



struct edge
{
	int Xmax,Ymax;
	int Xmin,Ymin;
};



struct bucketLine
{
	int Ymax, Xmin, i_s,startX;
	int iter;
	double real_i_s;
	struct bucketLine *nxt;
};



/************************************************************************************************************************
			algorithmos XY
*************************************************************************************************************************/

void initXY()
{
	int maxY=0,i,minY=10000;
	int s;

	for (i=0;i<length;i++)
	{
		if (maxY<coordinates[i].y)
		{
			maxY=coordinates[i].y;
		}
		if (minY>coordinates[i].y)
		{
			minY=coordinates[i].y;
		}

	}
	s=maxY-minY;
	polygon.bucketSize=s;
	polygon.maxY=maxY;
	polygon.minY=minY;
	//printf("\nmaxY %d\n",maxY);
	//printf("\nminY %d\n",minY);

}


//pros8hkh sth 8esh bucket ta dedomena apo to addEdge h' ta dedomena apo add + sort_by_x
void add2bucket(struct bucketLine **bucket, struct edge *addEdge, struct bucketLine *addb)
{
	
	struct bucketLine *l;
	struct bucketLine *add;
	double tmp;

	int i_s;
	add =(struct bucketLine *) malloc(sizeof(struct bucketLine));
	if (addb==NULL){
		//round method
		//if (!(addEdge->Ymax-addEdge->Ymin)) { printf("division by zero!\n"); exit(-1); }
		tmp=(double)(addEdge->Xmax-addEdge->Xmin)/(double)(addEdge->Ymax-addEdge->Ymin);
		if (tmp>=0)
		{
			i_s=(int)(tmp + 0.5); //round (1/s)
		}
		else
		{
			i_s=(int)(tmp - 0.5); //round (1/s)
		}
			
		//printf("i_s %d\n",i_s);

		add->iter=0;
		add->startX = addEdge->Xmin;
		add->real_i_s=tmp;
		add->Xmin = addEdge->Xmin;
		add->Ymax = addEdge->Ymax;
		add->i_s = i_s;
		add->nxt=NULL;
	}
	else
	{
		add->iter=addb->iter;
		add->startX = addb->startX;
		add->real_i_s = addb->real_i_s;
		add->i_s = addb->i_s;
		add->Xmin = addb->Xmin;
		add->Ymax=addb->Ymax;
		add->nxt=NULL;

	}

	//printf("ftanei 1 !!\n\n"); //dbg
	//printf("addEdge.Xmin %d\n ",add->Xmin); //dbg

	if (*bucket==NULL)
	{
		*bucket = add;
	}
	else
	{
		l=*bucket;
		do
		{
			if (l==*bucket && l->nxt==NULL)
			{
				if(add->Xmin<=l->Xmin)
				{
					*bucket = add;
					add->nxt=l;
					break;
				}
				else
				{
					l->nxt=add; //(*l).nxt=add;

					break;
				}
			}
			else
			{
				if(add->Xmin >= l->Xmin)
				{
					if (l->nxt==NULL)
					{
						l->nxt=add;
						break;
					}
					else 
					{
						if(add->Xmin<=l->nxt->Xmin)
						{
							add->nxt=l->nxt;
							l->nxt=add;
							break;
						}
						
					}
				}
				else if(l==*bucket && add->Xmin < l->Xmin)
				{
					*bucket = add;
					add->nxt=l;
					break;
				}
			}
			l=l->nxt;
		}while(1);
	}
}






void XY()
{
	int i,j,k,temp;
	int from,to;
	struct edge *edges; //pinakas apo akmes tou polygwnou
	struct bucketLine **bucket; //pinakas apo listes me tis akmes (dedomena XY)
	struct bucketLine *l, *prev;
	struct bucketLine addb;

	
	initXY();

	
	edges= (struct edge *)malloc(length*sizeof(struct edge)); //pinakas pou periexei tis plevres
	//gemisma tou pinaka plevrwn
	//me to max/min o algori8mos symvolizeito panw/katw meros ths plevras antistoixa
	for (i=0;i<length;i++)
	{
		edges[i].Ymax = coordinates[i].y;
		edges[i].Xmax = coordinates[i].x;
		edges[i].Ymin = coordinates[(i+1)%(length+1)].y; //h teleftaia koryfh einai idia me tin prwth
		edges[i].Xmin = coordinates[(i+1)%(length+1)].x;
		
		//swstes times ws pros Ymin Xmin,Ymax Xmax gia ka8e plevra
		if (edges[i].Ymax<edges[i].Ymin)
		{
			temp=edges[i].Ymax;
			edges[i].Ymax=edges[i].Ymin;
			edges[i].Ymin=temp;
			
			temp=edges[i].Xmax;
			edges[i].Xmax=edges[i].Xmin;
			edges[i].Xmin=temp;
		}
	}

	//uncomment gia ektypwsh stoixeiwn xrhsth
	/*for (i=0;i<length;i++)
	{
		printf(" Xmin %d Ymin %d, Xmax %d Ymax %d \n",edges[i].Xmin,edges[i].Ymin,edges[i].Xmax,edges[i].Ymax);
	}*/


	//************
	//dhmiourgia listas grammwn sarwshs
	bucket = (struct bucketLine **)malloc(polygon.bucketSize*sizeof(struct bucketLine));
	for (i=0;i<polygon.bucketSize;i++)
	{
		bucket[i]=NULL;
	}
	
	//uncomment gia ektypwsh stoixeiwn polygwnou
	/*printf("polygon.bucketSize %d\n",polygon.bucketSize);
	printf("polygon.minY %d\n",polygon.minY);
	printf("polygon.maxY %d\n",polygon.maxY);
	printf("length %d\n",length);*/


	//************
	//pros8hkh arxikwn plevrwn sto bucket kai taksinomhsh ws prox Xmin
	for(i=0; i<polygon.bucketSize; i++)
	{
		for(j=0; j<length; j++)
		{
			if (edges[j].Ymin==polygon.minY + i)
			{
				add2bucket(&bucket[i], &edges[j], NULL);
			}
		}
	}


	/****************TEST ADD START***********************************************************************************/
		//edges[0].Ymax = coordinates[i].y;
		//edges[0].Xmax = coordinates[i].x;
		//edges[0].Ymin = coordinates[(i+1)%(length+1)].y; //h teleftaia koryfh einai idia me tin prwth
		
		//printf("ftanei!!\n\n"); //dbg
		//edges[0].Xmin = 10;
		//add2bucket(&bucket[0], &edges[0], NULL);
		//edges[0].Xmin = 1000;
		//add2bucket(&bucket[0], &edges[0], NULL);
		//edges[0].Xmin = 9;
		//add2bucket(&bucket[0], &edges[0], NULL);
		//edges[0].Xmin = 11;
		//add2bucket(&bucket[0], &edges[0], NULL);
		//edges[0].Xmin = 11;
		//add2bucket(&bucket[0], &edges[0], NULL);	
	/****************TEST ADD END***********************************************************************************/

	//uncomment gia ektypwsh arxikis listas
	/*for(i=0; i<polygon.bucketSize; i++)
	{
		if (bucket[i]!=NULL)
		{
			printf("point3\n");
			printf("{line %d Xmin %d Ymax %d i_s %d ->}  ",
				polygon.minY+i,
				bucket[i]->Xmin,
				bucket[i]->Ymax,
				bucket[i]->i_s);
			for(l=bucket[i]->nxt;l!=NULL;l=l->nxt)
			{
				printf("{line %d Xmin %d Ymax %d i_s %d ->} ",
					polygon.minY+i,
					l->Xmin,
					l->Ymax,
					l->i_s);
			}
			printf("\n");
		}
	}*/

	//************
	//vhma 2 algorithmou
	//diatreksh ths listas se ka8e 8esh tou pinaka (bucket) kai update twn stoixeiwn 
	/*
	!!prepei ka8e stoixeio ths lista na exei ton diko tou metrhth
	giati me enan koino metrhth to sfalma/apoklhsh tou Xmin  einai megalo (meta apo enan ari8mo stoixeio)!!
	*/
	
	for(i=1; i<polygon.bucketSize; i++)
	{
		//pros8hkh prohgoumenwn stoixeiwn
		for(l=bucket[i-1];l!=NULL;l=l->nxt)
		{
			addb.real_i_s = l->real_i_s;
			addb.startX = l->startX;
			addb.iter=l->iter+1;
				//addb.i_s = l->i_s;
			k=addb.iter;
			addb.i_s =(int)(((double)k)*addb.real_i_s);
			addb.Ymax = l->Ymax;
			addb.nxt = NULL;
				//addb.Xmin = l->Xmin + addb.i_s;
			addb.Xmin = addb.startX + addb.i_s;
			add2bucket(&bucket[i], NULL, &addb);
		}




		//uncomment gia ektypwsh listas prin thn diagrafh stoixeiwn
		/*if (bucket[i]!=NULL)
		{
			printf("NEW\n");

			for(l=bucket[i];l!=NULL;l=l->nxt)
			{
				printf("{line %d Xmin %d Ymax %d i_s %d ->} ",
					polygon.minY+i,
					l->Xmin,
					l->Ymax,
					l->i_s);
			}
			printf("\n");
			if (bucket[i]==NULL){printf("\nADD_PRBLM_OK\n");}

			printf("\n");
		}*/

	
		//remove from list if equal
		k=0;
		for(l=bucket[i],prev=l; l!=NULL; prev=l, l=l->nxt)
		{
			k++;
			if(l->Ymax == i+polygon.minY)
			{
				if(l->nxt != NULL){
					while(l->nxt->Ymax == i+polygon.minY) //prosperash tyxwn omoiwn stoixeiwn se seira
					{
						l=l->nxt;
						if (l->nxt==NULL) break;
					}
				}
				if (prev==bucket[i]&&k==1){
					bucket[i]=l->nxt;
				}
				else{
					prev->nxt=l->nxt; //rmv from list
			
				}
			}
		}




	}
		

	//uncomment gia ektypwsh telikhs listas
	//printf("FINAL LIST\n");
	//for(i=0; i<polygon.bucketSize; i++)
	//{
	//		//check remv list
	//	if (bucket[i]!=NULL)
	//	{
	//		
	//		printf("\n line %d: ", polygon.minY+i);
	//		for(l=bucket[i];l!=NULL;l=l->nxt)
	//		{
	//			printf("{ Xmin %d Ymax %d i_s %d ->} ",
	//				
	//				l->Xmin,
	//				l->Ymax,
	//				l->i_s);
	//		}
	//		printf("\n");
	//	}
	//}

	//************************
	//vapsimo pixels
	for(i=0; i<polygon.bucketSize; i++)
	{
		for(l=bucket[i];l!=NULL;l=l->nxt->nxt)
		{
			from = l->Xmin;
			to = l->nxt->Xmin;
			glBegin (GL_POINTS);
			for(k=from; k<to; k++)
			{
				glVertex2i(k,i+polygon.minY);
			}
			glEnd();
			glFlush();
		}
	}

	//printf("%d",(int)((double)8/(double)6+0.5)); //example of round
	
	free(bucket);
	free(edges);

}


/************************************************************************************************************************
		openGL functions
*************************************************************************************************************************/
void mainDisplay(void){
  glClear(GL_COLOR_BUFFER_BIT);
  glFlush();

}

void init(){
	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(0,640,0, 480); 
	glClearColor(0,0,0,0);
  	glClear(GL_COLOR_BUFFER_BIT);
	glFlush();
	

}


void keyEvent(unsigned char key, int x, int y)
{


  switch (key)
  {
	//Quit
	case 'Q':
	case 'q':
	exit(-1);
	break;
	default:
	break;	
  }
}



/************************************************************************************************************************
		sxediasmos polygwnou me pontiki
*************************************************************************************************************************/

void drawPol(int vertex)
{
	int tempx,tempy,tol=20;
	if (vertex==0)
	{
		glBegin(GL_POINTS);
		glColor3f(0,0,0);
		glVertex2i(coordinates[0].x,coordinates[1].y);

		glEnd();
	}
	else
	{
		tempx=coordinates[vertex].x;
		tempy=coordinates[vertex].y;
		//printf(" %d ",coordinates[vertex].y);
		//printf(" %d ",coordinates[0].y);

		//ennwsh ths teleftaias koryfhs me thn  prwth gia to sxhmatismo tou polygvnou
		if ((tempx > coordinates[0].x-tol)&& (tempy< coordinates[0].y + tol)&& (tempx < coordinates[0].x+tol)&& (tempy > coordinates[0].y-tol))
		{
			
			coordinates[vertex].x = coordinates[0].x;
			coordinates[vertex].y = coordinates[0].y;
			length=vertex;
			startXY=1; //start algorithm XY

		}
		glBegin(GL_LINES); //ennwsh twn grammwn analoga me tis syntetagmenes tou xrhsth pao to pontiki
		glColor3f(1,0,0);
		glVertex2i(coordinates[vertex-1].x,coordinates[vertex-1].y);
		glVertex2i(coordinates[vertex].x,coordinates[vertex].y);
		glEnd();

	
	}
	glFlush();
	//if (startXY) XY();

}



int timesPushed=0;
void myMouse(int button, int state, GLint x, GLint y)
{


	if(button==GLUT_LEFT_BUTTON && state==GLUT_DOWN&&sel==1)
	{
	
		coordinates[timesPushed].x=x;
		coordinates[timesPushed].y=screenHeight - y; //apo8ikefsi ston pinaka se zevgh x,y

		drawPol(timesPushed);
	
		if (timesPushed < 10)
		{
			timesPushed++;
		}
		else
		{
			printf("\nERROR vertices > 10!!\n");
			exit(-1);
		}
	}
	

}


void myMenu(int selection)
{
	if(selection==1) 
	{
		//glutPostRedisplay(); 
		sel=1;
		
	}
	if(selection==2)
	{
		sel=2;
		XY();
	}
	if(selection==3) 
	{
		
		sel=3;
		timesPushed=0;
		glutPostRedisplay();
		
		
	}
}




/************************************************************************************************************************
			LAB2
*************************************************************************************************************************/

int main (int argc, char *argv[]){



  glutInit(&argc, argv);
  glutInitWindowPosition(50, 50);
  glutInitWindowSize(640, 480);
  glutInitDisplayMode(GLUT_SINGLE|GLUT_RGB);
  glutCreateWindow("lab2");
  init();

  glutDisplayFunc(mainDisplay);
  glutKeyboardFunc(keyEvent);
  glutMouseFunc(myMouse);



    // menu creation
  menu = glutCreateMenu(myMenu);

  glutAddMenuEntry("sxediash polygwnou MAX_edges 10", 1);
  glutAddMenuEntry("xrwmatismos", 2);
  glutAddMenuEntry("ka8arismos", 3);
	
  // register menu to right mouse button
  glutAttachMenu(GLUT_RIGHT_BUTTON);

  glutMainLoop();
  
	exit(-1);

}