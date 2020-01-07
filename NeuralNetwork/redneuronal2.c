/*

Code by Daniel Alvarado Bonilla
Based in: https://towardsdatascience.com/simple-neural-network-implementation-in-c-663f51447547
          
Structures based from:
        https://github.com/AndrewCarterUK/mnist-neural-network-plain-c
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "neuralnetwork.h"



double sigmoid(double x) {
    return 1 / (1 + exp(-x)); 
    }

double dSigmoid(double x) { 
    return sigmoid(x) * (1 - sigmoid(x));
    }

double init_weight() { 
    return ((double)rand())/((double)RAND_MAX); 
    }



void load_data_training(Images_Array* img_array){

    FILE *file;
    char* files [] = {"A", "B", "C", "D", "E", "F", "_"};
    for(int i = 0; i < num_output_nodes; i++){
        char f [11];
        strcpy(f, "Data/");
        strcat(f, files[i]);
        strcat(f, ".txt");
        printf("%s\n",f);
        file = fopen(f, "r"); 
        if (file == NULL) {
          perror("Error al abrir el file");
          exit(EXIT_FAILURE);
        }

        int eof = 0;
        int is_empty = 1;
        int img_counter = 0;
        while(!eof){
            char ch[3];
            Image * new_img = malloc(sizeof(Image));
            is_empty = 1;
            for(int j=0; j<num_input_nodes; j++){
                ch[0] = fgetc(file);    
                if(ch[0] == '\n' || ch[0] == '\r') break;  
                if(ch[0] == EOF){
                    eof = 1;
                    break;
                }
                ch[1] = fgetc(file);    
                ch[2] = fgetc(file);    
                fgetc(file);            
                new_img->pixels[j] = atof(ch);
                new_img->letter = files[i][0];
                set_img_output(files[i][0], new_img->expected_output);
                new_img->next = NULL;
                is_empty = 0;
            }
            if(!is_empty){
                add_to_imgs_array(img_array, new_img);
                img_counter++;
            }
        }
        fclose(file);
        printf("%d\n", img_counter); 
    }
}


void add_to_imgs_array(Images_Array * array, Image * img){ 
    if(array->first_img == NULL){
        array->first_img = img;
    }else{
        Image * curr_img = array->first_img;
        int cont = 2;
        while(!(curr_img->next == NULL)){
            curr_img = curr_img->next;
            cont++;
        }   
        curr_img->next = img;
    }
}

int get_size(Image *first_img) {
    int size = 0;
    Image* curr_img = first_img;
    while (curr_img->next != NULL) {
        size++;
        curr_img = curr_img->next;
    }
    return size;

}


void shuffle_array(Images_Array* array){
    int size = get_size(array->first_img);
    for(int i = 0; i < size; i++){
        int j = i + rand() / (RAND_MAX/(size-i)+1);
        if(i != j){
            Image * curr_img1 = NULL; 
            Image * ant1 = NULL;
            Image * curr_img2 = NULL;
            Image * ant2 = NULL;
            Image * curr_img = array->first_img;
            Image * ant = NULL;
            int cont = 0;
            while(curr_img1 == NULL || curr_img2 == NULL){
                if(cont == i){
                    curr_img1 = curr_img;
                    ant1 = ant;
                }
                else if(cont == j){
                    curr_img2 = curr_img;
                    ant2 = ant;
                }
                cont ++;
                ant = curr_img;
                curr_img = curr_img->next;
            }
            if(i == 0){
                array->first_img = curr_img2;
                ant2->next = curr_img1;             
            }
            else if(j == 0){
                array->first_img = curr_img1;
                ant1->next = curr_img2;
            }else{
                ant1->next = curr_img2;
                ant2->next = curr_img1;
            }
            curr_img = curr_img1->next;
            curr_img1->next = curr_img2->next;              
            curr_img2->next = curr_img;
        }
    }

}



void set_img_output(char letter, double * result){
    char letters [num_output_nodes]= {'A', 'B', 'C', 'D', 'E', 'F', '_'};

    for(int i = 0; i < num_output_nodes; i++){

        if(letters[i] == letter){
            result[i] = 1.0f;
        }else{
            result[i] = 0.0f;
        }
    }
}



void load_image_test(Input_Neuron * input_layer){
    FILE *file;
    file = fopen("Data/imagen_prueba.txt", "r"); 

    if (file == NULL) {
      perror("Error opening ");
      exit(EXIT_FAILURE);
    }
    char ch[3];

    for(int i = 0; i < num_input_nodes; i++){
        ch[0] = fgetc(file);    
        ch[1] = fgetc(file);    
        ch[2] = fgetc(file);    
        fgetc(file);            
        input_layer[i].value = atof(ch);
    }

    fclose(file);
}



void test_letter(Input_Neuron* input_layer, Neuron* hidden_layer, Output_Neuron* output_layer){
    for (int j=0; j<num_hidden_nodes; j++) {
        double activation = hidden_layer[j].bias;
        for (int k=0; k<num_input_nodes; k++) {
            activation += input_layer[k].value * hidden_layer[j].input_weights[k];
        }
        hidden_layer[j].value = sigmoid(activation);
    }
    
    for (int j=0; j<num_output_nodes; j++) {
        double activation = output_layer[j].bias;
        for (int k=0; k<num_hidden_nodes; k++) {
            activation += hidden_layer[k].value * output_layer[j].output_weights[k];
        }
        output_layer[j].value = sigmoid(activation);
    }
}

char get_letter(Output_Neuron* output_layer){
    char letters [] = {'A', 'B', 'C', 'D', 'E', 'F', ' '};
    double highest = 0;
    int highest_pos = 0;
    for(int i = 0; i < num_output_nodes; i++){
        if(output_layer[i].value > highest){
            highest = output_layer[i].value;
            highest_pos = i;
        }
    }
    return letters[highest_pos];
}

void test_network(Images_Array * img_array, Input_Neuron* input_layer, Neuron* hidden_layer, Output_Neuron* output_layer){
    int correct = 0;
    int incorrect = 0;
    int total = 0;
    Image * curr_img = img_array->first_img;
    while(curr_img != NULL){
        for(int i=0; i<num_input_nodes; i++){
            input_layer[i].value = curr_img->pixels[i];
        }
        test_letter(input_layer, hidden_layer, output_layer);
        
        if(curr_img->letter == get_letter(output_layer)){
            correct++;
        }else{
            incorrect++;
        }
        curr_img = curr_img->next;
        total++;
    }
    printf("Correct: %d\n",correct);
    printf("Incorrect: %d\n", incorrect);
    printf("Total: %d\n", total);
    printf("Accuracy: %f\n",(double) correct / (double) total);
    }


/**
 * Method used to load the hidden layer from a simple .dat file
 */ 
Neuron* load_hidden_layer(Neuron* hidden_layer) {
    
    FILE *file;
    file = fopen(path_hidden_layer, "r");
    int i, j;

    fread(&hidden_layer[0],sizeof(Neuron), num_hidden_nodes , file);

    fclose(file);

    return hidden_layer;
}

/* Method used to save hidden layer into a simple dat file
 * with all the respectives weights
 * Not optimized
 * 
 * "Layers/hidden_layer.dat"
 */

int save_hidden_layer(Neuron* hidden_layer) {
    FILE *file;
    file = fopen(path_hidden_layer, "w");

    if (file != NULL) {
        for (int i = 0; i < num_hidden_nodes; i++) {
            fwrite(&hidden_layer[i] ,sizeof(Neuron), 1 ,file);
            
        }
    }    
    
    fclose(file);

    return 1;
}

/**
 * Method used to load the output layer from a simple .dat file
 */ 
Output_Neuron* load_output_layer(Output_Neuron* output_layer) {
    
    FILE *file;
    file = fopen(path_output_layer, "r");
    int i, j;

    fread(&output_layer[0],sizeof(Output_Neuron), num_output_nodes , file);

    fclose(file);

    return output_layer;
}

/* Method used to save output layer into a simple dat file
 * with all the respectives weights
 * Not optimized
 * 
 * "Layers/output_layer.dat"
 */

int save_output_layer(Output_Neuron* output_layer) {
    FILE *file;
    file = fopen(path_output_layer, "w");

    if (file != NULL) {
        for (int i = 0; i < num_output_nodes; i++) {
            fwrite(&output_layer[i] ,sizeof(Output_Neuron), 1 ,file);
            
        }
    }    
    
    fclose(file);

    return 1;
}

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////


void print_output(Output_Neuron* output_layer) {
    char letters [] = {'A', 'B', 'C', 'D', 'E', 'F', ' '};
    for(int i = 0; i < num_output_nodes; i++){
        switch (i)
        {
        case 0:
            printf("A: %lf\n", output_layer[i].value);
            break;
        case 1:
            printf("B: %lf\n", output_layer[i].value);
            break;
        case 2:
            printf("C: %lf\n", output_layer[i].value);
            break;
        case 3:
            printf("D: %lf\n", output_layer[i].value);
            break;
        case 4:
            printf("E: %lf\n", output_layer[i].value);
            break;
        case 5:
            printf("F: %lf\n", output_layer[i].value);
            break;
        case 6:
            printf("_: %lf\n", output_layer[i].value);
            break;
        
        
        
        default:
            break;
        }
    }
}


void train_network_aux(Image * img, Neuron* _hidden_layer, Output_Neuron* _output_layer){    
    // FORWARD
    for (int j=0; j<num_hidden_nodes; j++) {
        double activation = _hidden_layer[j].bias;
        for (int k=0; k<num_input_nodes; k++) {
            activation += (img->pixels[k] * _hidden_layer[j].input_weights[k]);
        }
        _hidden_layer[j].value = sigmoid(activation);
    }
    
    for (int j=0; j<num_output_nodes; j++) {
        double activation = _output_layer[j].bias;
        for (int k=0; k<num_hidden_nodes; k++) {
            activation += _hidden_layer[k].value * _output_layer[j].output_weights[k];
        }
        _output_layer[j].value = sigmoid(activation);
    }

    // BACK

    double delta_output[num_output_nodes];
    for (int j=0; j<num_output_nodes; j++) {
        double error_output = img->expected_output[j] - _output_layer[j].value;
        delta_output[j] = error_output * dSigmoid(_output_layer[j].value);
    }
    
    double delta_hidden[num_hidden_nodes];
    for (int j=0; j<num_hidden_nodes; j++) {
        double error_hidden = 0.0f;
        for(int k=0; k<num_output_nodes; k++) {
            error_hidden += delta_output[k] * _output_layer[k].output_weights[j];
        }
        delta_hidden[j] = error_hidden * dSigmoid(_hidden_layer[j].value);
    }
    
    for (int j=0; j<num_output_nodes; j++) {
        _output_layer[j].bias += delta_output[j] * learning_rate;
        for (int k=0; k<num_hidden_nodes; k++) {
            _output_layer[j].output_weights[k] += _hidden_layer[k].value * delta_output[j] * learning_rate;
        }
    }
    
    for (int j=0; j<num_hidden_nodes; j++) {
        _hidden_layer[j].bias += delta_hidden[j] * learning_rate;
        for(int k=0; k<num_input_nodes; k++) {
            _hidden_layer[j].input_weights[k] += img->pixels[k] * delta_hidden[j] * learning_rate;
        }
    }
}

void train_network(Images_Array * img_array, Neuron* hidden_layer, Output_Neuron* output_layer){
    Image * curr_img = NULL;
    for (int epoch = 0; epoch < EPOCHS; epoch++) {
        curr_img = img_array -> first_img;
        while(curr_img != NULL){
            train_network_aux(curr_img, hidden_layer, output_layer);
            curr_img = curr_img->next;
        }
    }
}

int main(int argc, const char * argv[]){
    printf("========== RECONOCER LETRAS ==========\n");
    printf("\n\n");
    Images_Array * img_array = malloc(sizeof(Images_Array));
    load_data_training(img_array);
    shuffle_array(img_array);
    

    // setting layers
    Input_Neuron* input_layer = malloc(sizeof(Input_Neuron)*num_input_nodes);
    Neuron* hidden_layer = malloc(sizeof(Neuron) *num_hidden_nodes);
    Output_Neuron* output_layer = malloc(sizeof(Output_Neuron)*num_output_nodes);

    int train_network_bool = 0;
    if(train_network_bool){
        printf("\n===== ENTRENANDO =====\n");
        int first_time = 0;

        if (first_time) {
            printf("Creating new weights...\n");
            

            for(int i=0; i<num_hidden_nodes; i++) {
                hidden_layer[i].bias = init_weight();
                for(int j=0; j<num_input_nodes; j++){
                    hidden_layer[i].input_weights[j] = init_weight();
                }
            }

            for (int i=0; i<num_output_nodes; i++) {
                output_layer[i].bias = init_weight();
                for (int j=0; j<num_hidden_nodes; j++) {
                    output_layer[i].output_weights[j] = init_weight();
                }
            }
            
        } else {
            hidden_layer = load_hidden_layer(hidden_layer);
            output_layer = load_output_layer(output_layer);
            printf("\nNetwork loaded\n");
        }

        //printf("\nNodos Ocultos: %d, LR: %f, EPOCHS:%d\n",
        //    num_hidden_nodes, learning_rate, EPOCHS);

        train_network(img_array, hidden_layer, output_layer);
        test_network(img_array, input_layer, hidden_layer, output_layer);
        save_hidden_layer(hidden_layer);
        save_output_layer(output_layer);


    }else{
        hidden_layer = load_hidden_layer(hidden_layer);
        output_layer = load_output_layer(output_layer);

        printf("\n===== PROBANDO =====\n");
        // executing python file to transform raw data to usable data for the network
        system("python execute_nn.py"); 

        // loading image to input layer, to run in network
        load_image_test(input_layer);

        // testing networks output
        test_letter(input_layer, hidden_layer, output_layer);
        printf("Network output:\n");
        print_output(output_layer);
        printf("Letter is a: %c\n", get_letter(output_layer));
    }


    printf("============== DONE ==================\n");
    
    return 0;
}

/*
int main(int argc, const char * argv[]){
    printf("========== RECONOCER LETRAS ==========\n");
    printf("\n\n");
    Images_Array * img_array = malloc(sizeof(Images_Array));
    load_data_training(img_array);
    shuffle_array(img_array);
    int first_time = 0;

    // setting layers
    Input_Neuron* input_layer = malloc(sizeof(Input_Neuron)*num_input_nodes);
    Neuron* hidden_layer = malloc(sizeof(Neuron) *num_hidden_nodes);
    Output_Neuron* output_layer = malloc(sizeof(Output_Neuron)*num_output_nodes);


    if (first_time) {
        printf("Creating new weights...\n");
        

        for(int i=0; i<num_hidden_nodes; i++) {
            hidden_layer[i].bias = init_weight();
            for(int j=0; j<num_input_nodes; j++){
                hidden_layer[i].input_weights[j] = init_weight();
            }
        }

        for (int i=0; i<num_output_nodes; i++) {
            output_layer[i].bias = init_weight();
            for (int j=0; j<num_hidden_nodes; j++) {
                output_layer[i].output_weights[j] = init_weight();
            }
        }
        
    } else {
        hidden_layer = load_hidden_layer(hidden_layer);
        output_layer = load_output_layer(output_layer);
        printf("\nNetwork loaded\n");
    }
    
    

    int train_network_bool = 0;
    if(train_network_bool){
        printf("\n===== ENTRENANDO =====\n");

        //printf("\nNodos Ocultos: %d, LR: %f, EPOCHS:%d\n",
        //    num_hidden_nodes, learning_rate, EPOCHS);

        train_network(img_array, hidden_layer, output_layer);
        test(img_array, input_layer, hidden_layer, output_layer);
        save_hidden_layer(hidden_layer);
        save_output_layer(output_layer);


    }else{
        printf("\n===== PROBANDO =====\n");
        system("python execute_nn.py"); 
        load_image(input_layer);
        test_letter(input_layer, hidden_layer, output_layer);
        printf("Network output:\n");
        print_output(output_layer);
        printf("Letter is a: %c\n", get_letter(output_layer));
    }


    printf("============== DONE ==================\n");
    
    return 0;
}
*/

