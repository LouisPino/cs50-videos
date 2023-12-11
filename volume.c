// This program modifies the volume of an audio file

#include <stdint.h> // for our int data types
#include <stdio.h> // for our file operations
#include <stdlib.h> // for printf

/////////GET ALL OUR DUCKS IN A ROW/////////

// Number of bytes in a .wav header
const int HEADER_SIZE = 44;

 //pass in command-line arguments as arguments for the main function
 //argc is the amount (includes program name)
 //argv[] is an array of char *'s (aka strings) containing all command-line arguments (includes program name)
int main(int argc, char *argv[])
{
    if (argc != 4) // Check command-line arguments for correct amount
    {
        printf("Usage: ./volume input.wav output.wav factor\n");
        return 1;
    }

    // Open input file
    // FILE is a data type defined in <stdio.h>, it is a pointer to a file you open.
    //fopen() gets the file with the name specified in your first arg ("input.wav")
    //"r" specifies that you will be reading this file ("rb" would indicate reading as a binary file)
    FILE *input = fopen(argv[1], "r");
    if (input == NULL)
    {
        printf("Could not open file.\n"); // error handling if something goes wrong
        return 1;
    }
     // Open output file, if it doesn't exist, creates it.
     //"w" inidicated we will be writing to this file (aka replacing the whole thing, not just adding to it.)
    FILE *output = fopen(argv[2], "w");
    if (output == NULL)
    {
        printf("Could not open file.\n"); // error handling if something goes wrong
        return 1;
    }

    // Get our volume scaling factor from the command-line argument
    float factor = atof(argv[3]);



/////////COPY THE HEADER FROM OUR INPUT FILE TO OUR OUTPUT FILE/////////

    // TODO: Copy header from input file to output file
    //Create an array of unsigned 8bit ints HEADER_SIZE long (44 items in this case). Name it header.
    //This will be used to temporarily store the header from the input so we can later write it to the output.
    uint8_t header[HEADER_SIZE];

    //Read FROM input TO header.
    //I want to take HEADER_SIZE (44) items that are each the size of an unsigned 8bit integer from the beignning of the input file and copy them to the header array.
    //The in and out of fread and fwrite must be pointers. We don't need a * because the name of an array is already a pointer to the first itme, and the variables input and output are the FILE data type, which is also just a pointer to the file's location in memory.
    //You COULD copy directly from input to output, but there are a few reasons why it is generally not a good idea to do so, mostly coming down to scalability/flexibility. If you need to check for validity of or make any changes to the header in the future, here is where it would be done. 
    fread(header, sizeof(uint8_t), HEADER_SIZE, input); 

    //Write FROM header TO output.
    //I want to take HEADER_SIZE (44) items that are each the size of an unsigned 8bit integer (1 byte) from the header array and copy them to the beginning of the output file.
    //Make note of the ordering in fread() and fwrite(). Reads from right to left, wrotes from left to write.
    fwrite(header, sizeof(uint8_t), HEADER_SIZE, output);



    //COPY EACH AUDIO SAMPLE FROM OUR INPUT FILE TO OUR OUTPUT FILE
    //TODO: Read samples from input file and write updated data to output file

    //Create a variable named buffer (just some temporary storage), it will hold a 16bit signed integer(which is the size of an audio sample in our case, or 2 bytes).
    //bytes per sample = bit depth * num channels / 8,
    int16_t buffer;

    //Read from our input file exactly 1 item, that item will be the size of a 16bit int (one 2ch audio sample).
    //Because our buffer is a variable value and not a pointer, we need to specify the ADDRESS of our buffer variable in the fread() function.
    //fread() will return the amount of items read. This allows us to read through the whole file until the very end, returning 1 for rach audio sample, until we eventually return 0 at the end of the file and break the loop.
    while(fread(&buffer, sizeof(int16_t), 1, input)){

    //After reading one audio sample into the buffer, we multiply it by our volume scaling factor.
    //Digital audio is as simple as that! It is just a stream of numerical values, representing the physical push and pull of your speaker. Multiply by 2 and you push/pull twice as far, and therefore twice as loud. By .5, half as loud. You can also slow down the speed at which you read these values, leading to something one octave lower (half speed) or one octave higher (double speed), but that's for another day
    buffer *= factor;

    //Now we write the newly multiplied buffer value into our output file. Again, we need a pointer for our buffer's location in the fwrite() function, but no * necessary for ouput, as it is already a pointer to the output file's location.
    fwrite(&buffer, sizeof(int16_t), 1, output);
    }


//Things to discuss:
//There is an internal counter for each file, letting fread and other f operations know where exactly in the file you are. This can be modified sing the fseek() function. Because reading and writing the header is the first thing we do, these operations happen on the first 44 bytes of those files. Because the very next thing we do with these files is read and write them, we are already at the next point in the file just after the header, so we don't need to specify exactly where the read and write in the file. We also do not need to specify the size of the output file, as C will dynamically write this for us. Basically what happens is that C writes the data to a temporary buffer under the hood, then when we call fclose() it will actually create the file and write that entire buffer to it. All we need to do is close it when we're done.



    //CLOSE OUR FILES, WE'RE DONE!
    fclose(input);
    fclose(output);
}
