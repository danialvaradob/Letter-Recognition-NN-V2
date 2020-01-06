#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define num_input_nodes 784
#define num_output_nodes 7 
#define num_hidden_nodes 32
#define learning_rate 0.65
#define EPOCHS 1000

char path_hidden_layer[] = "Layers/hidden_layer.dat";
char path_output_layer[] = "Layers/output_layer.dat";

typedef struct
{
    double value;
    double output_weights[num_hidden_nodes];
    double bias;
    
} Output_Neuron;


typedef struct
{   
    double value;
    double input_weights[num_input_nodes];
    double bias;
} Neuron;


typedef struct {
    double value;
} Input_Neuron;

typedef struct {
    Input_Neuron* input_layer;
    Neuron* hidden_layer;
    Output_Neuron* output_layer;
} Network;


typedef struct Image{
    double input[num_input_nodes];
    double expected_output[num_output_nodes];
    char type;
    struct Image * next; 
} Image;

typedef struct Images_Array{
    struct Image * first_img;
} Images_Array;

void add_to_imgs_array(Images_Array * array, Image * img) {}
int array_size(Image * img, int num) {}
