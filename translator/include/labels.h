#ifndef LABELS_H_
#define LABELS_H_

#include "translator.h"

static const char LABEL_MARK = ':';

struct Label {
    const char* name;
    int adresses;
};

#endif // LABELS_VEC_H_