#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define QUADRATIC_PROBING 1
#define DOUBLE_HASHING 2
#define TABLE_SIZE 223
#define MAX_NEW_WORDS 100

int collisions_quad = 0; // To track collisions in Quadratic Probing
int collisions_double = 0; // To track collisions in Double Hashing
int numberofwords = 0;
int ch;
int num = 0;
float load_factor;

int newsize;
char words[TABLE_SIZE][50];
char meanings[TABLE_SIZE][1000];
int collision = 0;

struct new_word
{
    char word[50];
    char meaning[1000];
};

struct new_word new_words[MAX_NEW_WORDS];
int new_words_count = 0;

struct hash_table
{
    char value[50];
    int status;
    char meaning[3][1000]; // Array to store up to three meanings
};

typedef struct hash_table *table;
table newtable;
struct hash_table *t_quad;
struct hash_table *t_double;

table t_quad, t_double;

/// Initialize the arrays for words and meanings with default values "---"
void init_array()
{
    int i;
    for (i = 0; i < TABLE_SIZE; i++)
    {
        strcpy(words[i], "---");
        strcpy(meanings[i], "---");
    }
}


/// Hash function to calculate the index where the input should be stored in the table
int hash_function(int size, char *input)
{
    int sum = 0;
    for (int i = 0; input[i] != '\0'; i++)
    {
        sum += ((int)input[i] * (i + 1)); // Accumulate a weighted sum of characters in the input string
    }

    return sum % size; // Return the index based on the sum modulo the table size
}


/// Quadratic probing function to resolve collisions
int quad(int i, int input, int size)
{
    return (hash_function(size, input) + (i * i)) % size; // Quadratic probing formula
}


/// Double hashing function to resolve collisions
int double_hash(int i, int input, int size)
{
    int prime = 13; // Choose a prime number for double hashing
    return (hash_function(size, input) + i * (prime - (input % prime))) % size; // Double hashing formula
}


/// Create and initialize a hash table with a given size
table create_table(int size)
{
    table new_table = (struct hash_table *)malloc(size * sizeof(struct hash_table));

    if (new_table == NULL)
    {
        printf("Memory allocation failed.\n");
        exit(1);
    }

    for (int i = 0; i < size; i++)
    {
        strcpy(new_table[i].value, "---");   // Initialize the value field with "---"
        strcpy(new_table[i].meaning, "---"); // Initialize the meaning field with "---"
        new_table[i].status = 0;             // Initialize the status field to 0
    }

    return new_table; // Return the newly created and initialized hash table
}



/// Function to check if the load factor of a hash table exceeds a threshold.
int check_load_factor(int num, int size)
{
    load_factor = (float)num / size;
    if (load_factor >= 0.65)
    {
        newsize = find_prime(2 * size); // Calculate a new prime size for the table.
        return 1; // Signal that resizing is recommended.
    }
    else
    {
        newsize = size; // Keep the current size.
        return 0; // No need to resize.
    }
}

/// Function to find the next prime number greater than n.
int find_prime(int n)
{
    int i;
    int prime = 1;
    int v;
    for (i = 2; i < (n / 2); i++)
    {
        if (n % i == 0)
        {
            prime = 0; // n is not prime.
            v = find_prime(n + 1); // Recursively search for the next prime.
            break;
        }
    }
    if (prime != 0)
        return n; // Return the prime number.
}


/// Function to insert a key-value pair into a hash table with collision handling.
void insert(table tab, int size, char *key, char *meanings, int method, int *collision_counter)
{
    int i = 0;
    int location = hash_function(size, key); // Calculate the initial location.

    // Handle collisions using the specified method (quadratic probing or double hashing).
    while (strcmp(tab[location].value, "---") != 0 && strcmp(tab[location].value, key) != 0)
    {
        (*collision_counter)++; // Increment the collision counter.
        i++;
        if (method == QUADRATIC_PROBING)
        {
            location = quad(i, key, size);
        }
        else
        {
            location = double_hash(i, key, size);
        }
    }

    // Check if it's a new insertion or an update.
    if (strcmp(tab[location].value, "---") == 0)
    {
        strcpy(tab[location].value, key);
        tab[location].status = 0;
    }

    // Split the meanings and store them.
    char *meaningToken = strtok(meanings, "#");
    int currentStatus = tab[location].status;
    while (meaningToken != NULL && currentStatus < 3)
    {
        strcpy(tab[location].meaning[currentStatus], meaningToken);
        currentStatus++;
        meaningToken = strtok(NULL, "#");
    }
    tab[location].status = currentStatus;
}


/// Function to read data from a file and insert it into the hash table.
void read_from_file(table t, int tablesize, int methodChoice, int *collision_counter)
{
    FILE *f = fopen("words.txt", "r");
    char line[1000], word[50], meanings[1000];
    if (f == NULL)
    {
        printf("File is empty or does not exist.\n");
        return;
    }
    while (fgets(line, sizeof(line), f) != NULL)
    {
        if (strcmp(line, "\n") == 0) continue;
        char *poi = strtok(line, "\n");
        int init = 0;
        poi = strtok(line, ":");
        while (poi != NULL)
        {
            if (init % 2 == 0) strcpy(word, poi);
            else strcpy(meanings, poi);
            poi = strtok(NULL, ":");
            init++;
        }
        insert(t, tablesize, word, meanings, methodChoice, collision_counter);
    }
    fclose(f);
}


/// Function to rehash data from the old table to the new table after resizing.
void rehash_function(table oldt, table newt, int method, int *collision_counter)
{
    int i;
    for (i = 0; i < TABLE_SIZE; i++)
    {
        if (strcmp(oldt[i].value, "---") != 0)
        {
            insert(newt, newsize, oldt[i].value, oldt[i].meaning, method, collision_counter);
        }
    }
}


void print_table(table tab, int size)
{
    int j;
    for (j = 0; j < size; j++)
    {
        printf("\n ***location (%d): \t word: %s", j, tab[j].value);

        if (tab[j].status > 0)
        {
            printf("\t meaning: ");
            for (int m = 0; m < tab[j].status; m++)
            {
                printf("%s", tab[j].meaning[m]);
                if (m < tab[j].status - 1)
                    printf(", ");
            }
        }
    }
    printf("\n\n");
}

/// Function to print the contents of a table to a file.
void print_to_file(table tab, int size, FILE *fw)
{
    int j;
    for (j = 0; j < size; j++)
    {
        fprintf(fw, "\n ***location (%d): \t word: %s", j, tab[j].value);

        if (tab[j].status > 0)
        {
            fprintf(fw, "\t meaning: ");
            for (int m = 0; m < tab[j].status; m++)
            {
                fprintf(fw, "%s", tab[j].meaning[m]);
                if (m < tab[j].status - 1)
                    fprintf(fw, ", ");
            }
        }
    }
    fprintf(fw, "\n\n");
}

/// Function to print words and meanings from a table, excluding entries with "---" as a meaning.
void print_no_spaces(table tab, int size)
{
    int j = 0;
    int i = 0;
    // Loop over each item in the table
    for (j = 0; j < size; j++)
    {
        // Check if the meaning is not "---" (indicating an unused or deleted entry)
        if (strcmp(tab[j].meaning, "---") != 0)
        {
            i++;
            // Print the word and its meaning to the console
            printf("\n(%d)\t  WORD: %s", i, tab[j].value);
            printf("\t\t MEANING: %s", tab[j].meaning);
        }
    }
    // Print the total count of valid words
    printf("\n\n");
    printf("# of values: %d\n", i);
}

/// Function to add a new word and its meaning to a global array of new words.
void add_new_word(const char* word, const char* meaning)
{
    // Check if there is space for new words
    if (new_words_count < MAX_NEW_WORDS)
    {
        // Add the new word and its meaning to the array
        strcpy(new_words[new_words_count].word, word);
        strcpy(new_words[new_words_count].meaning, meaning);
        new_words_count++;
    }
    else
    {
        // Print a message if the maximum capacity is reached
        printf("Reached maximum capacity of new words\n");
    }
}

/// Function to print the original table of words followed by any new words added.
void print_table_with_new_words(table t, int size)
{
    // Print the original table
    print_table(t, size);

    // Loop over the new words and print them
    for (int i = 0; i < new_words_count; i++)
    {
        printf("!!!!location (New): \t word: %s \t meaning: %s\n", new_words[i].word, new_words[i].meaning);
    }
}

/// Function to insert a new word into the table, using a specific insertion method.
table insert_new(table newt, int size, char *key, char *meaning, int method, int *collision_counter)
{
    // Increment the count of total words
    numberofwords++;
    // Insert the new word into the table
    insert(newt, size, key, meaning, method, collision_counter);
    return newt;
}

/// Function to search for a word in the table and return its location.
int search_element(table tab, int size, char *key)
{
    int i = 0;
    int location;
    // Loop over each element in the table
    for (i = 0; i < size; i++)
    {
        // Check if the current element matches the key
        if (strcmp(tab[i].value, key) == 0)
        {
            location = i;
            break;
        }
        else
        {
            location = -1; // Set location to -1 if not found
            continue;
        }
    }

    return location; // Return the location of the word or -1 if not found
}

/// Function to delete a word from the table.
void delete_element(table tab, int size, char *key)
{
    // Find the location of the word to delete
    int s = search_element(tab, size, key);
    if (s != -1)
    {
        // If found, print the word
        printf("\t-->WORD to delete: %s \n", tab[s].value);
        // Mark the word and its meaning as "---" to indicate deletion
        strcpy(tab[s].value, "---");
        strcpy(tab[s].meaning, "---");
        tab[s].status = 0; // Set the status to 0 (unused)
    }
    else
        // If not found, print a message
        printf("word (%s) not found, could not delete\n ", key);
}

/// Function to replace empty slots in the table with words from a separate array.
void change_table(table tab)
{
    int j = 0;
    int k = 0;
    // Loop over the entire table
    for (j = 0; j < TABLE_SIZE; j++)
    {
        // If a slot in the table is marked as empty
        if (strcmp(tab[j].value, "---") == 0)
        {
            while (k < TABLE_SIZE)
            {
                // If the next slot in the 'words' array is not empty
                if (strcmp(words[k], "---") != 0)
                {
                    // Copy the word and its meaning to the table
                    strcpy(tab[j].value, words[k]);
                    strcpy(tab[j].meaning, meanings[k]);
                    tab[j].status = 1; // Set the status to 1 (used)
                    k++;
                }
                else
                {
                    // If the 'words' array slot is empty, move to the next one
                    k++;
                    continue;
                }
                break; // Break after filling one slot
            }
        }
    }
}

int main()
{
    int choice, tableChoice;
    t_quad = create_table(TABLE_SIZE);
    t_double = create_table(TABLE_SIZE);

    read_from_file(t_quad, TABLE_SIZE, QUADRATIC_PROBING, &collisions_quad);
    read_from_file(t_double, TABLE_SIZE, DOUBLE_HASHING, &collisions_double);



    FILE *fw;
    int s;

    while (1)
    {
        int choice, tableChoice;
        char readword[50], readmeaning[100];

        printf("\nMENU:\n");
        printf("1. Print hashed tables (including empty spots).\n");
        printf("2. Print out table size and the load factor.\n");
        printf("3. Print out the used hash functions.\n");
        printf("4. Insert a new record to hash table (insertion will be done on both hash tables).\n");
        printf("5. Search for a specific word (specify which table to search in).\n");
        printf("6. Delete a specific word (from both tables).\n");
        printf("7. Compare between the two methods in terms of the number of collisions occurred.\n");
        printf("8. Save hash table back to a file named saved_words.txt (of the double hashing).\n");
        printf("9. EXIT.\n");

        scanf("%d", &choice);
        getchar();

        switch (choice)
        {
        // Case 1: Print the contents of both the Quadratic Probing and Double Hashing tables.
        case 1:
        {
            // Print the Quadratic Probing table with all its contents.
            printf("\nQuadratic Probing Table:\n");
            print_table(t_quad, TABLE_SIZE);
            // Print the Double Hashing table with all its contents.
            printf("\nDouble Hashing Table:\n");
            print_table(t_double, TABLE_SIZE);
        }
        break;

        // Case 2: Display the size and load factor of the selected hash table.
        case 2:
            // Prompt the user to choose which method's table size and load factor to display.
            printf("\nChoose the method to display Table Size and Load Factor:\n1. Quadratic Probing\n2. Double Hashing\n3. Exit this option\n");
            scanf("%d", &tableChoice);
            getchar();  // Clear the newline character left in the input buffer.

            // Validate user's choice and proceed to display table size and load factor.
            if (tableChoice == 1 || tableChoice == 2)
            {
                // Select the appropriate table based on user's choice.
                table t;
                if (tableChoice == 1)
                {
                    t = t_quad;
                }
                else
                {
                    t = t_double;
                }


                // Count the number of occupied slots in the table.
                int occupied = 0;
                for (int i = 0; i < TABLE_SIZE; i++)
                {
                    if (strcmp(t[i].value, "---") != 0) occupied++;
                }
                // Calculate the load factor (number of occupied slots divided by table size).
                float load_factor = (float)occupied / TABLE_SIZE;

                // Print the chosen table's size and load factor.
                printf("\n%s Table:\n", (tableChoice == 1) ? "Quadratic Probing" : "Double Hashing");
                printf("Table Size: %d\n", TABLE_SIZE);
                printf("Load Factor: %0.2f\n", load_factor);
            }
            else if (tableChoice != 3)  // Check if the user chose to exit.
            {
                // Prompt the user to make a valid choice if an invalid option was selected.
                printf("Invalid choice. Please choose 1, 2, or 3.\n");
            }
            break;

        // Case 3: Display the hash function used by the selected hashing method.
        case 3:
            // Loop until the user chooses to exit this option.
            do
            {
                // Prompt the user to choose which hashing method's function to display.
                printf("\nChoose the hashing method to display its hash function:\n");
                printf("1. Quadratic Probing\n");
                printf("2. Double Hashing\n");
                printf("3. Exit this option\n");
                scanf("%d", &ch);
                getchar();  // Clear the newline character left in the input buffer.

                // Display the hash function based on the user's choice.
                switch (ch)
                {
                case 1:
                    // Display the hash function formula for Quadratic Probing.
                    printf("Hash Function used for Quadratic Probing:\n");
                    printf("h(key, i) = (hash_function(key) + i^2) %% table_size\n");
                    printf("Where hash_function(key) is the primary hash function.\n");
                    break;
                case 2:
                    // Display the hash function formula for Double Hashing.
                    printf("Hash Function used for Double Hashing:\n");
                    printf("h(key, i) = (hash_function(key) + i * (prime - (key %% prime))) %% table_size\n");
                    printf("Where hash_function(key) is the primary hash function and prime is a constant prime number.\n");
                    break;
                case 3:
                    // Print a message and exit the loop if the user chooses to exit this option.
                    printf("Exiting...\n");
                    break;
                default:
                    // Prompt the user to make a valid choice if an invalid option was selected.
                    printf("Invalid choice. Please choose 1, 2, or 3.\n");
                }
            }
            while (ch != 3);  // Continue the loop until the exit option is chosen.
            break;


        case 4:
            printf("Word to insert: ");
            fgets(readword, 50, stdin);
            strtok(readword, "\n");

            printf("Meaning: ");
            fgets(readmeaning, 1000, stdin);
            strtok(readmeaning, "\n");

            // Check if the word already exists in either hash table
            int foundInQuad = search_element(t_quad, TABLE_SIZE, readword) != -1;
            int foundInDouble = search_element(t_double, TABLE_SIZE, readword) != -1;

            if (!foundInQuad && !foundInDouble)
            {
                // Word not found in both tables, proceed to insert
                add_new_word(readword, readmeaning);

                collision = 0;
                insert(t_quad, TABLE_SIZE, readword, readmeaning, QUADRATIC_PROBING, &collision);
                printf("After insertion using Quadratic Probing:\n");
                print_table(t_quad, TABLE_SIZE);
                printf("Collisions in Quadratic Probing: %d\n", collision);

                collision = 0;
                insert(t_double, TABLE_SIZE, readword, readmeaning, DOUBLE_HASHING, &collision);
                printf("After insertion using Double Hashing: \n");
                print_table(t_double, TABLE_SIZE);
                printf("Collisions in Double Hashing: %d\n", collision);
            }
            else
            {
                // Word already exists in at least one table
                printf("The word '%s' already exists in the hash table(s).\n", readword);
            }
            break;

        case 5:
            printf("Enter a word to search for:\t");
            fgets(readword, 50, stdin);
            strtok(readword, "\n");

            int searchTable;
            printf("Specify which table to search in (1 for Quadratic Probing, 2 for Double Hashing): ");
            scanf("%d", &searchTable);
            getchar();

            table t = (searchTable == 1) ? t_quad : t_double;
            int size = (searchTable == 1) ? TABLE_SIZE : TABLE_SIZE;

            s = search_element(t, size, readword);

            if (s == -1)
            {
                printf("\t!!! Word (%s) not found\n", readword);
            }
            else
            {
                printf("\n!!FOUND\n---->WORD: %s\n", t[s].value);
                printf("---->MEANING: %s\n\n", t[s].meaning[0]);
            }
            break;


        case 6:
            printf("Enter a word to delete:\t");
            fgets(readword, 50, stdin);
            strtok(readword, "\n"); // Remove newline character from input

            // Search for the word in both tables
            int foundInQuad1 = search_element(t_quad, TABLE_SIZE, readword) != -1;
            int foundInDouble1 = search_element(t_double, TABLE_SIZE, readword) != -1;

            if (foundInQuad1 || foundInDouble1)
            {
                // If found, delete from Quadratic Probing table
                if (foundInQuad1)
                {
                    delete_element(t_quad, TABLE_SIZE, readword);
                }

                // If found, delete from Double Hashing table
                if (foundInDouble1)
                {
                    delete_element(t_double, TABLE_SIZE, readword);
                }

                printf("Word (%s) deleted successfully from both tables.\n", readword);
            }
            else
            {
                printf("Word (%s) not found in either table.\n", readword);
            }

            // Print Quadratic Probing table to confirm deletion
            printf("\nQuadratic Probing Table after deletion:\n");
            print_table(t_quad, TABLE_SIZE);

            // Print Double Hashing table to confirm deletion
            printf("\nDouble Hashing Table after deletion:\n");
            print_table(t_double, TABLE_SIZE);

            break;


        case 7:
            printf("\n*****\t COLLISION COMPARISON\n\n");
            printf("Number of collisions for Quadratic Probing: %d\n", collisions_quad);
            printf("Number of collisions for Double Hashing: %d\n", collisions_double);
            break;


        case 8:
        {
            fw = fopen("saved_words.txt", "w");
            if (fw == NULL)
            {
                printf("Error opening file for writing.\n");
            }
            else
            {
                // Save the Double Hashing table (t_double) to the file
                print_to_file(t_double, TABLE_SIZE, fw);
                printf("Double Hashing Table saved to saved_words.txt successfully.\n");
                fclose(fw);
            }
            break;
        }


        case 9:
            exit(0);

        default:
            printf("Invalid choice. Choose operation numbered (1-10)\n");
            break;
        }
    }

    return 0;
}



