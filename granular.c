//Granular sythesizer 

#include <stdint.h> // for int data types
#include <stdio.h> // for file ops
#include <stdlib.h> // for printf and rand
#include <string.h> // for strcmp with command-line args
#include <time.h> // for seeding random
long getFileSize(const char *filename);

// Number of bytes in .wav header
const int HEADER_SIZE = 44;

int main(int argc, char *argv[])
{
       srand(time(NULL)); //seed random with current time for later use, otherwise output will always be the same
   
    // Check command-line arguments
    if (argc < 4 || argc>6)
    {
        printf("Usage: ./volume input.wav output.wav factor\n"); // didn't bother correcting this but it's error handling
        return 1;
    }

    // Open files and determine scaling factor
    FILE *input = fopen(argv[1], "r");
    if (input == NULL)
    {
        printf("Could not open file.\n");
        return 1;
    }
    FILE *output = fopen(argv[2], "w");
    if (output == NULL)
    {
        printf("Could not open file.\n");
        return 1;
    }

    //Copy header from input file to output file
    uint8_t header[HEADER_SIZE];
    fread(header, sizeof(uint8_t), HEADER_SIZE, input);
    fwrite(header, sizeof(uint8_t), HEADER_SIZE, output);


//scramble arg says hey let's granulate
    if(strcmp(argv[3], "scramble") == 0){
        long input_size = getFileSize(argv[1]); //get size of entire input file MINUS the header (already been read).
        int16_t *buffer = malloc(input_size); //create a buffer the size of the entire input file minus header.
        if (buffer == NULL) {
            printf("Could not open file."); // malloc error handling (also wrong message)
        }

        //size_t is an unsigned int specifically for storing length of things. It is guaranteed to be able to store the maximum size of any object the host system can handle, which means it's typically used for lengths of files / memory allocation / things like that. Here it is being used to store the number of audio samples in the input file. It will vary from machine to machine so it will work well in some cases and cause problems in others. It's designed for stuff like this, but not every int should be stored here.
        //fread() is reading the entirety of the input file's audio data. It is reading input_size/2 amount of items (because input_size is the TOTAL amount of samples, but we are reading 2 bytes at a time for audio samples so dividing by 2 for total length)
        //bytes per sample = bit depth * num channels / 8, and this is a 16 bit mono file. This means each audio sample is 2 bytes, therefore input_size(representing number of bytes) / 2 is the amount of samples.
        //Num samples is storing the amount of items read by fread() so we can store our audio sample amount for later.
        size_t num_samples = fread(buffer, sizeof(int16_t), input_size/2, input);

        //grain size in samples
        int chunk_size = atoi(argv[4]); 

        //allows grain size to be greater or less than specified by .5 range (grain size = 1000, range = 500, grain sizes range from 750-1250. This helps avoid rhythm if you don;t want a constant rhythmic pattern.)
        int range = atoi(argv[5]);


        //This is where the magic happens.
        //for the amount of chunks there are, write a chunk from a random point in the buffer (containing the audio data from the input file) to the output in order.
        for (int i = 0; i < num_samples/chunk_size; i++) {
            int random_index = rand() % num_samples;
            fwrite(&buffer[random_index], sizeof(int16_t), (rand()%range -range/2)+chunk_size, output);
        }
//if no scramble arg, just do volume as normal
    }else{
            float factor = atof(argv[3]);
            int16_t buffer;
            while(fread(&buffer, sizeof(int16_t), 1, input)){
            buffer *= factor;
            fwrite(&buffer, sizeof(int16_t), 1, output);
    }
    }

    // Close files
    fclose(input);
    fclose(output);
}



long getFileSize(const char *filename) {
//Open file as binary file, if something is up, print an error and quit.
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

//Seek to the last bit of the file, 
    fseek(file, 0, SEEK_END);
    long size = ftell(file);  // report current file locaton (location of last bit == length of file)
    fclose(file);
    return size; // Return the file size
}
