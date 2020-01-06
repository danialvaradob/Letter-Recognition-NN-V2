/*

Code by Daniel Alvarado Bonilla
Based in: https://towardsdatascience.com/simple-neural-network-implementation-in-c-663f51447547

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define FILAS 28
#define COLUMNAS  28
#define num_input_nodes (FILAS * COLUMNAS)
#define num_output_nodes 7 // A, B, C, D, E, F, _
#define num_hidden_nodes 32
#define learning_rate 0.5
#define EPOCHS 1000




typedef struct
{	
    double value;
    double output_weights[num_hidden_nodes];
    double bias;

}Output_Neuron;

typedef struct
{	
    double value;
    double input_weights[num_input_nodes];
    double bias;
}Neuron;

typedef struct {
    double value;
}Input_Neuron;

typedef struct Image{
	char type;
    double input[num_input_nodes];
    double expected_output[num_output_nodes];
    struct Image * next; 
}Image;

typedef struct Images_Array{
    struct Image * first_img;
}Images_Array;

void add_to_imgs_array(Images_Array * array, Image * img){ 
    if(array->first_img == NULL){
        array->first_img = img;
    }else{
        Image * current_image = array->first_img;
        int cont = 2;
        while(!(current_image->next == NULL)){
            current_image = current_image->next;
            cont++;
        }   
        current_image->next = img;
    }
}

int array_size(Image * img, int num){
    if(img->next == NULL){
        return num;
    }
    else{
        return array_size(img->next, num+1);
    }
}

double sigmoid(double x) { return 1 / (1 + exp(-x)); }
double dSigmoid(double x) { return sigmoid(x) * (1 - sigmoid(x)); }
double generate_random_weight() { return ((double)rand())/((double)RAND_MAX); }


void get_output(char type, double * result){
    char types [num_output_nodes]= {'A', 'B', 'C', 'D', 'E', 'F', '_'};
    for(int i = 0; i < num_output_nodes; i++){
        if(types[i] == type){
            result[i] = 1.0f;
        }else{
            result[i] = 0.0f;
        }
    }
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
        file = fopen(f, "r"); //readmode
        if (file == NULL) {
          perror("Error al abrir el file");
          exit(EXIT_FAILURE);
        }

        int eof = 0;
        int is_empty = 1;
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
                new_img->input[j] = atof(ch);
                new_img->type = files[i][0];
                get_output(files[i][0], new_img->expected_output);
                new_img->next = NULL;
                is_empty = 0;
            }
            if(!is_empty){
                add_to_imgs_array(img_array, new_img);
            }
        }
        fclose(file);   
    }
}

void shuffle_array(Images_Array* array){
    int size = array_size(array->first_img, 1);
    for(int i = 0; i < size; i++){
        int j = i + rand() / (RAND_MAX/(size-i)+1);
        if(i != j){
            Image * current_image1 = NULL; 
            Image * ant1 = NULL;
            Image * current_image2 = NULL;
            Image * ant2 = NULL;
            Image * current_image = array->first_img;
            Image * ant = NULL;
            int cont = 0;
            while(current_image1 == NULL || current_image2 == NULL){
                if(cont == i){
                    current_image1 = current_image;
                    ant1 = ant;
                }
                else if(cont == j){
                    current_image2 = current_image;
                    ant2 = ant;
                }
                cont ++;
                ant = current_image;
                current_image = current_image->next;
            }
            //aqui current_image1 y current_image tienen que cambiar de posicion
            if(i == 0){
                array->first_img = current_image2;
                ant2->next = current_image1;             
            }
            else if(j == 0){
                array->first_img = current_image1;
                ant1->next = current_image2;
            }else{
                ant1->next = current_image2;
                ant2->next = current_image1;
            }
            current_image = current_image1->next;
            current_image1->next = current_image2->next;              
            current_image2->next = current_image;
        }
    }

}

void print_img_array(Images_Array * array){
    Image * current_image = array -> first_img;
    while(current_image!= NULL){
        printf("%c ", current_image->type);
        current_image = current_image->next;
    }
    printf("\n");
}

void load_data(Input_Neuron * input_layer){
    FILE *file;
    file = fopen("Data/correr.txt", "r"); // read mode

    if (file == NULL) {
      perror("Error al abrir el file de letras");
      exit(EXIT_FAILURE);
    }

    char ch[3];
    for(int i=0; i<num_input_nodes; i++){
        ch[0] = fgetc(file);    
        ch[1] = fgetc(file);    
        ch[2] = fgetc(file);    
        fgetc(file);            
        input_layer[i].value = atof(ch);
    }

    fclose(file);
}

int load_weights(Neuron* hidden_layer, Output_Neuron* output_layer){
    FILE *file;
    file = fopen("Data/pesos.txt", "r"); // read mode

    if (file == NULL) {
      return 0;
    }

    char * line = NULL;
    size_t len = 0;
    size_t read;

    for(int i=0; i<num_hidden_nodes; i++){
        if((read = getline(&line, &len, file)) != -1) {
            char * ptr = strtok(line, " ");
            int cont = 0;
            while (ptr != NULL && cont < num_input_nodes) {
                hidden_layer[i].input_weights[cont] = atof(ptr);
                ptr = strtok (NULL, " ");
                cont++;
            }
        }else{
            fclose(file);
            return 0;
        }
    }

    for(int i=0; i<num_output_nodes; i++){
        if((read = getline(&line, &len, file)) != -1) {
            char * ptr = strtok(line, " ");
            int cont = 0;
            while (ptr != NULL && cont < num_hidden_nodes) {
                output_layer[i].output_weights[cont] = atof(ptr);
                ptr = strtok (NULL, " ");
                cont++;
            }
        }else{
            fclose(file);
            return 0;
        }
    }

    fclose(file);
    if (line)
        free(line);
    return 1;
}

int load_bias(Neuron* hidden_layer, Output_Neuron* output_layer){
    FILE *file;
    file = fopen("Data/biases.txt", "r"); // read mode

    if (file == NULL) {
      return 0;
    }

    char * line = NULL;
    size_t len = 0;
    size_t read;

    if((read = getline(&line, &len, file)) != -1) {
        char * ptr = strtok(line, " ");
        int cont = 0;
        while (ptr != NULL && cont < num_hidden_nodes) {
            hidden_layer[cont].bias = atof(ptr);
            ptr = strtok (NULL, " ");
            cont++;
        }
    }else{
        fclose(file);
        return 0;
    }

    if((read = getline(&line, &len, file)) != -1) {
        char * ptr = strtok(line, " ");
        int cont = 0;
        while (ptr != NULL && cont < num_output_nodes) {
            output_layer[cont].bias = atof(ptr);
            ptr = strtok (NULL, " ");
            cont++;
        }
    }else{
        fclose(file);
        if(line)
            free(line);
        return 0;
    }

    fclose(file);
    if(line)
        free(line);
    
    return 1;
}

void save_weights(Neuron* hidden_layer, Output_Neuron* output_layer){
    FILE *file;
    file = fopen("Data/pesos.txt", "w");

    for(int i=0; i<num_hidden_nodes; i++){
        for(int j=0; j<num_input_nodes; j++){
            fprintf(file, "%lf ", hidden_layer[i].input_weights[j]);
        }
        fprintf(file, "\n"); 
    }
    for(int i=0; i<num_output_nodes; i++){
        for(int j=0; j<num_hidden_nodes; j++){
            fprintf(file, "%lf ", output_layer[i].output_weights[j]);
        }
        fprintf(file, "\n"); 
    }

    fclose(file);
}

void save_bias(Neuron* hidden_layer, Output_Neuron* output_layer){
    FILE *file;
    file = fopen("Data/biases.txt", "w");

    for(int i=0; i<num_hidden_nodes; i++){
        fprintf(file, "%lf ", hidden_layer[i].bias);
    }
    fprintf(file, "\n"); 
    for(int i=0; i<num_output_nodes; i++){
        fprintf(file, "%lf ", output_layer[i].bias);
    }

    fclose(file);
}

void test_aux(Input_Neuron* input_layer, Neuron* hidden_layer, Output_Neuron* output_layer){
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

char get_type(Output_Neuron* output_layer){
    char types [] = {'A', 'B', 'C', 'D', 'E', 'F', ' '};
    double highest = 0;
    int highest_pos = 0;
    for(int i = 0; i < num_output_nodes; i++){
        if(output_layer[i].value > highest){
            highest = output_layer[i].value;
            highest_pos = i;
        }
    }
    return types[highest_pos];
}

void test(Images_Array * img_array, Input_Neuron* input_layer, Neuron* hidden_layer, Output_Neuron* output_layer){
    int correct = 0;
    int incorrect = 0;
    int total = 0;
    Image * current_image = img_array->first_img;
    while(current_image != NULL){
        for(int i=0; i<num_input_nodes; i++){
            input_layer[i].value = current_image->input[i];
        }
        test_aux(input_layer, hidden_layer, output_layer);
        char r = get_type(output_layer);
        if(current_image->type == r){
            correct++;
        }else{
            incorrect++;
        }
        current_image = current_image->next;
        total++;
    }
    printf("Correctos: %d\n",correct);
    printf("Incorrectos: %d\n", incorrect);
    printf("Total: %d\n", total);
}


void train_network_aux(Image * img, Neuron* hidden_layer, Output_Neuron* output_layer){
    
    // FORWARD

    for (int j=0; j<num_hidden_nodes; j++) {
        double activation = hidden_layer[j].bias;
        for (int k=0; k<num_input_nodes; k++) {
            activation += (img->input[k] * hidden_layer[j].input_weights[k]);
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

    // BACK

    double deltaoutput[num_output_nodes];
    for (int j=0; j<num_output_nodes; j++) {
        double erroroutput = img->expected_output[j] - output_layer[j].value;
        deltaoutput[j] = erroroutput * dSigmoid(output_layer[j].value);
    }
    
    double deltaOculto[num_hidden_nodes];
    for (int j=0; j<num_hidden_nodes; j++) {
        double errorOculto = 0.0f;
        for(int k=0; k<num_output_nodes; k++) {
            errorOculto += deltaoutput[k] * output_layer[k].output_weights[j];
        }
        deltaOculto[j] = errorOculto*dSigmoid(hidden_layer[j].value);
    }
    
    for (int j=0; j<num_output_nodes; j++) {
        output_layer[j].bias += deltaoutput[j] * learning_rate;
        for (int k=0; k<num_hidden_nodes; k++) {
            output_layer[j].output_weights[k] += hidden_layer[k].value * deltaoutput[j] * learning_rate;
        }
    }
    
    for (int j=0; j<num_hidden_nodes; j++) {
        hidden_layer[j].bias += deltaOculto[j] * learning_rate;
        for(int k=0; k<num_input_nodes; k++) {
            hidden_layer[j].input_weights[k] += img->input[k] * deltaOculto[j] * learning_rate;
        }
    }
}

void train_network(Images_Array * img_array, Neuron* hidden_layer, Output_Neuron* output_layer){
    Image * current_image = img_array -> first_img;
    for (int n=0; n < EPOCHS; n++) {
        current_image = img_array -> first_img;
        while(current_image != NULL){
            train_network_aux(current_image, hidden_layer, output_layer);
            current_image = current_image->next;
        }
    }
}


int main(int argc, const char * argv[]){
    
    Input_Neuron* input_layer = malloc(sizeof(Input_Neuron)*num_input_nodes);
    Neuron* hidden_layer = malloc(sizeof(Neuron) *num_hidden_nodes);
    Output_Neuron* output_layer = malloc(sizeof(Output_Neuron)*num_output_nodes);

    Images_Array * img_array = malloc(sizeof(Images_Array));


    load_data_training(img_array);
    shuffle_array(img_array);
    printf("size: %d\n", array_size(img_array->first_img, 1));
    print_img_array(img_array);

    int weights_loaded = load_weights(hidden_layer, output_layer);
    int bias_loaded = load_bias(hidden_layer, output_layer);

    if(!weights_loaded || !bias_loaded){
        printf("\nERROR: No se cargaron los files de pesos y bias.\n");

        for(int i=0; i<num_hidden_nodes; i++) {
            hidden_layer[i].bias = generate_random_weight();
            for(int j=0; j<num_input_nodes; j++){
                hidden_layer[i].input_weights[j] = generate_random_weight();
            }
        }

        for (int i=0; i<num_output_nodes; i++) {
            output_layer[i].bias = generate_random_weight();
            for (int j=0; j<num_hidden_nodes; j++) {
                output_layer[i].output_weights[j] = generate_random_weight();
            }
        }
    }


    int train_network_bool = 1;
    if(train_network_bool){
        printf("\n===== ENTRENANDO =====\n");

        printf("\nNodos Ocultos: %d, LR: %f, EPOCHS:%d\n",
            num_hidden_nodes, learning_rate, EPOCHS);

        train_network(img_array, hidden_layer, output_layer);
        for(int i=0; i<num_output_nodes; i++){
            printf("output:%lf\n", output_layer[i].value);
        }
        save_weights(hidden_layer, output_layer);
        save_bias(hidden_layer, output_layer);
        test(img_array, input_layer, hidden_layer, output_layer);

    }else{
        printf("\n===== PROBANDO =====\n");

        printf("Ejecutando...\n");
        system("python3 execute_nn.py"); 
        load_data(input_layer);

        test_aux(input_layer, hidden_layer, output_layer);
        for(int i=0; i<num_output_nodes; i++){
            printf("output:%lf\n", output_layer[i].value);
        }
        char r = get_type(output_layer);
        printf("La letra es una: %c\n", r);

        test(img_array, input_layer, hidden_layer, output_layer);

        
        /*test_aux(input_layer, hidden_layer, output_layer);
        for(int i=0; i<num_output_nodes; i++){
            printf("output:%lf, Esperado:%lf\n", output_layer[i].value);
        }*/
    }


    printf("Termina.\n");
    
    return 0;
}


/*

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


    
    
    

    int train_network_bool = 1;
    if(train_network_bool){
        printf("\n===== ENTRENANDO =====\n");
        int first_time = 0;

        if (first_time) {
            printf("Creating new weights...\n");
            

            for(int i=0; i<num_hidden_nodes; i++) {
                hidden_layer[i].bias = generate_random_weight();
                for(int j=0; j<num_input_nodes; j++){
                    hidden_layer[i].input_weights[j] = generate_random_weight();
                }
            }

            for (int i=0; i<num_output_nodes; i++) {
                output_layer[i].bias = generate_random_weight();
                for (int j=0; j<num_hidden_nodes; j++) {
                    output_layer[i].output_weights[j] = generate_random_weight();
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
        test(img_array, input_layer, hidden_layer, output_layer);
        save_hidden_layer(hidden_layer);
        save_output_layer(output_layer);


    }else{
        printf("\n===== PROBANDO =====\n");
        system("python execute_nn.py"); 
        load_image_test(input_layer);
        test_aux(input_layer, hidden_layer, output_layer);
        printf("Network output:\n");
        print_output(output_layer);
        printf("Letter is a: %c\n", get_type(output_layer));
    }


    printf("============== DONE ==================\n");
    
    return 0;
}
*/