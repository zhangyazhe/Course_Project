#ifndef LANUAGE_FORMAT_PROCESSING_FORMAT_H
#define LANUAGE_FORMAT_PROCESSING_FORMAT_H

typedef struct token{
    char *str;
    struct token *next;
}token;

void format();
token* readline();

#endif //LANUAGE_FORMAT_PROCESSING_FORMAT_H
