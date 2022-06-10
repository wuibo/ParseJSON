#include "jsonparser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
  FILE *fp;
  long lSize;
  char *buffer;
  struct object *json_root = malloc(sizeof *json_root);
  struct object *temp_json;
  int i,id;
  double *doub;
  int pnum;

  fp = fopen("example3.json", "rb");
  if (!fp)
    perror("blah.txt"), exit(1);

  fseek(fp, 0L, SEEK_END);
  lSize = ftell(fp);
  rewind(fp);

  /* allocate memory for entire content */
  buffer = calloc(1, lSize + 1);
  if (!buffer)
    fclose(fp), fputs("memory alloc fails", stderr), exit(1);

  /* copy the file into the buffer */
  if (1 != fread(buffer, lSize, 1, fp))
    fclose(fp), free(buffer), fputs("entire read fails", stderr), exit(1);

  /* do your work here, buffer is a string contains the whole text */
  i = read_json(json_root, buffer, lSize + 1);
  if (i < 0) {
    printf("Error JSON %d\n", i);
  } else {
    printf("ok JSON %d\n", i);
    temp_json = json_root->content;
    printf("Tamaño: %d\n", json_root->size);
    //printf("Name %s\n",temp_json[2].name);

    //numero
    if((id=json_get_item_by_name(*json_root,"player_count"))<0){
      printf("Player num no encontrado\n");
      fflush(stdout);
    }else{
      doub = temp_json[id].content;
      pnum = (int) *doub;
      printf("Número: %lf, %d\n",*doub,pnum);
      fflush(stdout);
    }
    
  }

  fclose(fp);
  free(buffer);
}