#ifndef LABELS_VEC_H_
#define LABELS_VEC_H_

#include "int_vector.h"
#include "my_vector.h"

struct LabelsVec {
    MyVector names;
    IntVector adresses;
    size_t size;
};

void LabelsVecInit(LabelsVec* labels_vec);

void LabelsVecAdd(LabelsVec* labels_vec, Line name, int adress);

void LabelsVecFree(LabelsVec* labels_vec);

#endif // LABELS_VEC_H_