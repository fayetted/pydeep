#include <Python.h>
#include <fuzzy.h>

static PyObject *pydeepError;

static PyObject * pydeep_hash_file(PyObject *self, PyObject *args){
    FILE *inputFile;
    PyObject *ssdeepHash = NULL;
    char *hashResult = NULL;
    char *filename;
    int ret;
    if (!PyArg_ParseTuple(args, "s", &filename)){
        return NULL;
    }

    inputFile = fopen(filename, "rb");

    // Allocate return buffers for hash
    hashResult = (char *)malloc(FUZZY_MAX_RESULT);
    if (hashResult == NULL){
        PyErr_SetString(pydeepError, "Error allocating malloc buffer");
        return NULL;
    }
    ret = fuzzy_hash_file( inputFile, hashResult);
    if (ret != 0){
        free(hashResult);
        fclose(inputFile);
        PyErr_SetString(pydeepError, "Error in fuzzy_hash!");
        return NULL;
    }
    ssdeepHash = PyString_FromString( hashResult );
    free(hashResult);
    fclose(inputFile);
    return ssdeepHash;
}

static PyObject * pydeep_hash_buf(PyObject *self, PyObject *args){
    PyObject *inputStringBuffer = NULL;
    PyObject *ssdeepHash= NULL;
    Py_ssize_t stringSize = 0;
    char *inputBuffer=NULL;
    char *hashResult;
    int ret;

    if (!PyArg_ParseTuple(args, "s#", &inputBuffer, &stringSize)){
        return NULL;
    }

    // Allocate return buffers for hash
    hashResult = (char *)malloc(FUZZY_MAX_RESULT);
    if (hashResult == NULL){
        PyErr_SetString(pydeepError, "Error allocating malloc buffer");
        return NULL;
    }

    inputStringBuffer = PyString_FromStringAndSize( inputBuffer, stringSize);

    ret = fuzzy_hash_buf((unsigned char*)inputBuffer, (uint32_t)stringSize, hashResult);
    if (ret !=0 ){
        free(hashResult);
        PyErr_SetString(pydeepError, "Error in fuzzy_hash!");
        return NULL;
    }
    ssdeepHash = PyString_FromString( hashResult );
    free(hashResult);
    return ssdeepHash;
}

static PyObject * pydeep_compare(PyObject *self, PyObject *args){
    char *ssdeepHash1= NULL;
    char *ssdeepHash2= NULL;
    int ret;
    if (!PyArg_ParseTuple(args, "ss", &ssdeepHash1, &ssdeepHash2)){
        return NULL;
    }
    ret = fuzzy_compare(ssdeepHash1, ssdeepHash2);
    if (ret < 0){
        PyErr_SetString(pydeepError, "Error in fuzzy compare");
        return NULL;
    }
    return Py_BuildValue("i", ret);
}

// Method definitions
static PyMethodDef pydeepMethods[] = {
    {"hash_file", pydeep_hash_file, METH_VARARGS, "compute the ssdeep fuzzy hash for a file"},
    {"hash_buf", pydeep_hash_buf, METH_VARARGS, "compute the ssdeep fuzzy hash for a buffer"},
    {"hash_bytes", pydeep_hash_buf, METH_VARARGS, "compute the ssdeep fuzzy hash for a buffer"},
    {"compare", pydeep_compare, METH_VARARGS, "compute two ssdeep hashes"},
    {NULL,  NULL}
};

// Initialization
void initpydeep(void)
{
    PyObject *m;
    m = Py_InitModule("pydeep", pydeepMethods);
    pydeepError = PyErr_NewException("pydeep.Error", NULL, NULL);
    Py_INCREF(pydeepError);
    PyModule_AddObject(m, "error", pydeepError);
}
