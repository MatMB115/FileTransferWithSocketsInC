#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
   char Autor[30];
   char Comentario[50];
} MPEGInfo;

int main()
{
   FILE *arquivo;
   char byte;

   MPEGInfo MPEGMetaDados;

   strcpy(MPEGMetaDados.Autor, "Jose da Silva");
   strcpy(MPEGMetaDados.Comentario, "Este v�deo demonstra um robo desorientado...");

   arquivo = fopen("Robo.mpeg","wb");
   int conta = 0;

   if(arquivo == NULL)
   {
      printf("\nArquivo MPEG nao pode ser aberto...");
      exit(0);
   }

   fseek(arquivo,0L,SEEK_END);

   fwrite(&MPEGMetaDados,sizeof(MPEGInfo),1,arquivo);
   fclose(arquivo);

}

