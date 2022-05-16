#include "jsonparser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define s_start 0
#define s_namestart 1
#define s_name 2
#define s_nameend 3
#define s_type 4
#define s_array 5
#define s_object_name 8
#define s_object_nameend 9
#define s_object_type 10

unsigned short read_json(struct object *object, char *json, unsigned int size) {
  short state = s_start;
  struct object *json_root;
  struct object *json_temp;
  struct object *temp_object;
  unsigned int *object_count;
  unsigned int point;
  unsigned short *stack_type;
  unsigned short *stack_tmp;
  unsigned short stack_level = 0;
  char *temp_char;
  char *back_char;
  unsigned char *temp_int;
  double *temp_num;
  unsigned int char_size, i;
  char x_char;
  char hex_char[4];
  unsigned short decimal;


  object->type = json_type_object;
  stack_type = malloc(sizeof(unsigned short));
  stack_type[0] = json_type_object;
  object_count = malloc(sizeof(unsigned int));
  object_count[0] = 0;

  for (point = 0; point < size; point++) {
    switch (state) {
    case s_start:
      // Inicio  del json se espera withespace o { para empezar
      switch (json[point]) {
      case ' ':
        break;
      case '\n':
        break;
      case '\r':
        break;
      case '\t':
        break;
      case '{':
        // comienza
        state = s_namestart;
        break;
      default:
        // error de ejecución
        return -1;
        break;
      }
      break;
    case s_namestart:
      // Comienzo de nombre de objetos se espera withespace o "
      switch (json[point]) {
      case ' ':
        break;
      case '\n':
        break;
      case '\r':
        break;
      case '\t':
        break;
      case '"':
        // comienza el string
        char_size = 0;
        state = s_name;
        break;
      default:
        // error de ejecución
        return -1;
        break;
      }
      break;
    case s_name:
      //cargando nombre. cualquier caracter cuidado con los especiales
      switch (json[point]) {
      case '"':
        // fin de cadena crear objeto para guardar nombre
        json_temp = json_root;
        json_root =
            malloc((object_count[stack_level] + 1) * sizeof(struct object));
        for (i = 0; i < object_count[stack_level]; i++) {
          json_root[i] = json_temp[i];
        }
        free(json_temp);
        json_root[object_count[stack_level]].name = temp_char;
        object_count[stack_level]++;
        state = s_nameend;
        char_size = 0;
        break;
      case '\\':
        // comprobar sigiguiente
        point++;
        switch (json[point]) {
        case '"':
          x_char = '"';
          break;
        case '/':
          x_char = '/';
          break;
        case '\\':
          x_char = '\\';
          break;
        case 'b':
          x_char = '\b';
          break;
        case 'f':
          x_char = '\f';
          break;
        case 'n':
          x_char = '\n';
          break;
        case 'r':
          x_char = '\r';
          break;
        case 't':
          x_char = '\t';
          break;
        case 'u':
          hex_char[0] = json[point + 1];
          hex_char[1] = json[point + 2];
          hex_char[2] = json[point + 3];
          hex_char[3] = json[point + 4];
          point += 4;
          x_char = strtol(hex_char, 0, 16);
          break;
        default:
          return -1;
        }
        back_char = temp_char;
        temp_char = malloc((char_size + 1) * sizeof(char));
        for (i = 0; i < char_size; i++) {
          temp_char[i] = back_char[i];
        }
        free(back_char);
        temp_char[char_size] = x_char;
        char_size++;
        break;
      default:
        // almacenar el siguiente char
        back_char = temp_char;
        temp_char = malloc((char_size + 1) * sizeof(char));
        for (i = 0; i < char_size; i++) {
          temp_char[i] = back_char[i];
        }
        free(back_char);
        temp_char[char_size] = json[point];
        char_size++;
        break;
      }
      break;
    case s_nameend:
      // esperar : para buscar tipo (witespace o :)
      switch (json[point]) {
      case ' ':
        break;
      case '\n':
        break;
      case '\r':
        break;
      case '\t':
        break;
      case ':':
        // detectar tipo
        state = s_type;
        break;
      default:
        // error de ejecución
        return -1;
        break;
      }
      break;
    case s_type:
      // witespace o inicio de tipo
      switch (json[point]) {
      case ' ':
        break;
      case '\n':
        break;
      case '\r':
        break;
      case '\t':
        break;
      case 't':
        // true
        if ((json[point + 1] == 'r' && json[point + 2] == 'u' &&
             json[point + 3] == 'e') ||
            (json[point + 1] == 'R' && json[point + 2] == 'U' &&
             json[point + 3] == 'E')) {
          // correcto
          json_root[object_count[stack_level]].type = json_type_boolean;
          temp_int = malloc(sizeof(unsigned short));
          temp_int[0] = 1;
          json_root[object_count[stack_level]].content = temp_int;
          json_root[object_count[stack_level]].size = 1;
          point += 3;
        } else {
          // erroneo
          return -1;
        }
        break;
      case 'T':
        // true
        if ((json[point + 1] == 'r' && json[point + 2] == 'u' &&
             json[point + 3] == 'e') ||
            (json[point + 1] == 'R' && json[point + 2] == 'U' &&
             json[point + 3] == 'E')) {
          // correcto
          json_root[object_count[stack_level]].type = json_type_boolean;
          temp_int = malloc(sizeof(unsigned short));
          temp_int[0] = 1;
          json_root[object_count[stack_level]].content = temp_int;
          json_root[object_count[stack_level]].size = 1;
          point += 3;
        } else {
          // erroneo
          return -1;
        }
        break;
      case 'f':
        // false
        if ((json[point + 1] == 'a' && json[point + 2] == 'l' &&
             json[point + 3] == 's' && json[point + 4] == 'e') ||
            (json[point + 1] == 'A' && json[point + 2] == 'L' &&
             json[point + 3] == 'S' && json[point + 4] == 'E')) {
          // correcto
          json_root[object_count[stack_level]].type = json_type_boolean;
          temp_int = malloc(sizeof(unsigned short));
          temp_int[0] = 0;
          json_root[object_count[stack_level]].content = temp_int;
          json_root[object_count[stack_level]].size = 1;
          point += 4;
        } else {
          // erroneo
          return -1;
        }
        break;
      case 'F':
        // false
        if ((json[point + 1] == 'a' && json[point + 2] == 'l' &&
             json[point + 3] == 's' && json[point + 4] == 'e') ||
            (json[point + 1] == 'A' && json[point + 2] == 'L' &&
             json[point + 3] == 'S' && json[point + 4] == 'E')) {
          // correcto
          json_root[object_count[stack_level]].type = json_type_boolean;
          temp_int = malloc(sizeof(unsigned short));
          temp_int[0] = 0;
          json_root[object_count[stack_level]].content = temp_int;
          json_root[object_count[stack_level]].size = 1;
          point += 4;
        } else {
          // erroneo
          return -1;
        }
        break;
      case 'n':
        // null
        if ((json[point + 1] == 'u' && json[point + 2] == 'l' &&
             json[point + 3] == 'l') ||
            (json[point + 1] == 'U' && json[point + 2] == 'L' &&
             json[point + 3] == 'L')) {
          // correcto
          json_root[object_count[stack_level]].type = json_type_null;
          json_root[object_count[stack_level]].size = 1;
        } else {
          // erroneo
          return -1;
        }
        break;
      case 'N':
        // null
        if ((json[point + 1] == 'u' && json[point + 2] == 'l' &&
             json[point + 3] == 'l') ||
            (json[point + 1] == 'U' && json[point + 2] == 'L' &&
             json[point + 3] == 'L')) {
          // correcto
          json_root[object_count[stack_level]].type = json_type_null;
          json_root[object_count[stack_level]].size = 1;
          point += 3;
        } else {
          // erroneo
          return -1;
        }
        break;
      case '[':
        // array
        state = s_array;
        json_root[object_count[stack_level]].type = json_type_array;
        stack_tmp = stack_type;
        stack_type = malloc((stack_level + 1) * sizeof(unsigned short));
        for (i = 0; i < stack_level; i++) {
          stack_type[i] = stack_tmp[i];
        }
        stack_type[stack_level] = json_type_array;
        stack_level++;
        break;
      case '"':
        // string
        json_root[object_count[stack_level]].type = json_type_string;
        point++;
        char_size = 0;
        while (json[point] != '"') {
          // cuidado con los \ es un string de JSON
          if (json[point] == '\\') {
            point++;
            switch (json[point]) {
            case '"':
              x_char = '"';
              break;
            case '/':
              x_char = '/';
              break;
            case '\\':
              x_char = '\\';
              break;
            case 'b':
              x_char = '\b';
              break;
            case 'f':
              x_char = '\f';
              break;
            case 'n':
              x_char = '\n';
              break;
            case 'r':
              x_char = '\r';
              break;
            case 't':
              x_char = '\t';
              break;
            case 'u':
              hex_char[0] = json[point + 1];
              hex_char[1] = json[point + 2];
              hex_char[2] = json[point + 3];
              hex_char[3] = json[point + 4];
              point += 4;
              x_char = strtol(hex_char, 0, 16);
              break;
            default:
              return -1;
            }
            back_char = temp_char;
            temp_char = malloc((char_size + 1) * sizeof(char));
            for (i = 0; i < char_size; i++) {
              temp_char[i] = back_char[i];
            }
            free(back_char);
            temp_char[char_size] = x_char;
            char_size++;
          }else{
            //cualquier otro caracter
                        back_char = temp_char;
            temp_char = malloc((char_size + 1) * sizeof(char));
            for (i = 0; i < char_size; i++) {
              temp_char[i] = back_char[i];
            }
            free(back_char);
            temp_char[char_size] = json[point];
            char_size++;
          }
          point++;
        }
        //guardar la cadena
        json_root[object_count[stack_level]].content = temp_char;
        json_root[object_count[stack_level]].size = char_size;
        break;
      default:
        // numérico
        if(json[point]<='0'||json[point]>='9'){
          //no es númerico
          return -1;
        }
        char_size = 0;
        decimal = 0;
        json_root[object_count[stack_level]].type = json_type_number;
        while(json[point]!=','){
          if((json[point]>='0'&&json[point]<='9')||json[point]=='.'){
            //es válido
            if(decimal==2){
              //ya ha habido espacio
              return -1;
            }
            if(json[point]=='.'){
              if(decimal != 0){
                //punto dos veces
                return -1;
              }
              else{
                decimal = 1;
              }
            }
            back_char = temp_char;
            temp_char = malloc((char_size+1)*sizeof(char));
            for(i=0;i<char_size;i++){
              temp_char[i] = back_char[i];
            }
            free(back_char);
            temp_char[char_size]=json[point];
            //TODO
            point++;
          }else{
            //solo whitespace
            switch (json[point]) {
              case ' ':
                if(decimal==2){
                  //ignorar
                }else{
                  //guardar
                  decimal = 2;
                  temp_num = malloc(sizeof(double));
                  json_root[object_count[stack_level]].content = temp_num;
                  json_root[object_count[stack_level]].size = 1;
                }
                break;
              case '\n':
                if(decimal==2){
                  //ignorar
                }else{
                  //guardar
                  decimal = 2;
                  temp_num = malloc(sizeof(double));
                  json_root[object_count[stack_level]].content = temp_num;
                  json_root[object_count[stack_level]].size = 1;
                }
                break;
              case '\r':
                if(decimal==2){
                  //ignorar
                }else{
                  //guardar
                  decimal = 2;
                  temp_num = malloc(sizeof(double));
                  json_root[object_count[stack_level]].content = temp_num;
                  json_root[object_count[stack_level]].size = 1;
                }
                break;
              case '\t':
                if(decimal==2){
                  //ignorar
                }else{
                  //guardar
                  decimal = 2;
                  temp_num = malloc(sizeof(double));
                  json_root[object_count[stack_level]].content = temp_num;
                  json_root[object_count[stack_level]].size = 1;
                }
                break;
              default:
                // error de ejecución
                return -1;
                break;
            }
          }
        }
        //ya se ha guradodo el número esperamos siguiente nombre
        state = s_namestart;
        break;
      }
      break;
    case s_array:
      // se espera whitespace o valores
      switch (json[point]) {
        case ' ':
          break;
        case '\n':
          break;
        case '\r':
          break;
        case '\t':
          break;
        case '"':
          //string
          //guardar la string y detectar si es object o string
          //guardar el último objeto agrupador
          temp_object = get_last_object(json_root,object_count,stack_level);
        point++;
        char_size = 0;
        while (json[point] != '"') {
          // cuidado con los \ es un string de JSON
          if (json[point] == '\\') {
            point++;
            switch (json[point]) {
            case '"':
              x_char = '"';
              break;
            case '/':
              x_char = '/';
              break;
            case '\\':
              x_char = '\\';
              break;
            case 'b':
              x_char = '\b';
              break;
            case 'f':
              x_char = '\f';
              break;
            case 'n':
              x_char = '\n';
              break;
            case 'r':
              x_char = '\r';
              break;
            case 't':
              x_char = '\t';
              break;
            case 'u':
              hex_char[0] = json[point + 1];
              hex_char[1] = json[point + 2];
              hex_char[2] = json[point + 3];
              hex_char[3] = json[point + 4];
              point += 4;
              x_char = strtol(hex_char, 0, 16);
              break;
            default:
              //no es un comando
              return -1;
            }
            back_char = temp_char;
            temp_char = malloc((char_size + 1) * sizeof(char));
            for (i = 0; i < char_size; i++) {
              temp_char[i] = back_char[i];
            }
            free(back_char);
            temp_char[char_size] = x_char;
            char_size++;
          }else{
            //cualquier otro caracter
                        back_char = temp_char;
            temp_char = malloc((char_size + 1) * sizeof(char));
            for (i = 0; i < char_size; i++) {
              temp_char[i] = back_char[i];
            }
            free(back_char);
            temp_char[char_size] = json[point];
            char_size++;
          }
          point++;
        }
        //avanzar hasta detectar si , o :
        point++;
        while(json[point]!=','&&json[point]==':'){
          //solo wite space o lo esperado
          switch(json[point]){
            case ' ':
              break;
            case '\n':
              break;
            case '\r':
              break;
            case '\t':
              break;
            case ':':
              // es objeto hay que detectar el tipo
              break;
            case ',':
              //es parte del array guardar valor
              break;
            default:
              //error
              return -1;
              break;
          }
          point++;
        }
        break;
      }
      break;
    case s_object_name:
      break;
    case s_object_nameend:
      break;
    case s_object_type:
      break;
    }
  }
  object->content = json_root;
  return 0;
}

struct object *get_last_object(struct object *root, unsigned int *object_count, unsigned short stack_level){
  int i;
  struct object *act_object=root;
  for(i=0;i<stack_level;i++){
    act_object = act_object[object_count[i]].content;
  }
  return act_object;
}