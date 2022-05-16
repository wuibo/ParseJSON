#define json_type_string 1
#define json_type_number 2
#define json_type_object 3
#define json_type_array 4
#define json_type_boolean 5
#define json_type_null 6


struct object {
  unsigned short type;
  unsigned int size;
  char * name;
  void * content;
};

unsigned short read_json(struct object *object, char * json,unsigned int size);
struct object *get_last_object(struct object *root, unsigned int *object_count, unsigned short stack_level)