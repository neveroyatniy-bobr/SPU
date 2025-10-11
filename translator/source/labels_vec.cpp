#include "labels_vec.h"

#include "int_vector.h"
#include "my_vector.h"

void LabelsVecInit(LabelsVec* labels_vec) {
    IntVectorInit(&labels_vec->adresses, 0);
    MyVectorInit(&labels_vec->names, 0);
    labels_vec->size = 0;
}

void LabelsVecAdd(LabelsVec* labels_vec, Line name, int adress) {
    MyVectorAdd(&labels_vec->names, name);
    IntVectorAdd(&labels_vec->adresses, adress);
    labels_vec->size++;
}

void LabelsVecFree(LabelsVec* labels_vec) {
    IntVectorFree(&labels_vec->adresses);
    MyVectorFree(&labels_vec->names);
}