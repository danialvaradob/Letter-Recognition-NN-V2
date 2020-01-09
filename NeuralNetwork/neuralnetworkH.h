#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define num_input_nodes 784
#define num_hidden_nodes  32
#define num_output_nodes  7
#define learning_rate  0.5
#define EPOCHS  1500

// numbers of files per folder
static int imgs_A_num = 217;
static int imgs_B_num = 227;
static int imgs_C_num = 213;
static int imgs_D_num = 235;
static int imgs_E_num = 333;
static int imgs_F_num = 283;
static int imgs_num = 10;


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
    double pixels[num_input_nodes];
    char letter;
    double expected_output[num_output_nodes];
    struct Image * next; 
} Image;

typedef struct Images_Array{
    struct Image * first_img;
} Images_Array;

void add_to_imgs_array(Images_Array * array, Image * img);
void set_img_output(char letter, double * result);
void print_imgs(Images_Array* img_array);
void shuffleList(Image** head);
int get_size(Image *first_img);
Images_Array* get_images(char* path, int number_of_images, Images_Array* images, char letter);
void load_data_training(Images_Array* img_array);
void print_imgs(Images_Array* img_array);
void add_to_imgs_array(Images_Array * array, Image * img);
void set_img_output(char letter, double * result);
void load_image_test(Input_Neuron * input_layer);
void test_letter(Input_Neuron* input_layer, Neuron* hidden_layer, Output_Neuron* output_layer);
char get_letter(Output_Neuron* output_layer);
void test_network(Images_Array * img_array, Input_Neuron* input_layer, Neuron* hidden_layer, Output_Neuron* output_layer);
Neuron* load_hidden_layer(Neuron* hidden_layer);
int save_hidden_layer(Neuron* hidden_layer);
Output_Neuron* load_output_layer(Output_Neuron* output_layer);
int save_output_layer(Output_Neuron* output_layer);
void print_output(Output_Neuron* output_layer);
void train_network_aux(Image * img, Neuron* _hidden_layer, Output_Neuron* _output_layer);
void train_network(Images_Array * img_array, Neuron* hidden_layer, Output_Neuron* output_layer);
