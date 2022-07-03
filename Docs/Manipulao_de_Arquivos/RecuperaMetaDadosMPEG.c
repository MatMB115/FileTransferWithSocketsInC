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

   arquivo = fopen("Robo.mpeg","rb+");
   int conta = 0;

   if(arquivo == NULL)
   {
      printf("\nArquivo MPEG nao pode ser aberto...");
      exit(0);
   }

   fseek(arquivo,0L,SEEK_END);
   fseek(arquivo,-(int) sizeof(MPEGInfo),SEEK_CUR);

   fread(&MPEGMetaDados,sizeof(MPEGInfo),1,arquivo);
   fclose(arquivo);

   printf("\nAutor: %s", MPEGMetaDados.Autor);
   printf("\nComentario: %s", MPEGMetaDados.Comentario);
}

