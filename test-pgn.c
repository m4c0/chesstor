#include <stdio.h>
#include <string.h>

int main() {
  FILE * f = fopen("test.pgn", "rb");
  
  char line[1024];
  while (fgets(line, sizeof(line), f)) {
    if (strcmp(line, "[Result \"0-1\"]\n")) continue;

    puts("found it");
  }
}
