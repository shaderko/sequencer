/**
 * @file io.c
 * @author shaderko
 * @brief Module implementation for working with files
 * @version 0.1
 * @date 2024-09-12
 *
 * @copyright Copyright (c) 2024
 *
 */

#include <io.h>

#include <logger.h>

// define error codes
#define FILE_ERROR 1
#define MEMORY_ERROR 2
#define READ_ERROR 3
#define WRITE_ERROR 3
#define INVALID_DATA_POINTER 4

// clear a file
void clear_file(const char *path)
{
    FILE *file = fopen(path, "w");
    if (!file)
    {
        log_message(LOG_ERROR, "Failed to open file for writing");
        return;
    }

    fclose(file);
}

int save(Serialized data, const char *path)
{
    if (data.size <= 0 || !data.buffer)
    {
        log_message(LOG_ERROR, "Invalid data to save");
        return INVALID_DATA_POINTER;
    }

    FILE *file = fopen(path, "ab");
    if (!file)
    {
        log_message(LOG_ERROR, "Failed to open file for writing");
        return FILE_ERROR;
    }

    // write the size of the serialized data first
    if (fwrite(&data.size, sizeof(size_t), 1, file) < 1)
    {
        log_message(LOG_ERROR, "Failed to write serialized data size");
        fclose(file);
        return WRITE_ERROR;
    }

    // write the serialized data
    if (fwrite(data.buffer, sizeof(unsigned char), data.size, file) < data.size)
    {
        log_message(LOG_ERROR, "Failed to write serialized data");
        fclose(file);
        return WRITE_ERROR;
    }

    // close the file
    fclose(file);

    return 0;
}

int save_all(Serialized *data_array, size_t count, const char *path)
{
    if (!data_array || count == 0)
    {
        log_message(LOG_ERROR, "Invalid data array or count");
        return INVALID_DATA_POINTER;
    }

    // Iterate over each Serialized object in the array and save it
    for (size_t i = 0; i < count; ++i)
    {
        int result = save(data_array[i], path);
        if (result != 0)
        {
            log_message(LOG_ERROR, "Failed to save serialized data at index %zu", i);
            return result; // Return the error code from the `save` function
        }
    }

    return 0; // Success
}

// function to get the sizes of all the serialized data in file
// sizes needs to be allocated before calling this function so does count
int get_serialized_sizes(size_t **sizes, size_t *count, const char *path)
{
    FILE *file = fopen(path, "rb");
    if (!file)
    {
        log_message(LOG_ERROR, "Failed to open file for reading");
        return FILE_ERROR;
    }

    size_t objects_count = 0;
    size_t allocated_count = 10;

    *sizes = (size_t *)malloc(allocated_count * sizeof(size_t));
    if (!*sizes)
    {
        log_message(LOG_ERROR, "Failed to allocate memory for sizes array");
        fclose(file);
        return MEMORY_ERROR;
    }

    while (!feof(file))
    {
        size_t size;

        // read the size of the serialized data
        if (fread(&size, sizeof(size_t), 1, file) < 1)
        {
            if (feof(file))
                break;

            log_message(LOG_ERROR, "Failed to read serialized data size");
            free(*sizes);
            fclose(file);
            return READ_ERROR;
        }

        // add the size to the array
        if (objects_count >= allocated_count)
        {
            // resize the array if necessary
            allocated_count *= 2;
            *sizes = (size_t *)realloc(*sizes, allocated_count * sizeof(size_t));
            if (!*sizes)
            {
                log_message(LOG_ERROR, "Failed to reallocate memory for sizes array");
                fclose(file);
                return MEMORY_ERROR;
            }
        }

        (*sizes)[objects_count] = size;
        objects_count++;

        // skip the actual serialized data by seeking forward
        if (fseek(file, size, SEEK_CUR) != 0)
        {
            log_message(LOG_ERROR, "Failed to seek past serialized data");
            free(*sizes);
            fclose(file);
            return READ_ERROR;
        }
    }

    *count = objects_count;

    fclose(file);

    return 0;
}

int load(Serialized *data, const char *path, size_t start)
{
    if (!data)
    {
        log_message(LOG_ERROR, "Invalid data pointer");
        return INVALID_DATA_POINTER;
    }

    FILE *file = fopen(path, "rb");
    if (!file)
    {
        log_message(LOG_ERROR, "Failed to open file for reading");
        return FILE_ERROR;
    }

    // seek to the start of the serialized data
    if (fseek(file, start, SEEK_SET) != 0)
    {
        log_message(LOG_ERROR, "Failed to seek to start of serialized data");
        fclose(file);
        return READ_ERROR;
    }

    // read the size of the serialized data first
    size_t size;
    if (fread(&size, sizeof(size_t), 1, file) != 1)
    {
        log_message(LOG_ERROR, "Failed to read serialized data size");
        fclose(file);
        return READ_ERROR;
    }

    // allocate memory for the serialized data
    char *buffer = (char *)malloc(size);
    if (!buffer)
    {
        log_message(LOG_ERROR, "Failed to allocate memory for serialized data");
        fclose(file);
        return MEMORY_ERROR;
    }

    // read the serialized data
    if (fread(buffer, sizeof(unsigned char), size, file) != size)
    {
        log_message(LOG_ERROR, "Failed to read serialized data");
        free(buffer);
        fclose(file);
        return READ_ERROR;
    }

    // close the file
    fclose(file);

    // assign the buffer to the data structure
    data->buffer = buffer;
    data->size = size;

    return 0;
}

int load_all(Serialized ***data, size_t *count, const char *path)
{
    if (!data)
    {
        log_message(LOG_ERROR, "Invalid data pointer");
        return INVALID_DATA_POINTER;
    }

    // get the sizes of all the serialized data in file
    size_t *sizes;
    int status = get_serialized_sizes(&sizes, count, path);
    if (status != 0)
    {
        log_message(LOG_ERROR, "Failed to get serialized data sizes");
        return status;
    }

    // allocate memory for the serialized data
    *data = (Serialized **)malloc(*count * sizeof(Serialized *));
    if (!*data)
    {
        log_message(LOG_ERROR, "Failed to allocate memory for serialized data");
        free(sizes);
        return MEMORY_ERROR;
    }

    size_t starting_size = 0;

    // load the serialized data
    for (size_t i = 0; i < *count; i++)
    {
        Serialized *serialized = (Serialized *)malloc(sizeof(Serialized));
        if (!serialized)
        {
            log_message(LOG_ERROR, "Failed to allocate memory for serialized data");
            free(sizes);
            for (size_t j = 0; j < i; j++)
            {
                free((*data)[j]->buffer);
                free((*data)[j]);
            }
            free(*data);
            return MEMORY_ERROR;
        }

        status = load(serialized, path, starting_size);
        if (status != 0)
        {
            log_message(LOG_ERROR, "Failed to load serialized data");
            free(sizes);
            for (size_t j = 0; j < i; j++)
            {
                free((*data)[j]->buffer);
                free((*data)[j]);
            }
            free(*data);
            return MEMORY_ERROR;
        }

        (*data)[i] = serialized;

        starting_size += sizes[i];
    }

    free(sizes);

    return 0;
}
