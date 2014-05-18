int LEV_ROW;
int LEV_COL;
int *levelMatrixData;

//ritorna il valore del livello come coordinate
int levelMatrix(int a,int b)
{
	return levelMatrixData[b+a*LEV_COL];
}
void readMatrix()
{
	int width,height;
	
	//apro il file
	FILE *livello=fopen("livello.txt","r");

	//leggo le dimensioni e le salvo nella variabile globale
    fscanf(livello,"%d,%d\n",&width,&height);
	LEV_ROW=height;
	LEV_COL=width;

	//creo l'array del livello
	levelMatrixData=new int [height*width];

	

	//leggo il livello e lo memorizzo nell'array
	for (int j=0; j<width*height; j++)
		{
			int num;
			fscanf(livello,"%d,",&num);
			levelMatrixData[j]=num;
		}
	
}